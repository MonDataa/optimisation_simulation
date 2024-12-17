// Projet   : Hillclimber lançant Scilab en langage C/C++ en gérant les entrées/sorties
// Fichier  : hillclimber.cpp
// Auteur   : D. Duvivier (c) 2022..2025 UPHF/INSA
// Licence  : Utilisable dans le cadre des enseignements en optimisation-simulation en Master TNSID
// Version  : 0.0.2
// Date     : 23/10/2024
//          :
// Limites  : Le hillclimber est trop "simpliste" comme méthode d'optimisation
//
// Contenu  : hillclimber de base pour tester un exemple d'optimisation-simulation
//
// Remarque : Afin de pouvoir tester plusieurs fonctions coûts analytiques ou dont la valeur
//          : est issue d'un mécanisme d'évaluation par simulation,
//          : les "fonctions coût utilisables" sont numérotées et elles conditionnent finalement
//          : tout le "problème cible résolu" avec un impact sur :
//          : - le domaine de définition de la fonction (i.e. Xmin...Xmax)
//          : - le calcul de la solution initiale
//          : - l'opérateur de déplacement
//          : - une valeur objectif à atteindre ou approcher (Ylimit)
//          : - ATTENTION : Il est supposé dans tous les cas que le HC cherche à réduire
//          :   l'écart entre le coût C(s) et Ylimit. La valeur de Ylimit détermine si l'on
//          :   maximise ou si l'on minimise le coût.
//          :   Dit simplement, si le coût est >= 0, en supposant Ylimit positif ou nul :
//          :   * si Ylimit >> 0 on maximise le coût ;
//          :   * si Ylimit petit (supposé positif ou nul) ou minimise le coût..
//
// TODO     : Les codes de retour sont mal ou incorrectement gérés ("exit()" utilisé trop souvent)
// TODO     : Le code devrait être intégralement converti du C au C++
//
#include <QtGlobal>
#include <QTime>
#include <QDateTime>
#include <iostream>
#include <tuple>
#include <cmath>
//#include <QMessageBox>
//#include <cstdio>
#include "globalutils.h"
#include "hillclimber.h"

#include <set> // Pour mémorisation des solutions explorées
#include <vector>
#include <algorithm>
#include <cmath>

// Pour la [prochaine] version avec une escadrille de HillClimbers :
// Nombre de hillclimbers lancés (idéalement en parallèle)
#define NBHC 7

// Pour la [prochaine] version avec une escadrille de HillClimbers :
// Autre critère d'arrêt : nombre maximum d'itérations sur un plateau,
// si cette constante est initialisée à 0, il n'est pas possible de se déplacer sur un plateau
#define MAXITERPLATEAU 100


// Nombre de problèmes sur lesquels tester le hillclimber
// (c'est le nombre de fonctions coût différentes et/ou de paramétrages différents)
#define NRPROBLEMES 3

// Numéro du problème que le hillclimber tente de résoudre actuellement
// (attention le premier problème, i.e. première fonction coût & cie,
// a le numéro 0 et non 1).
// ATTENTION : le problème 0 est supposé être le seul à utiliser l'optimisation-simulation
// via scilab, les autres servent à tester/calibrer le hillclimber et ses composants

// Critère d'arrêt = nombre maximum d'itérations
const int MAXITERATIONS[NRPROBLEMES] = {
    100,   // Pour le problème 0 : optimisation-simulation via scilab
    30000, // Pour le problème 1 : sinus en marche d'escalier sur [0..20]
    30000  // Pour le problème 2 : sinus en marche d'escalier et plateaux sur [0..20]
};

// Suite du critère d'arrêt : arret si gain entre 2 meilleures solutions consécutives intérieur à ce seuil
const double SEUILARRET[NRPROBLEMES] = {
    0.00001,   // Pour le problème 0 : optimisation-simulation via scilab
    0.000001,       // Pour le problème 1 : sinus en marche d'escalier sur [0..20]
    0.05       // Pour le problème 2 : sinus en marche d'escalier et plateaux sur [0..20]
};

