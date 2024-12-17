#ifndef GLOBALUTILS_H
#define GLOBALUTILS_H

// Projet   : Hillclimber lançant Scilab en langage C/C++ en gérant les entrées/sorties
// Fichier  : globalutils.h
// Auteur   : D. Duvivier (c) 2022 UPHF/INSA
// Licence  : Utilisable dans le cadre des enseignements en optimisation-simulation en Master TNSID
// Version  : 0.0.1
// Date     : 03/03/2022
//          :
// Contenu  : Méthodes "utilitaires/globales" et constantes globales
//
#include <QString>

// Pour la prochaine version de HillClimbers :
// Nombre de réplications en terme de nombre d'appels à la méthode d'optimisation
#define NBREPLICATIONS 5

// Ces constantes doivent être visibles depuis GUI (au moins pour les tests prélimnaires)
// Valeurs du paramètre 0 pour AfficheErreur
#define STOPEXIT 1   // Mode "sauvage"
#define STOPRETURN 0 // Mode "return" + gestion des codes de retour

// Pour la prochaine version de HillClimbers :
// Type pour contenir les résultats du "benchmark" pour la réplication N°i
// Tresults[i][0] = sf -> solution finale (valeur du paramètre t ou autre)
// Tresults[i][1] = Csf -> valeur du coût de la solution finale
// Tresults[i][2] = evaluationsCsf -> nombre d'évaluations jusqu'à la solution finale
// Tresults[i][3] = evaluations -> nombre d'évaluations jusqu'à la fin d'exécution du HC
typedef double Tresults[NBREPLICATIONS][4];

extern int AfficheErreur(const QString& message, const int stoppersauvagement);
extern void calculEtAffichageStatistiques(const Tresults results, const int nbreplications, const int nbcriteres);

#endif // GLOBALUTILS_H
