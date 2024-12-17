#ifndef HILLCLIMBER_H
#define HILLCLIMBER_H

// Projet   : Hillclimber lançant Scilab en langage C/C++ en gérant les entrées/sorties
// Fichier  : hillclimber.h
// Auteur   : D. Duvivier (c) 2022..2025 UPHF/INSA
// Licence  : Utilisable dans le cadre des enseignements en optimisation-simulation en Master TNSID
// Version  : 0.0.2
// Date     : 23/10/2024
//          :
// Limites  : Le hillclimber est trop "simpliste" comme méthode d'optimisation
//
// Contenu  : hillclimber de base pour tester un exemple d'optimisation-simulation
//
#include <QRandomGenerator>
#include <QPlainTextEdit>
#include <QString>
#include <QDateTime>
//#include <QStringLiteral>
#include "scilabcomm.h"

#define NRPROBLEMES 3

class hillclimber {
public:
    int currentProblem; // Replace the macro NRPB
    hillclimber() : currentProblem(1), lignes(-1) {
        myprng.seed(QDateTime::currentMSecsSinceEpoch() / 1000);
    }

    double fonctionCout(double s); // Single declaration
    void run(QPlainTextEdit *plainTextEdit, double *sf, double *Csf, int *itersf, int *plateauItersf);
    void runMultiple(QPlainTextEdit *plainTextEdit, int runs, int NRPB);
    void setSceFile(const char Nom[]);
    void performGridSearch(int currentProblem, QPlainTextEdit *plainTextEdit);

    // Méthodes pour modifier les hyperparamètres
    void setDelta(double newDelta) { delta = newDelta; }
    void setSeuilArret(double newSeuilArret) { seuilArret = newSeuilArret; }
    void setMaxIterations(int newMaxIterations) { maxIterations = newMaxIterations; }

private:
    QRandomGenerator myprng; // Générateur de nombres pseudo-aléatoires
    scilabcomm slchc; // Objet de communication/synchronisation avec scilab pour le hillclimber
    TLignesFichier lignesfichier; // Copie du fichier en mémoire
    int lignes = -1; // Nombre de lignes lues depuis le fichier contenant les paramètres de Scilab [et le modèle].

    // Hyperparamètres dynamiques
    double delta;         // Variation élémentaire
    double seuilArret;    // Seuil pour arrêter les itérations
    int maxIterations;    // Nombre maximal d'itérations

    double fonctionCoutOptiSim(double s);
    double solutionInitiale(void);
    double operateurDeplacement(double s);
};

#endif // HILLCLIMBER_H
