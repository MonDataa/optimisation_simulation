#include <QDir>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hillclimber.h"
#include "globalutils.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->comboBox_NRPB->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_lanceScilab_clicked()
{
    TLignesFichier lignesfichier;
    int lignes;
    double t = 4.0;
    double vitesse = -1.0;

    // Désactiver les boutons pendant l'exécution
    ui->pushButton_lanceHC->setEnabled(false);
    ui->pushButton_lanceScilab->setEnabled(false);

    QDir mydir(qApp->applicationDirPath() + "/../../..");

    QDir::setCurrent(mydir.canonicalPath());

    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText("# Lecture du fichier contenant le modèle Scilab...");
    ui->plainTextEdit->repaint();

    if (slcmw.LireLignesFichier("parachute.sce", lignesfichier, &lignes) < 0)
        AfficheErreur("LireLignesFichier: erreur lors de la lecture du fichier", STOPEXIT);

    t = 4.0;
    sprintf(lignesfichier[6], "t = %.8lf ;\n", t);

    ui->plainTextEdit->appendPlainText("# Ecriture du fichier contenant le modèle Scilab modifié...");
    ui->plainTextEdit->repaint();

    if (slcmw.EcrireLignesFichier("optisimparachute.sce", lignesfichier, lignes) < 0)
        AfficheErreur("EcrireLignesFichier: erreur lors de l'écriture dans le fichier", STOPEXIT);

    ui->plainTextEdit->appendPlainText("# Début de lancement de Scilab...");
    ui->plainTextEdit->appendPlainText(QStringLiteral("# Dans le répertoire : %1").arg(mydir.canonicalPath()));
    ui->plainTextEdit->repaint();

    vitesse = slcmw.EvaluerParSimulationViaScilab();
    ui->plainTextEdit->appendPlainText(QString::number(vitesse, 'g', 10));

    ui->pushButton_lanceHC->setEnabled(true);
    ui->pushButton_lanceScilab->setEnabled(true);
}

void MainWindow::on_pushButton_runGridSearch_clicked() {
    // Désactiver les boutons pendant l'exécution
    ui->pushButton_lanceHC->setEnabled(false);
    ui->pushButton_lanceScilab->setEnabled(false);

    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText("=== Lancement de la Grid Search ===");

    // Récupérer le problème sélectionné dans le comboBox
    int NRPB = ui->comboBox_NRPB->currentIndex();
    hc.currentProblem = NRPB;

    // Vérification de la validité du problème sélectionné
    if (NRPB < 0 || NRPB >= NRPROBLEMES) {
        ui->plainTextEdit->appendPlainText("# Erreur : problème sélectionné invalide.");
        // Réactiver les boutons après erreur
        ui->pushButton_lanceHC->setEnabled(true);
        ui->pushButton_lanceScilab->setEnabled(true);
        return;
    }

    ui->plainTextEdit->appendPlainText("Problème sélectionné pour Grid Search : " + QString::number(NRPB));

    try {
        // Exécuter la Grid Search pour le problème sélectionné
        hc.performGridSearch(NRPB, ui->plainTextEdit);
    } catch (const std::exception &e) {
        ui->plainTextEdit->appendPlainText(QStringLiteral("Erreur pendant la Grid Search : %1").arg(e.what()));
    } catch (...) {
        ui->plainTextEdit->appendPlainText("Erreur inconnue lors de la Grid Search.");
    }

    ui->pushButton_lanceHC->setEnabled(true);
    ui->pushButton_lanceScilab->setEnabled(true);

    ui->plainTextEdit->appendPlainText("=== Fin de la Grid Search ===");
}



void MainWindow::on_pushButton_lanceHC_clicked()
{
    // Désactiver les boutons pendant l'exécution
    ui->pushButton_lanceHC->setEnabled(false);
    ui->pushButton_lanceScilab->setEnabled(false);

    int NRPB = ui->comboBox_NRPB->currentIndex();
    hc.currentProblem = NRPB; // Synchroniser le problème actuel avec la sélection du comboBox

    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText("Lancement du HillClimber pour le problème NRPB : " + QString::number(NRPB));
    ui->plainTextEdit->repaint();

    // Vérification de la validité de NRPB
    if (NRPB < 0 || NRPB >= NRPROBLEMES) {
        ui->plainTextEdit->appendPlainText("# Erreur : problème sélectionné invalide.");
        ui->pushButton_lanceHC->setEnabled(true);
        return;
    }

    // Informations spécifiques au problème
    switch (NRPB) {
    case 0:
        ui->plainTextEdit->appendPlainText("# Problème NRPB0 : Optimisation-Simulation via Scilab.");
        break;
    case 1:
        ui->plainTextEdit->appendPlainText("# Problème NRPB1 : Fonction Sinus.");
        break;
    case 2:
        ui->plainTextEdit->appendPlainText("# Problème NRPB2 : Fonction avec Plateaux.");
        break;
    }

    // Exécution du HillClimber
    double sf, Csf;
    int itersf, plateauItersf;
    hc.run(ui->plainTextEdit, &sf, &Csf, &itersf, &plateauItersf);

    // Résumé des résultats
    ui->plainTextEdit->appendPlainText("# Résultats de l'exécution :");
    ui->plainTextEdit->appendPlainText("  - Solution finale (sf) : " + QString::number(sf));
    ui->plainTextEdit->appendPlainText("  - Coût final (Csf) : " + QString::number(Csf));
    ui->plainTextEdit->appendPlainText("  - Nombre d'itérations : " + QString::number(itersf));
    ui->plainTextEdit->appendPlainText("  - Itérations sur plateaux : " + QString::number(plateauItersf));

    ui->plainTextEdit->appendPlainText("# Fin de l'exécution du HillClimber.");

    // Réactiver les boutons
    ui->pushButton_lanceHC->setEnabled(true);
    ui->pushButton_lanceScilab->setEnabled(true);
}


void MainWindow::on_pushButton_runMultiple_clicked()
{
    // Désactiver les boutons pendant l'exécution
    ui->pushButton_lanceHC->setEnabled(false);
    ui->pushButton_lanceScilab->setEnabled(false);

    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText("# Exécution multiple du HillClimber...");

    int runs = 30; // Nombre fixe de runs
    int NRPB = ui->comboBox_NRPB->currentIndex();
    hc.currentProblem = NRPB; // Synchroniser le problème actuel

    // Vérification de la validité de NRPB
    if (NRPB < 0 || NRPB >= NRPROBLEMES) {
        ui->plainTextEdit->appendPlainText("# Erreur : problème sélectionné invalide.");
        // Réactiver les boutons après erreur
        ui->pushButton_lanceHC->setEnabled(true);
        ui->pushButton_lanceScilab->setEnabled(true);
        return;
    }

    // Exécution multiple du HillClimber
    ui->plainTextEdit->appendPlainText("# Lancement de " + QString::number(runs) + " exécutions multiples pour NRPB " + QString::number(NRPB) + "...");
    hc.runMultiple(ui->plainTextEdit, runs, NRPB);

    // Résumé global des résultats
    ui->plainTextEdit->appendPlainText("# Fin des exécutions multiples.");

    // Réactiver les boutons après l'exécution
    ui->pushButton_lanceHC->setEnabled(true);
    ui->pushButton_lanceScilab->setEnabled(true);
}

