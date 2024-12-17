// Projet   : Hillclimber lançant Scilab en langage C/C++ en gérant les entrées/sorties
// Fichier  : scilabcomm.cpp
// Auteur   : D. Duvivier (c) 2022..2025 UPHF/INSA
// Licence  : Utilisable dans le cadre des enseignements en optimisation-simulation en Master TNSID
// Version  : 0.0.2
// Date     : 23/10/2024
//          :
// Limites  : Le fichier sce doit comporter
//          : au maximum MAXLIGNES lignes de MAXCOLONNES caractères,
//          : à raison d'une seule instruction par ligne,
//          : non précédée par des espaces ou tabulations
//          : et terminée par ;
//
// Contenu  : communications avec Scilab via fichiers & tube et lancement de scilab
//          : pour réaliser l'évaluation par simulation (i.e. un type d'optimisation-simulation)
//

#include <cstdio>
#include "globalutils.h"
#include "scilabcomm.h"
#include <QFile>
#include <QTextStream>
#include <QProcess>

// Constructeur
scilabcomm::scilabcomm()
{

}

// 03032022: Ajouté -> TODO : Convertir en C++ & améliorer l'affichage si possible/utile (via GUI)
// LireLignesFichier permet de ligne un fichier contenant les paramètres de Scilab [et le modèle].
// Le nom du fichier est passé en paramètre et le nombre de lignes effectivement lues est retourné via la variable lignes.
//
// Attention : Le premier numéro de ligne est initialisé à 0
//
// Le code de retour est géré comme suit :
// - 0 pour pas d'erreur
// - <0 en cas d'erreur
// - Arrêt "brutal" du programme en cas d'erreur majeure (criticable, à améliorer !!!)
//
// Limites : Rien ne vérifie si la ligne courante contient plus de 80 caractères
//
int scilabcomm::LireLignesFichier(const QString &fileName, char lignes[][81], int *lineCount) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return -1; // Erreur d'ouverture
    }
    QTextStream in(&file);
    int count = 0;
    while (!in.atEnd() && count < *lineCount) {
        QString line = in.readLine();
        strncpy(lignes[count], line.toStdString().c_str(), 80); // Limite à 80 caractères
        lignes[count][80] = '\0'; // Assurer la terminaison
        count++;
    }
    *lineCount = count;
    return 0; // Succès
}

int scilabcomm::EcrireLignesFichier(const QString &fileName, char lignes[][81], int lineCount) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return -1;
    }
    QTextStream out(&file);
    for (int i = 0; i < lineCount; ++i) {
        out << lignes[i] << "\n";
    }
    return 0;
}

bool scilabcomm::validerVitesseTheorique(double vitesseSimulee, double masse) {
    double vitesseTheorique;
    if (masse == 75.0) {
        vitesseTheorique = 40.0;
    } else if (masse == 150.0) {
        vitesseTheorique = 50.0;
    } else {
        vitesseTheorique = 36.6583;
    }

    double difference = fabs(vitesseSimulee - vitesseTheorique);

    if (difference > 0.1) {
        printf("Erreur: Vitesse simulée (%.3f km/h) diffère de la valeur théorique (%.3f km/h)\n", vitesseSimulee, vitesseTheorique);
        return false;
    }
    return true;
}





// 03032022: Ajouté -> TODO : Convertir en C++ & améliorer l'affichage si possible/utile (via GUI)
// Affiche le fichier lu
void scilabcomm::AfficheLignesFichier(TLignesFichier lignesfichier, int lignes)
{
  int noligne; // Numéro de ligne courante

  for(noligne = 0; noligne < lignes; noligne++)
    printf("%3d\t%s", noligne, lignesfichier[noligne]);
}