// Variation élémentaire induite par l'opérateur de déplacement/mouvement
const double DELTA[NRPROBLEMES] = {
    0.01,    // Pour le problème 0 : optimisation-simulation via scilab
    0.00025,   // Pour le problème 1 : sinus en marche d'escalier sur [0..20]
    0.00025    // Pour le problème 2 : sinus en marche d'escalier et plateaux sur [0..20]
};

// Domaine de définition de la fonction coût :
// entre XMIN (XMINMAX[NRPB][0]) et XMAX20.0 (XMINMAX[NRPB][1])
const double XMINMAX[NRPROBLEMES][2] = {
    {1.0, 10.0}, // Pour le problème 0 : optimisation-simulation via scilab
    {0.0, 20.0}, // Pour le problème 1 : sinus en marche d'escalier sur [0..20]
    {0.0, 20.0}  // Pour le problème 2 : sinus en marche d'escalier et plateaux sur [0..20]
};

// Valeur limite dont on cherche à s'approcher itérativement
const double YLIMIT[NRPROBLEMES] = {
    35.65636,    // NRPB 0: Adjusted target velocity (in km/h) -> Configuration B
    21.22,   // NRPB 1
    22.00    // NRPB 2
};

// Constructeur sans paramètre
// -> initialise la graine du PRNG
// -> force le nombre de lignes lues depuis un fichier .sce à -1
//    pour indiquer, pas de fichier .sce (optimisation d'une fonction "analytique")
// => Peut être faudrait-il vérifier si la valeur de NRPB est bien entre 0 et NRPROBLEMES-1 ????

// Fixe/définit/modifie et lit le fichier .sce
// utilisé pour l'optimisation-simulation
// -> lit le fichier .sce
// -> met à jour le nombre de lignes lues dans "lignes"
//    (optimisation-simulation avec Scilab)
void hillclimber::setSceFile(const char Nom[]) {
    lignes = -1; // Reset the number of lines
    if (slchc.LireLignesFichier(Nom, lignesfichier, &lignes) < 0)
        AfficheErreur("LireLignesFichier: erreur lors de la lecture du fichier", STOPEXIT);
}


// Cost function for optimization simulation
/*double hillclimber::fonctionCoutOptiSim(double s) {
    double y = -1.0;
    sprintf(lignesfichier[6], "t = %.2lf ;\n", s);
    if (slchc.EcrireLignesFichier("optisimparachute.sce", lignesfichier, lignes) < 0)
        AfficheErreur("EcrireLignesFichier: erreur lors de l'écriture dans le fichier", STOPEXIT);
    y = slchc.EvaluerParSimulationViaScilab();
    return y;
}*/
double hillclimber::fonctionCoutOptiSim(double s) {
    double y = -1.0;
    sprintf(lignesfichier[6], "t = %.2lf ;\n", s);
    if (slchc.EcrireLignesFichier("optisimparachute.sce", lignesfichier, lignes) < 0)
        AfficheErreur("EcrireLignesFichier: erreur lors de l'écriture dans le fichier", STOPEXIT);
    y = slchc.EvaluerParSimulationViaScilab();

    // Valider la vitesse limite théorique après la simulation
    double masse = 75.0; // Exemple: remplacer par une valeur en fonction du problème
    if (!slchc.validerVitesseTheorique(y, masse)) {
        printf("Attention: Validation échouée pour la masse %.2f kg\n", masse);
    }

    return y;
}


// Cost function for the current problem
double hillclimber::fonctionCout(double s) {
    switch (currentProblem) {
    case 0: return fonctionCoutOptiSim(s);
    case 1: return s + 1.5 * sin(2 * s);
    case 2: return floor(s + 1.5 * sin(2 * s));
    default:
        AfficheErreur("Invalid problem number", STOPEXIT);
        return -1;
    }
}



