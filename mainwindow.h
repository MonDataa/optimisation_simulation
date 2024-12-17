#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
// #include <QRandomGenerator>
#include "globalutils.h"
#include "hillclimber.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_lanceScilab_clicked();
    void on_pushButton_runGridSearch_clicked();
    void on_pushButton_lanceHC_clicked();
    void on_pushButton_runMultiple_clicked(); // Slot for multiple Hill Climber executions
private:
    Ui::MainWindow *ui;
    scilabcomm slcmw; // Objet de communication/synchronisation avec scilab depuis MainWindow
    hillclimber hc; // Le hillclimber est un objet non publiquement visible
};
#endif // MAINWINDOW_H
