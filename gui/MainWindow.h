#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QString>
#include "MachineController.h"

class QTableWidget;
class QTableWidgetItem;
class QLabel;
class QPlainTextEdit;
class QPushButton;
class QComboBox;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onLoad();
    void onStep();
    void onRun();
    void onReset();
    void onMemoryEdited(QTableWidgetItem* item);
    void onRegisterEdited(QTableWidgetItem* item);

private:
    void buildUi();
    void refresh();
    void setStatus(const QString& text, bool error = false);
    void showStepResult(StepStatus status);

    MachineController controller;
    bool updating = false;
    bool programLoaded = false;

    QTableWidget* memoryTable = nullptr;
    QTableWidget* registerTable = nullptr;
    QLabel* pcLabel = nullptr;
    QLabel* irLabel = nullptr;
    QLabel* haltLabel = nullptr;
    QLabel* statusLabel = nullptr;
    QPlainTextEdit* screenView = nullptr;
    QComboBox* screenModeBox = nullptr;
    QPushButton* loadButton = nullptr;
    QPushButton* stepButton = nullptr;
    QPushButton* runButton = nullptr;
    QPushButton* resetButton = nullptr;
};

#endif