// Generate an initial solution
/*double hillclimber::solutionInitiale(void) {
    return XMINMAX[currentProblem][0]
           + myprng.generateDouble() * (XMINMAX[currentProblem][1] - XMINMAX[currentProblem][0]);
}*/


// Retourne une solution voisine s' de s
//
// Une chance sur deux d'ajouter DELTA[NRPB]
// une chance sur deux de retrancher DELTA[NRPB]
// + vérification que l'on reste entre [XMINMAX[NRPB][0] et XMINMAX[NRPB][1]]
//
// Remarque : c'est trop simpliste car risque d'osciller entre deux solutions
//
double hillclimber::operateurDeplacement(double s) {
    if (s + DELTA[currentProblem] > XMINMAX[currentProblem][1])
        return s - DELTA[currentProblem];
    if (s - DELTA[currentProblem] < XMINMAX[currentProblem][0])
        return s + DELTA[currentProblem];
    return (myprng.generateDouble() >= 0.5) ? s + DELTA[currentProblem] : s - DELTA[currentProblem];
}


#include <cmath>
#include <vector>
#include <algorithm>
#include <numeric>
// Helper functions for statistics
double calculateAverage(const std::vector<double>& values) {
    return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
}

double calculateStdDev(const std::vector<double>& values, double mean) {
    double sum = 0.0;
    for (const auto& value : values) {
        sum += std::pow(value - mean, 2);
    }
    return std::sqrt(sum / values.size());
}

/*double hillclimber::solutionInitiale(void) {
    double initial = XMINMAX[currentProblem][0]
                     + myprng.generateDouble() * (XMINMAX[currentProblem][1] - XMINMAX[currentProblem][0]);
    if (currentProblem == 2 && floor(initial + 1.5 * sin(2 * initial)) == YLIMIT[2]) {
        // Recalculer si la solution est déjà sur un plateau optimal
        return solutionInitiale();
    }
    return initial;
}*/

double hillclimber::solutionInitiale(void) {
    double initial = XMINMAX[currentProblem][0]
                     + myprng.generateDouble() * (XMINMAX[currentProblem][1] - XMINMAX[currentProblem][0]);
    // Recalculer si la solution initiale tombe directement sur un plateau (NRPB = 2 uniquement)
    if (currentProblem == 2 && floor(initial + 1.5 * sin(2 * initial)) == YLIMIT[2]) {
        return solutionInitiale(); // Recalcule une nouvelle solution initiale
    }
    return initial;
}



// Fonction utilitaire pour générer un nombre suivant une distribution normale
double generateGaussian(double mean, double stddev) {
    static QRandomGenerator* generator = QRandomGenerator::global();
    double u1 = generator->generateDouble(); // Uniforme [0,1]
    double u2 = generator->generateDouble(); // Uniforme [0,1]
    double z0 = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * M_PI * u2);
    return z0 * stddev + mean;
}

    /*--------------------------------------Travail 3------------------------------------------------------
    => A :
    # Résultats de l'exécution :
      - Solution finale (sf) : 8.08315
      - Coût final (Csf) : 35.6744
      - Nombre d'itérations : 2
      - Itérations sur plateaux : 0
    === Résultats pour 30 exécutions ===
    Moyenne des solutions : 5.410
    Écart-type des solutions : 2.312
    Moyenne des coûts : 35.674

    --------------------------------------------------------------------------------------------
    => B :
    # Résultats de l'exécution :
      - Solution finale (sf) : 8.27207
      - Coût final (Csf) : 43.7569
      - Nombre d'itérations : 2
      - Itérations sur plateaux : 0
    === Résultats pour 30 exécutions ===
    Moyenne des solutions : 5.168
    Écart-type des solutions : 2.628
    Moyenne des coûts : 43.757

    --------------------------------------------------------------------------------------------
    => C  :
    # Résultats de l'exécution :
      - Solution finale (sf) : 9.09162
      - Coût final (Csf) : 39.9095
      - Nombre d'itérations : 2
      - Itérations sur plateaux : 0
    === Résultats pour 30 exécutions ===
        Moyenne des solutions : 5.300
        Écart-type des solutions : 2.685
        Moyenne des coûts : 39.910

*/

