#include "MainWindow.h"
#include <QBrush>
#include <QColor>
#include <QComboBox>
#include <QFileDialog>
#include <QFont>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStringList>
#include <QTableWidget>
#include <QVBoxLayout>

//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

static QString hex2(unsigned value) {
    return QString("%1").arg(value, 2, 16, QChar('0')).toUpper();
}

//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

static bool parseHexText(const QString& text, unsigned maxValue, unsigned& out) {
    bool ok = false;
    unsigned value = text.trimmed().toUInt(&ok, 16);
    if (!ok || value > maxValue) {
        return false;
    }
    out = value;
    return true;
}

//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {

    setWindowTitle("Vole Machine Simulator");
    resize(1100, 700);

    buildUi();
    refresh();
    setStatus("Click \"Load Program\" to begin.");
}

//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

void MainWindow::buildUi() {

    QFont mono("Consolas");
    mono.setStyleHint(QFont::Monospace);

    auto* central = new QWidget(this);
    auto* root = new QHBoxLayout(central);

    // ---------- Memory (left side) ----------
    auto* memBox = new QGroupBox("Main Memory (double-click a cell to edit)");
    auto* memLayout = new QVBoxLayout(memBox);

    memoryTable = new QTableWidget(16, 16);
    memoryTable->setFont(mono);

    QStringList colHeads;
    QStringList rowHeads;
    for (int i = 0; i < 16; ++i) {
        colHeads << QString::number(i, 16).toUpper();
        rowHeads << hex2(static_cast<unsigned>(i * 16));
    }
    memoryTable->setHorizontalHeaderLabels(colHeads);
    memoryTable->setVerticalHeaderLabels(rowHeads);
    memoryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    memoryTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for (int r = 0; r < 16; ++r) {
        for (int c = 0; c < 16; ++c) {
            auto* item = new QTableWidgetItem("00");
            item->setTextAlignment(Qt::AlignCenter);
            memoryTable->setItem(r, c, item);
        }
    }

    memLayout->addWidget(memoryTable);
    root->addWidget(memBox, 3);

    // ---------- Right side ----------
    auto* right = new QVBoxLayout();

    // CPU info
    auto* cpuBox = new QGroupBox("CPU");
    auto* cpuLayout = new QHBoxLayout(cpuBox);

    QFont bigMono = mono;
    bigMono.setPointSize(12);
    bigMono.setBold(true);

    pcLabel = new QLabel("PC: 00");
    irLabel = new QLabel("IR: 0000");
    haltLabel = new QLabel("Halted: No");
    pcLabel->setFont(bigMono);
    irLabel->setFont(bigMono);
    haltLabel->setFont(bigMono);

    cpuLayout->addWidget(pcLabel);
    cpuLayout->addWidget(irLabel);
    cpuLayout->addWidget(haltLabel);
    right->addWidget(cpuBox);

    // Registers
    auto* regBox = new QGroupBox("Registers (double-click to edit)");
    auto* regLayout = new QVBoxLayout(regBox);

    registerTable = new QTableWidget(16, 1);
    registerTable->setFont(mono);
    registerTable->setHorizontalHeaderLabels(QStringList() << "Value");

    QStringList regHeads;
    for (int i = 0; i < 16; ++i) {
        regHeads << ("R" + QString::number(i, 16).toUpper());
    }
    registerTable->setVerticalHeaderLabels(regHeads);
    registerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    registerTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for (int i = 0; i < 16; ++i) {
        auto* item = new QTableWidgetItem("00");
        item->setTextAlignment(Qt::AlignCenter);
        registerTable->setItem(i, 0, item);
    }

    regLayout->addWidget(registerTable);
    right->addWidget(regBox, 3);

    // Screen
    auto* screenBox = new QGroupBox("Screen");
    auto* screenLayout = new QVBoxLayout(screenBox);

    screenModeBox = new QComboBox();
    screenModeBox->addItem("Show as decimal");
    screenModeBox->addItem("Show as hexadecimal");

    screenView = new QPlainTextEdit();
    screenView->setReadOnly(true);
    screenView->setFont(mono);

    screenLayout->addWidget(screenModeBox);
    screenLayout->addWidget(screenView);
    right->addWidget(screenBox, 1);

    // Buttons
    auto* buttons = new QHBoxLayout();
    loadButton = new QPushButton("1. Load Program");
    stepButton = new QPushButton("2. Step");
    runButton = new QPushButton("3. Run");
    resetButton = new QPushButton("Reset");

    buttons->addWidget(loadButton);
    buttons->addWidget(stepButton);
    buttons->addWidget(runButton);
    buttons->addWidget(resetButton);
    right->addLayout(buttons);

    // Status message
    statusLabel = new QLabel();
    statusLabel->setWordWrap(true);
    right->addWidget(statusLabel);

    root->addLayout(right, 2);
    setCentralWidget(central);

    stepButton->setEnabled(false);
    runButton->setEnabled(false);

    // Connections
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::onLoad);
    connect(stepButton, &QPushButton::clicked, this, &MainWindow::onStep);
    connect(runButton, &QPushButton::clicked, this, &MainWindow::onRun);
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::onReset);
    connect(memoryTable, &QTableWidget::itemChanged,
            this, &MainWindow::onMemoryEdited);
    connect(registerTable, &QTableWidget::itemChanged,
            this, &MainWindow::onRegisterEdited);
    connect(screenModeBox, &QComboBox::currentIndexChanged,
            this, [this](int) { refresh(); });
}

//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

