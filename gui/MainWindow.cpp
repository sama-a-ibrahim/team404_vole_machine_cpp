#include "MainWindow.h"
#include <QLabel>


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {

    setWindowTitle("Vole Machine Simulator");
    resize(1000, 650);

    setCentralWidget(new QLabel("Vole Machine GUI works!", this));
}