// TODO : Convertir en C++ & améliorer l'affichage si possible/utile (via GUI)
// 03032022: Il faut fusionner le code initialement présent en fin de la méthode
// void MainWindow::on_pushButton_lanceScilab_clicked() et qui concerne l'appel à Scilab
// dans cette méthode hillclimber::EvaluerParSimulationViaScilab, de manière à déplacer
// tout le code concernant le hillclimber dans cette classe hillclimber
//
// Exécute scilab en récupérant la sortie standard dans un tube anonyme
// et en convertissant le résultat en double
//
// Retourne une valeur négative en cas d'erreur
//

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

double scilabcomm::EvaluerParSimulationViaScilab()
{
    FILE *fp; // Sortie standard du processus associé à Scilab
    TLigne ligne; // Ligne retournée par Scilab (il pourrait y en avoir plusieurs, en ce cas utiliser TLignesFichier ;-) )
    double vitesse = -1.0; // Vitesse lue depuis la sortie standard de Scilab

    // 03032022: L'ancien code (issu de lancerScilabEnC, fourni sur Moodle)
    // était celui qui est repris ci-dessous et préfixé par //**//
    //**// if ((fp = popen("/usr/bin/scilab -nb -nwni -f optisimparachute.sce", "r")) == NULL)
    //**//  AfficheErreur("EvaluerParSimulationViaScilab: problème lors de l'ouverture de la sortie standard du process", STOPEXIT);
    //**//
    //**// // Lit la [première] ligne retournée par Scilab
    //**// if (fgets(ligne, MAXCOLONNES, fp) == NULL)
    //**//  AfficheErreur("EvaluerParSimulationViaScilab: problème de lecture de la sortie standard du process", STOPEXIT);
    //**//
    //**// pclose(fp);
    //**// // Normalement pclose retourne un code de retour --> À gérer
    //**// // de même que les codes associés au "wait()" caché dans l'appel à pclose
    //**// // (à gérer aussi pour connaître l'état dans lequel s'est terminé le process lancé)
    //**//
    //**// // Juste pour déboguer : printf("%s", ligne);
    //**//
    //**// // Conversion de la sortie standard en double
    //**// if (sscanf(ligne, "%lf", &vitesse) < 1)
    //**//  AfficheErreur("EvaluerParSimulationViaScilab: problème de conversion en double de la sortie standard du process", STOPEXIT);

    // 03032022: Je déplace ici le code initialement placé en fin de méthode
    // MainWindow::on_pushButton_lanceScilab_clicked

    // TODO : A ADAPTER + passer plaintextedit en paramètre

    // 03032022: Ci dessous remplacez parachute.sce par optisimparachute.sce
    if ((fp = popen("\"C:/Program Files/scilab-2024.1.0/bin/scilab\" -nb -nwni -f C:/Users/ASUS/Documents/Programme/M2_TNSID/s9/Optimisation_simulation/hillclimbersinusxqt_test/optisimparachute.sce", "r")) == NULL) {
        AfficheErreur("Problème lors du lancement de Scilab !", STOPEXIT);
    }
    else {
        // Si vous utilisez disp(v) au lieu de printf("%lf", v) dans parachute.sce, il faut décommenter le premier fgets ci-dessous
        //if (fgets(ligne, 255, fp) == NULL) {
        //    AfficheErreur(Problème de lecture de la sortie standard du processus lancé !");
        //}
        if (fgets(ligne, 255, fp) == NULL) {
            AfficheErreur("Problème de lecture de la sortie standard du processus lancé !", STOPEXIT);
        }
        pclose(fp);
        // 03032022: Pour vérifier/afficher la ligne/valeur retournée, décommentez la ligne suivante :
        // plainTextEdit->appendPlainText(ligne);

        if (sscanf(ligne, "%lf", &vitesse) < 1) {
            AfficheErreur("Problème de lecture de la sortie standard du processus lancé !", STOPEXIT);
        }
        // 03032022: Pour afficher la valeur retournée convertie en double,
        // décommentez les deux lignes suivantes :
        // else
        //    AfficheErreur(QString::number(vitesse,'g',10), STOPEXIT);
    }

    return vitesse;
}


