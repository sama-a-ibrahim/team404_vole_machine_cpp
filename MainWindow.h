#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QTimer>
#include "MachineController.h"
class QTableWidget;
class QPlainTextEdit;
class QLabel;
class QLineEdit;
class QSlider;
class QPushButton;
class QComboBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onOpenFile();
    void onLoad();
    void onStep();
    void onRunPause();
    void onTick();
    void onReset();
    void onSetPC();
    void onMemoryEdited(int row, int col);
    void onRegisterEdited(int row, int col);
    void onSpeedChanged(int level);

private:
    void buildUi();
    void applyTheme();
    void refresh();
    void capture();
    void stopRunning();
    void handleStatus(StepStatus status);
    int  stepsPerSecond() const;
    QString describe(const Instruction& ins) const;

    MachineController controller;
    QTimer timer;
    MachineState prev; 
    bool refreshing = false;
    double acc = 0.0;

    QPushButton *btnOpen = nullptr;
    QPushButton *btnLoad = nullptr;
    QPushButton *btnRun = nullptr;
    QPushButton *btnStep = nullptr;
    QPushButton *btnReset = nullptr;
    QPushButton *btnSetPC = nullptr;
    QLineEdit *startEdit = nullptr;
    QLineEdit *pcEdit = nullptr;
    QPlainTextEdit *editor = nullptr;
    QPlainTextEdit *screenView = nullptr;
    QTableWidget *memTable = nullptr;
    QTableWidget *regTable = nullptr;
    QLabel *pcLabel = nullptr;
    QLabel *irLabel = nullptr;
    QLabel *stateLabel = nullptr;
    QLabel *nextLabel = nullptr;
    QLabel *speedLabel = nullptr;
    QSlider *speedSlider = nullptr;
    QComboBox *screenMode = nullptr;
};

#endif