void hillclimber::run(QPlainTextEdit *plainTextEdit, double *sf, double *Csf, int *itersf, int *plateauItersf) {
    plainTextEdit->appendPlainText(QStringLiteral("=== Starting Hill Climber for Problem: %1 ===").arg(currentProblem));
    plainTextEdit->repaint();

    // Initialisation
    double solution = solutionInitiale();
    double cost = fonctionCout(solution);
    double Es = fabs(cost - YLIMIT[currentProblem]);

    int iteration = 0;
    int stagnantIterations = 0;       // Compteur pour la durée sur un plateau
    int plateauCount = 0;             // Nombre total de plateaux détectés
    int totalPlateauIterations = 0;   // Nombre total d'itérations sur plateaux
    int limitReachedCount = 0;        // Compteur pour les fois où la limite est atteinte
    const double LIMIT_TOLERANCE = 0.0001; // Tolérance pour déterminer si la limite est atteinte

    std::vector<int> plateauDurations;    // Durées des plateaux
    std::vector<double> plateauSolutions; // Solutions atteintes sur plateaux
    double adaptiveDelta = DELTA[currentProblem]; // Valeur adaptative pour DELTA
    const double MAX_DELTA = 2.0; // Limite maximale pour DELTA

    std::set<double> visitedSolutions; // Ensemble des solutions explorées
    std::vector<double> costHistory;   // Historique des coûts pour guider les sauts


    // Déterminer le nombre maximal d'itérations pour NRPB 0
    int maxIterations = (currentProblem == 0) ? 2 : MAXITERATIONS[currentProblem];

    // Boucle principale
    while (iteration < maxIterations) {
        // Génération d'une nouvelle solution
        double randomFactor = generateGaussian(0, 1); // Facteur aléatoire gaussien
        double newSolution = operateurDeplacement(solution) + adaptiveDelta * randomFactor;
        newSolution = std::clamp(newSolution, XMINMAX[currentProblem][0], XMINMAX[currentProblem][1]);

        // Vérification si la solution a déjà été explorée
        if (visitedSolutions.find(newSolution) != visitedSolutions.end()) {
            plainTextEdit->appendPlainText(QStringLiteral("Solution déjà visitée, saut ignoré : %1")
                                               .arg(QString::number(newSolution, 'f', 10)));
            plainTextEdit->repaint(); // Mise à jour dynamique
            continue; // Ignorer cette solution
        }
        visitedSolutions.insert(newSolution); // Ajouter la solution au set

        double newCost = fonctionCout(newSolution);
        double newEs = fabs(newCost - YLIMIT[currentProblem]);
        double gain = newCost - cost;
        double Egain = newEs - Es;

        // Vérifier si la limite est atteinte
        if (fabs(newCost - YLIMIT[currentProblem]) < LIMIT_TOLERANCE) {
            limitReachedCount++;
            plainTextEdit->appendPlainText(QStringLiteral("Limite atteinte à l'itération %1 : Coût = %2")
                                               .arg(iteration)
                                               .arg(QString::number(newCost, 'f', 10)));
        }

        // Ajout à l'historique des coûts
        costHistory.push_back(newCost);

        // Détection et gestion des plateaux pour NRPB 2
        if (currentProblem == 2 && fabs(gain) < adaptiveDelta) { // Plateau détecté
            stagnantIterations++;
            totalPlateauIterations++;

            if (stagnantIterations == 1) {
                plainTextEdit->appendPlainText(QStringLiteral("Plateau detected at iteration %1. Solution: %2.")
                                                   .arg(iteration)
                                                   .arg(QString::number(solution, 'f', 10)));
            } else {
                plainTextEdit->appendPlainText(QStringLiteral("Plateau continuation at iteration %1. Count: %2.")
                                                   .arg(iteration)
                                                   .arg(stagnantIterations));
            }
            plainTextEdit->repaint(); // Mise à jour dynamique

            if (stagnantIterations >= MAXITERPLATEAU / 2) {
                adaptiveDelta = std::min(adaptiveDelta * 1.5, MAX_DELTA); // Augmenter DELTA avec une limite
                plainTextEdit->appendPlainText(QStringLiteral("Adaptive DELTA increased to %1 at iteration %2.")
                                                   .arg(QString::number(adaptiveDelta, 'f', 10))
                                                   .arg(iteration));
                plainTextEdit->repaint(); // Mise à jour dynamique
            }

            if (stagnantIterations >= MAXITERPLATEAU) {
                plateauCount++;
                plateauDurations.push_back(stagnantIterations);
                plateauSolutions.push_back(solution);

                plainTextEdit->appendPlainText(QStringLiteral("[Plateau Detected] %1 Iterations | Solution: %2")
                                                   .arg(stagnantIterations)
                                                   .arg(QString::number(solution, 'f', 10)));

                // Saut global pour diversification
                double maxCostVariation = *std::max_element(costHistory.begin(), costHistory.end()) - *std::min_element(costHistory.begin(), costHistory.end());
                double biasFactor = maxCostVariation > 0 ?
                                        (newCost - *std::min_element(costHistory.begin(), costHistory.end())) / maxCostVariation :
                                        0.5;
                newSolution = XMINMAX[currentProblem][0] + biasFactor * (XMINMAX[currentProblem][1] - XMINMAX[currentProblem][0]);

                plainTextEdit->appendPlainText("Random exploration performed to escape plateau.");
                plainTextEdit->repaint(); // Mise à jour dynamique

                stagnantIterations = 0; // Réinitialisation
                adaptiveDelta = DELTA[currentProblem]; // Réinitialisation de DELTA
            }
        } else {
            stagnantIterations = 0; // Réinitialisation en cas de sortie de plateau
        }

        // Mise à jour si la solution est meilleure
        if (Egain <= 0.0) {
            solution = newSolution;
            cost = newCost;
            Es = newEs;

            plainTextEdit->appendPlainText(QStringLiteral("Iteration: %1 | Solution: %2 | Cost: %3 | Gain: %4")
                                               .arg(iteration, 5)
                                               .arg(QString::number(solution, 'f', 10))
                                               .arg(QString::number(cost, 'f', 10))
                                               .arg(QString::number(gain, 'f', 10)));
            plainTextEdit->repaint(); // Mise à jour dynamique

            // Réinitialisation de DELTA après amélioration
            adaptiveDelta = DELTA[currentProblem];
        }

        // Arrêt anticipé si trop de plateaux détectés
        if (plateauCount >= 5 && Egain >= -SEUILARRET[currentProblem]) {
            plainTextEdit->appendPlainText("[Threshold Reached] Too many consecutive plateaus detected.");
            plainTextEdit->repaint(); // Mise à jour dynamique
            break;
        }

        iteration++;
    }

    // Sauvegarder les résultats
    if (sf) *sf = solution;
    if (Csf) *Csf = cost;
    if (itersf) *itersf = iteration;
    if (plateauItersf) *plateauItersf = totalPlateauIterations;

    // Résumé final
    plainTextEdit->appendPlainText(QStringLiteral("=== Final Results ==="));
    plainTextEdit->appendPlainText(QStringLiteral("Final Solution: %1").arg(QString::number(solution, 'f', 10)));
    plainTextEdit->appendPlainText(QStringLiteral("Final Cost: %1").arg(QString::number(cost, 'f', 10)));
    plainTextEdit->appendPlainText(QStringLiteral("Total Plateaus Detected: %1").arg(plateauCount));
    plainTextEdit->appendPlainText(QStringLiteral("Limit reached %1 times").arg(limitReachedCount));

    for (size_t i = 0; i < plateauDurations.size(); ++i) {
        plainTextEdit->appendPlainText(QStringLiteral("Plateau %1: Duration = %2 Iterations | Solution = %3")
                                           .arg(i + 1)
                                           .arg(plateauDurations[i])
                                           .arg(QString::number(plateauSolutions[i], 'f', 10)));
    }

    plainTextEdit->appendPlainText("=== Hill Climber Run Completed ===");
    plainTextEdit->repaint(); // Mise à jour finale
}