void MainWindow::refresh() {

    MachineState s = controller.snapshot();
    unsigned pc = static_cast<unsigned>(s.pc);

    updating = true;

    for (int r = 0; r < 16; ++r) {
        for (int c = 0; c < 16; ++c) {
            unsigned index = static_cast<unsigned>(r * 16 + c);
            QTableWidgetItem* item = memoryTable->item(r, c);

            item->setText(hex2(s.memory[index]));

            bool isNextInstruction =
                programLoaded && (index == pc || index == pc + 1);

            if (isNextInstruction) {
                item->setBackground(QBrush(QColor("#ffe66d")));
                item->setForeground(QBrush(QColor("#000000")));
            }
            else {
                item->setBackground(QBrush());
                item->setForeground(QBrush());
            }
        }
    }

    for (int i = 0; i < 16; ++i) {
        registerTable->item(i, 0)->setText(hex2(s.registers[i]));
    }

    pcLabel->setText("PC: " + hex2(pc));
    irLabel->setText(QString("IR: %1")
                     .arg(static_cast<unsigned>(s.ir), 4, 16, QChar('0'))
                     .toUpper());
    haltLabel->setText(s.halted ? "Halted: Yes" : "Halted: No");

    QStringList parts;
    for (uint8_t v : s.screen) {
        if (screenModeBox->currentIndex() == 1) {
            parts << (QStringLiteral("0x") + hex2(v));
        }
        else {
            parts << QString::number(v);
        }
    }
    screenView->setPlainText(parts.join(" "));

    updating = false;
}

//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

void MainWindow::setStatus(const QString& text, bool error) {
    statusLabel->setText(text);
    statusLabel->setStyleSheet(error ? "color: #e74c3c; font-weight: bold;"
                                     : "font-weight: bold;");
}

//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

void MainWindow::showStepResult(StepStatus status) {

    switch (status) {
    case StepStatus::OK:
        setStatus("Step executed.");
        break;
    case StepStatus::HALTED:
        setStatus("Program halted. Load another program or press Reset.");
        break;
    case StepStatus::INVALID_INSTRUCTION:
        setStatus("Invalid instruction at the current PC.", true);
        break;
    case StepStatus::PC_OUT_OF_RANGE:
        setStatus("Program counter went out of range.", true);
        break;
    case StepStatus::STEP_LIMIT_REACHED:
        setStatus("Step limit reached (possible infinite loop).", true);
        break;
    default:
        setStatus("Unknown status.", true);
        break;
    }

    if (status != StepStatus::OK && status != StepStatus::STEP_LIMIT_REACHED) {
        stepButton->setEnabled(false);
        runButton->setEnabled(false);
    }
}

//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

void MainWindow::onLoad() {

    QString path = QFileDialog::getOpenFileName(
        this, "Open a Vole program", QString(),
        "Text files (*.txt);;All files (*)");

    if (path.isEmpty()) {
        return;
    }

    bool ok = false;
    QString text = QInputDialog::getText(
        this, "Start address",
        "Start address (hex, even number from 00 to FE):",
        QLineEdit::Normal, "00", &ok);

    if (!ok) {
        return;
    }

    unsigned start = 0;
    if (!parseHexText(text, 0xFE, start) || start % 2 != 0) {
        setStatus("Invalid start address. Use an even hex value from 00 to FE.", true);
        return;
    }

    LoadOutcome result = controller.loadProgram(
        std::string(path.toLocal8Bit().constData()),
        static_cast<uint16_t>(start));

    if (result.status != LoadStatus::OK) {
        QString message = "Loading failed: " + QString::fromStdString(result.message);
        if (result.line != 0) {
            message += QString(" (line %1)").arg(result.line);
        }
        setStatus(message, true);
        return;
    }

    programLoaded = true;
    stepButton->setEnabled(true);
    runButton->setEnabled(true);

    refresh();
    setStatus("Program loaded at address " + hex2(start) + ". Press Step or Run.");
}

//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

void MainWindow::onStep() {
    StepStatus status = controller.step();
    refresh();
    showStepResult(status);
}

//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

void MainWindow::onRun() {
    StepStatus status = controller.run();
    refresh();
    showStepResult(status);
}

//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

void MainWindow::onReset() {
    controller.reset();
    programLoaded = false;
    stepButton->setEnabled(false);
    runButton->setEnabled(false);
    refresh();
    setStatus("Machine reset. Load a program to begin.");
}

//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

void MainWindow::onMemoryEdited(QTableWidgetItem* item) {

    if (updating) {
        return;
    }

    unsigned address = static_cast<unsigned>(item->row() * 16 + item->column());
    unsigned value = 0;

    if (!parseHexText(item->text(), 0xFF, value)) {
        setStatus("Invalid value. Enter a hex number from 00 to FF.", true);
        refresh();
        return;
    }

    controller.overwriteMemory(static_cast<uint8_t>(address),
                               static_cast<uint8_t>(value));
    refresh();
    setStatus("Memory " + hex2(address) + " set to " + hex2(value) + ".");
}

//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

void MainWindow::onRegisterEdited(QTableWidgetItem* item) {

    if (updating) {
        return;
    }

    unsigned index = static_cast<unsigned>(item->row());
    unsigned value = 0;

    if (!parseHexText(item->text(), 0xFF, value)) {
        setStatus("Invalid value. Enter a hex number from 00 to FF.", true);
        refresh();
        return;
    }

    controller.overwriteRegister(static_cast<uint8_t>(index),
                                 static_cast<uint8_t>(value));
    refresh();
    setStatus("Register R" + QString::number(index, 16).toUpper() +
              " set to " + hex2(value) + ".");
}