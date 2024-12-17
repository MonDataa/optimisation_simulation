// Projet   : Hillclimber lançant Scilab en langage C/C++ en gérant les entrées/sorties
// Fichier  : globalutils.h
// Auteur   : D. Duvivier (c) 2022-2025 UPHF/INSA
// Licence  : Utilisable dans le cadre des enseignements en optimisation-simulation en Master TNSID
// Version  : 0.0.2
// Date     : 23/10/2024
//          :
// Contenu  : Méthodes "utilitaires/globales" et constantes globales
//

#include <QtGlobal>
#include <QTime>
#include <QMessageBox>
#include "globalutils.h"

// Affiche une erreur via un "popup".
// Le paramètre "stoppersauvagement" indique si cette procédure stoppe
// "sauvagement" le programme en utilisant exit ou non :
// - si stoppersauvagement est différent de 0, on utilise exit()
// - si stoppersauvagement est égal à 0, on retourne -1
//   et exit n'est pas utilisé (l'appelant doit gérer l'erreur)
int AfficheErreur(const QString& message, const int stoppersauvagement)
{
  QMessageBox msgBox;

  msgBox.setText(message);
  if (stoppersauvagement != 0)
    msgBox.setIcon(QMessageBox::Critical);
  else
    msgBox.setIcon(QMessageBox::Warning);
  msgBox.setWindowTitle("***_ERREUR_***");
  msgBox.exec();

  // fprintf(stderr,"#\n# ERREUR: %s !\n#\n", message.toStdString().c_str());
  if (stoppersauvagement != 0)
    exit(1);
  return -1;
}

// Calcul et afffichage des résultats stockés dans le tableau results :
// - première dimension indicée par le numéro de réplication
// - deuxième dimension indicée par le numéro de "critère" à évaluer
//
// ATTENTION : Suppose que nbreplications et nbcriteres >= 1
//
// TODO : Calculer l'écart-type (n-1) et la variabilité (n-1)
//
void calculEtAffichageStatistiques(const Tresults results, const int nbreplications, const int nbcriteres)
{
  int i, j; // N° de réplication/critère
  double rmin, rmax, rsom; // Valeurs mini/maxi et somme pour le critère courant

  for (j=0; j<nbcriteres; j++) {
    rmin = rmax = rsom = results[0][j]; // Initialise à la première valeur
    for (i=1; i<nbreplications; i++) {
      if (results[i][j] < rmin)
    rmin = results[i][j];
      if (results[i][j] > rmax)
    rmax = results[i][j];
      rsom += results[i][j];
    }
    printf("Crit%1d\tmin %13.6lf\tmax %13.6lf\tmoy %13.6lf\n", j, rmin, rmax, rsom/(double)nbreplications);
  }
}