void hillclimber::runMultiple(QPlainTextEdit *plainTextEdit, int runs, int NRPB) {
    currentProblem = NRPB;

    std::vector<double> solutions(runs);
    std::vector<double> costs(runs);
    std::vector<int> iterations(runs);
    std::vector<int> plateauIterations(runs);
    int totalLimitReached = 0; // Compteur pour les atteintes de la limite

    for (int i = 0; i < runs; ++i) {
        double sf, Csf;
        int itersf, plateauItersf;

        // Exécution du HillClimber
        run(plainTextEdit, &sf, &Csf, &itersf, &plateauItersf);

        solutions[i] = sf;
        costs[i] = Csf;
        iterations[i] = itersf;
        plateauIterations[i] = plateauItersf;

        // Vérifier si la limite a été atteinte
        if (fabs(Csf - YLIMIT[NRPB]) < 0.0001) {
            totalLimitReached++;
        }
    }

    // Calcul des statistiques
    double avgSolution = calculateAverage(solutions);
    double avgCost = calculateAverage(costs);
    double avgIterations = calculateAverage(std::vector<double>(iterations.begin(), iterations.end()));
    double avgPlateauIterations = calculateAverage(std::vector<double>(plateauIterations.begin(), plateauIterations.end()));

    double stdDevSolution = calculateStdDev(solutions, avgSolution);
    double stdDevCost = calculateStdDev(costs, avgCost);

    // Afficher les résultats des exécutions multiples
    plainTextEdit->appendPlainText("=== Résultats pour " + QString::number(runs) + " exécutions ===");
    plainTextEdit->appendPlainText("Moyenne des solutions : " + QString::number(avgSolution, 'f', 3));
    plainTextEdit->appendPlainText("Écart-type des solutions : " + QString::number(stdDevSolution, 'f', 3));
    plainTextEdit->appendPlainText("Moyenne des coûts : " + QString::number(avgCost, 'f', 3));
    plainTextEdit->appendPlainText("Écart-type des coûts : " + QString::number(stdDevCost, 'f', 3));
    plainTextEdit->appendPlainText("Moyenne des itérations : " + QString::number(avgIterations, 'f', 3));
    plainTextEdit->appendPlainText("Moyenne des itérations sur plateaux : " + QString::number(avgPlateauIterations, 'f', 3));
    plainTextEdit->appendPlainText("Nombre d'atteintes de la limite : " + QString::number(totalLimitReached) + " sur " + QString::number(runs));
    plainTextEdit->appendPlainText("=== Fin des exécutions multiples ===");
}



