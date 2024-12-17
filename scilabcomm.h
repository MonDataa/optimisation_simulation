#ifndef SCILABCOMM_H
#define SCILABCOMM_H

// Projet   : Hillclimber lançant Scilab en langage C/C++ en gérant les entrées/sorties
// Fichier  : scilabcomm.h
// Auteur   : D. Duvivier (c) 2022 UPHF/INSA
// Licence  : Utilisable dans le cadre des enseignements en optimisation-simulation en Master TNSID
// Version  : 0.0.1
// Date     : 03/03/2022
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

#include <QPlainTextEdit>

// Nombre maximum de lignes dans le fichier contenant les paramètres de Scilab [et le modèle]
// (fichier .sce) :
#define MAXLIGNES   25

// Nombre maximum de caractères par ligne pour le fichier .sce
#define MAXCOLONNES 80

// 03032022: Ajouté -> TODO : Convertir en C++
// Type "TLignesFichier" : grille 2D de MAXLIGNES * MAXCOLONNES caractères (+ \0).
typedef char TLigne[MAXCOLONNES+1];
typedef TLigne TLignesFichier[MAXLIGNES];

class scilabcomm
{
public:
    scilabcomm();
    // Les méthodes suivante sont dans la section "public"
    // pour tester (temporairement) ou utiliser (définitivement) via PushButton dans GUI
    //double EvaluerParSimulationViaScilab(void);
    //int LireLignesFichier(const char Nom[], TLignesFichier lignesfichier, int *lignes);
    void AfficheLignesFichier(TLignesFichier lignesfichier, int lignes);
    //int EcrireLignesFichier(const char Nom[], TLignesFichier lignesfichier, int lignes);
    int LireLignesFichier(const QString &fileName, char lignes[][81], int *lineCount);
    int EcrireLignesFichier(const QString &fileName, char lignes[][81], int lineCount);
    double EvaluerParSimulationViaScilab(); // Runs a Scilab simulation and retrieves results
    bool validerVitesseTheorique(double vitesseSimulee, double masse);


};

#endif // SCILABCOMM_H