// Définir les plages pour les hyperparamètres
std::vector<double> DELTA_VALUES = {0.01, 0.05, 0.1};
std::vector<double> SEUILARRET_VALUES = {0.0001, 0.00001, 0.000001};
std::vector<int> MAXITERATIONS_VALUES = {5, 10, 20};

void hillclimber::performGridSearch(int currentProblem, QPlainTextEdit *plainTextEdit) {
    // Plages des hyperparamètres spécifiques au problème courant
    std::vector<double> DELTA_VALUES, SEUILARRET_VALUES;
    std::vector<int> MAXITERATIONS_VALUES;

    switch (currentProblem) {
    case 0: // Problème 0
        DELTA_VALUES = {0.01, 0.05, 0.1};
        SEUILARRET_VALUES = {0.0001, 0.00001, 0.000001};
        MAXITERATIONS_VALUES = {100, 500, 1000};
        break;
    case 1: // Problème 1
        DELTA_VALUES = {0.001, 0.01, 0.05};
        SEUILARRET_VALUES = {0.00001, 0.000001, 0.0000001};
        MAXITERATIONS_VALUES = {5, 10, 20};
        break;
    case 2: // Problème 2
        DELTA_VALUES = {0.005, 0.02, 0.1};
        SEUILARRET_VALUES = {0.001, 0.0001, 0.00001};
        MAXITERATIONS_VALUES = {5, 6, 7};
        break;
    default:
        throw std::invalid_argument("Numéro de problème invalide !");
    }

    // Stocker les résultats sous forme de tuples (DELTA, SEUILARRET, MAXITERATIONS, coût moyen, écart-type, score)
    std::vector<std::tuple<double, double, int, double, double, double>> results;

    for (double delta : DELTA_VALUES) {
        for (double seuil : SEUILARRET_VALUES) {
            for (int maxIter : MAXITERATIONS_VALUES) {
                // Configurer les hyperparamètres
                setDelta(delta);
                setSeuilArret(seuil);
                setMaxIterations(maxIter);

                // Effectuer plusieurs runs pour calculer les statistiques
                const int NUM_RUNS = 10;
                std::vector<double> costs(NUM_RUNS);

                for (int i = 0; i < NUM_RUNS; ++i) {
                    double sf, Csf;
                    int itersf, plateauItersf;

                    // Exécuter le Hillclimber
                    run(plainTextEdit, &sf, &Csf, &itersf, &plateauItersf);
                    costs[i] = Csf;
                }

                // Calcul des statistiques (moyenne et écart-type)
                double avgCost = std::accumulate(costs.begin(), costs.end(), 0.0) / NUM_RUNS;
                double variance = std::accumulate(costs.begin(), costs.end(), 0.0, [avgCost](double acc, double cost) {
                                      return acc + std::pow(cost - avgCost, 2);
                                  }) / NUM_RUNS;
                double stdDevCost = std::sqrt(variance);

                // Calcul d'un score combiné : coût moyen + pondération pour l'écart-type
                double alpha = 0.7; // Pondération pour le coût moyen
                double score = alpha * avgCost + (1 - alpha) * stdDevCost;

                // Sauvegarder les résultats
                results.emplace_back(delta, seuil, maxIter, avgCost, stdDevCost, score);
            }
        }
    }

    // Trier les résultats par le score combiné croissant
    std::sort(results.begin(), results.end(), [](const auto &a, const auto &b) {
        return std::get<5>(a) < std::get<5>(b);
    });

    // Afficher les résultats
    plainTextEdit->appendPlainText(QString("=== Résultats pour le Problème %1 ===").arg(currentProblem));

    for (const auto &result : results) {
        QString line = QString("DELTA: %1, SEUILARRET: %2, MAXITERATIONS: %3, Coût Moyen: %4, Ecart-Type: %5, Score: %6")
                           .arg(std::get<0>(result))
                           .arg(std::get<1>(result))
                           .arg(std::get<2>(result))
                           .arg(QString::number(std::get<3>(result), 'f', 6))
                           .arg(QString::number(std::get<4>(result), 'f', 6))
                           .arg(QString::number(std::get<5>(result), 'f', 6));
        plainTextEdit->appendPlainText(line);
    }

    // Afficher le meilleur ensemble d'hyperparamètres (premier après tri)
    plainTextEdit->appendPlainText("=== Meilleur Ensemble d'Hyperparamètres ===");
    const auto &bestResult = results.front(); // Premier élément = meilleur
    QString bestLine = QString("DELTA: %1, SEUILARRET: %2, MAXITERATIONS: %3, Coût Moyen: %4, Ecart-Type: %5, Score: %6")
                           .arg(std::get<0>(bestResult))
                           .arg(std::get<1>(bestResult))
                           .arg(std::get<2>(bestResult))
                           .arg(QString::number(std::get<3>(bestResult), 'f', 6))
                           .arg(QString::number(std::get<4>(bestResult), 'f', 6))
                           .arg(QString::number(std::get<5>(bestResult), 'f', 6));
    plainTextEdit->appendPlainText(bestLine);
}


