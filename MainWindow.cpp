#include "mainwindow.h"
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFontDatabase>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSlider>
#include <QSplitter>
#include <QStatusBar>
#include <QTableWidget>
#include <QTemporaryFile>
#include <QTextStream>
#include <QVBoxLayout>

// ---------- small helpers ----------
static QString hex2(unsigned v)
{
    return QString::number(v, 16).toUpper().rightJustified(2, '0');
}

static QString hex1(unsigned v)
{
    return QString::number(v, 16).toUpper();
}

static bool parseHex(const QString &text, unsigned maxValue, unsigned &out)
{
    QString t = text.trimmed();
    if (t.startsWith("0x", Qt::CaseInsensitive))
        t = t.mid(2);
    if (t.isEmpty())
        return false;
    bool ok = false;
    unsigned v = t.toUInt(&ok, 16);
    if (!ok || v > maxValue)
        return false;
    out = v;
    return true;
}

// ---------- constructor ----------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Vole Machine Simulator");
    resize(1280, 780);

    buildUi();

    connect(&timer, &QTimer::timeout, this, &MainWindow::onTick);

    prev = controller.snapshot();
    refresh();
    statusBar()->showMessage("Ready. Write or open a program, then press Load.");
}

// ---------- UI ----------
void MainWindow::buildUi()
{
    const QFont mono = QFontDatabase::systemFont(QFontDatabase::FixedFont);

    auto *central = new QWidget;
    setCentralWidget(central);
    auto *root = new QVBoxLayout(central);

    // ===== top control bar =====
    auto *bar = new QHBoxLayout;

    btnRun = new QPushButton("Run");
    btnStep = new QPushButton("Step");
    btnReset = new QPushButton("Reset");
    btnRun->setStyleSheet("background:#2e9e5b;color:white;font-weight:bold;padding:7px 18px;");
    btnStep->setStyleSheet("background:#2f6fd6;color:white;font-weight:bold;padding:7px 18px;");
    btnReset->setStyleSheet("background:#c0392b;color:white;font-weight:bold;padding:7px 18px;");

    speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setRange(1, 10);
    speedSlider->setValue(4);
    speedSlider->setFixedWidth(180);
    speedLabel = new QLabel;

    stateLabel = new QLabel;
    stateLabel->setStyleSheet("font-weight:bold;padding:4px 10px;border-radius:4px;");

    bar->addWidget(btnRun);
    bar->addWidget(btnStep);
    bar->addWidget(btnReset);
    bar->addSpacing(20);
    bar->addWidget(new QLabel("Speed:"));
    bar->addWidget(speedSlider);
    bar->addWidget(speedLabel);
    bar->addStretch();
    bar->addWidget(stateLabel);
    root->addLayout(bar);

    // ===== left: program editor =====
    auto *progBox = new QGroupBox("1. Program (hex, one instruction per line)");
    auto *pl = new QVBoxLayout(progBox);

    editor = new QPlainTextEdit;
    editor->setFont(mono);
    editor->setPlainText(
        "2105 ; R1 = 05\n"
        "2203 ; R2 = 03\n"
        "5312 ; R3 = R1 + R2\n"
        "33FF ; write R3 to the screen\n"
        "C000 ; halt\n");

    btnOpen = new QPushButton("Open file...");
    btnLoad = new QPushButton("Load to memory");
    startEdit = new QLineEdit("00");
    startEdit->setMaximumWidth(50);
    startEdit->setMaxLength(4);

    auto *loadRow = new QHBoxLayout;
    loadRow->addWidget(btnOpen);
    loadRow->addStretch();
    loadRow->addWidget(new QLabel("Start address (hex):"));
    loadRow->addWidget(startEdit);
    loadRow->addWidget(btnLoad);

    pl->addWidget(editor);
    pl->addLayout(loadRow);

    // ===== center: memory =====
    auto *memBox = new QGroupBox("2. Main Memory (double-click a cell to edit)");
    auto *ml = new QVBoxLayout(memBox);

    memTable = new QTableWidget(16, 16);
    QStringList hdr;
    for (int i = 0; i < 16; ++i)
        hdr << hex1(i);
    memTable->setHorizontalHeaderLabels(hdr);
    memTable->setVerticalHeaderLabels(hdr);
    memTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    memTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    memTable->setFont(mono);
    for (int r = 0; r < 16; ++r) {
        for (int c = 0; c < 16; ++c) {
            auto *it = new QTableWidgetItem("00");
            it->setTextAlignment(Qt::AlignCenter);
            memTable->setItem(r, c, it);
        }
    }

    auto *legend = new QLabel(
        "<span style='background:#78b4ff;color:black'>&nbsp;PC / next instruction&nbsp;</span> "
        "<span style='background:#ffc86e;color:black'>&nbsp;just changed&nbsp;</span> "
        "<span style='background:#e1e1e1;color:black'>&nbsp;FF = screen&nbsp;</span>");

    ml->addWidget(memTable);
    ml->addWidget(legend);

    // ===== right: CPU + screen =====
    auto *rightWidget = new QWidget;
    auto *rl = new QVBoxLayout(rightWidget);
    rl->setContentsMargins(0, 0, 0, 0);

    auto *cpuBox = new QGroupBox("3. CPU");
    auto *cl = new QVBoxLayout(cpuBox);

    pcLabel = new QLabel;
    irLabel = new QLabel;
    pcLabel->setFont(mono);
    irLabel->setFont(mono);
    auto *pcirRow = new QHBoxLayout;
    pcirRow->addWidget(pcLabel);
    pcirRow->addWidget(irLabel);
    pcirRow->addStretch();

    nextLabel = new QLabel;
    nextLabel->setWordWrap(true);
    nextLabel->setFont(mono);
    nextLabel->setStyleSheet("background:#eef4ff;color:black;border:1px solid #9bb8e8;padding:6px;");

    pcEdit = new QLineEdit;
    pcEdit->setPlaceholderText("new PC (00-FE, even)");
    pcEdit->setMaxLength(4);
    btnSetPC = new QPushButton("Set PC");
    auto *pcSetRow = new QHBoxLayout;
    pcSetRow->addWidget(pcEdit);
    pcSetRow->addWidget(btnSetPC);

    regTable = new QTableWidget(16, 1);
    QStringList regHdr;
    for (int i = 0; i < 16; ++i)
        regHdr << ("R" + hex1(i));
    regTable->setVerticalHeaderLabels(regHdr);
    regTable->setHorizontalHeaderLabels({"Value (hex)"});
    regTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    regTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    regTable->setFont(mono);
    for (int r = 0; r < 16; ++r) {
        auto *it = new QTableWidgetItem("00");
        it->setTextAlignment(Qt::AlignCenter);
        regTable->setItem(r, 0, it);
    }

    cl->addLayout(pcirRow);
    cl->addWidget(nextLabel);
    cl->addLayout(pcSetRow);
    cl->addWidget(regTable);

    auto *screenBox = new QGroupBox("4. Screen (memory cell FF)");
    auto *sl = new QVBoxLayout(screenBox);
    screenMode = new QComboBox;
    screenMode->addItems({"Decimal", "Hexadecimal", "ASCII"});
    screenView = new QPlainTextEdit;
    screenView->setReadOnly(true);
    screenView->setFont(mono);
    screenView->setMaximumHeight(110);
    auto *smRow = new QHBoxLayout;
    smRow->addWidget(new QLabel("Show as:"));
    smRow->addWidget(screenMode);
    smRow->addStretch();
    sl->addLayout(smRow);
    sl->addWidget(screenView);

    rl->addWidget(cpuBox, 1);
    rl->addWidget(screenBox);

    // ===== assemble =====
    auto *splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(progBox);
    splitter->addWidget(memBox);
    splitter->addWidget(rightWidget);
    splitter->setSizes({280, 620, 360});
    root->addWidget(splitter, 1);

    // ===== connections =====
    connect(btnOpen, &QPushButton::clicked, this, &MainWindow::onOpenFile);
    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::onLoad);
    connect(btnRun, &QPushButton::clicked, this, &MainWindow::onRunPause);
    connect(btnStep, &QPushButton::clicked, this, &MainWindow::onStep);
    connect(btnReset, &QPushButton::clicked, this, &MainWindow::onReset);
    connect(btnSetPC, &QPushButton::clicked, this, &MainWindow::onSetPC);
    connect(pcEdit, &QLineEdit::returnPressed, this, &MainWindow::onSetPC);
    connect(memTable, &QTableWidget::cellChanged, this, &MainWindow::onMemoryEdited);
    connect(regTable, &QTableWidget::cellChanged, this, &MainWindow::onRegisterEdited);
    connect(speedSlider, &QSlider::valueChanged, this, &MainWindow::onSpeedChanged);
    connect(screenMode, &QComboBox::currentIndexChanged, this, [this]() { refresh(); });

    onSpeedChanged(speedSlider->value());
}

// ---------- state -> widgets ----------
void MainWindow::capture()
{
    prev = controller.snapshot();
}

void MainWindow::refresh()
{
    const MachineState s = controller.snapshot();
    refreshing = true;

    const QColor pcColor(120, 180, 255);
    const QColor changedColor(255, 200, 110);
    const QColor screenColor(225, 225, 225);

    // memory
    const bool pcValid = (s.pc <= 0xFE);
    for (int a = 0; a < 256; ++a) {
        QTableWidgetItem *it = memTable->item(a / 16, a % 16);
        it->setText(hex2(s.memory[a]));

        QColor bg;
        if (pcValid && (a == s.pc || a == s.pc + 1))
            bg = pcColor;
        else if (s.memory[a] != prev.memory[a])
            bg = changedColor;
        else if (a == 0xFF)
            bg = screenColor;

        if (bg.isValid()) {
            it->setBackground(bg);
            it->setForeground(Qt::black);
        } else {
            it->setBackground(QBrush());
            it->setForeground(QBrush());
        }
    }

    // registers
    for (int i = 0; i < 16; ++i) {
        QTableWidgetItem *it = regTable->item(i, 0);
        it->setText(hex2(s.registers[i]));
        if (s.registers[i] != prev.registers[i]) {
            it->setBackground(changedColor);
            it->setForeground(Qt::black);
        } else {
            it->setBackground(QBrush());
            it->setForeground(QBrush());
        }
    }

    // PC / IR
    pcLabel->setText("PC: " + hex2(s.pc));
    irLabel->setText("IR: " + hex2(s.ir >> 8) + hex2(s.ir & 0xFF));

    // next instruction
    if (s.halted) {
        nextLabel->setText("Machine is halted.");
    } else if (!pcValid) {
        nextLabel->setText("PC is out of range.");
    } else {
        const uint16_t raw = static_cast<uint16_t>((s.memory[s.pc] << 8) | s.memory[s.pc + 1]);
        Instruction ins(raw);
        const QString code = hex2(raw >> 8) + hex2(raw & 0xFF);
        if (ins.isValid())
            nextLabel->setText("Next: " + code + "  ->  " + describe(ins));
        else
            nextLabel->setText("Next: " + code + "  ->  (invalid instruction)");
    }

    // state badge
    if (s.halted) {
        stateLabel->setText("HALTED");
        stateLabel->setStyleSheet("font-weight:bold;padding:4px 10px;border-radius:4px;background:#c0392b;color:white;");
    } else if (timer.isActive()) {
        stateLabel->setText("RUNNING");
        stateLabel->setStyleSheet("font-weight:bold;padding:4px 10px;border-radius:4px;background:#2e9e5b;color:white;");
    } else {
        stateLabel->setText("READY");
        stateLabel->setStyleSheet("font-weight:bold;padding:4px 10px;border-radius:4px;background:#7f8c8d;color:white;");
    }

    // screen
    QString out;
    const int mode = screenMode->currentIndex();
    for (uint8_t v : s.screen) {
        if (mode == 0) {
            out += QString::number(v) + " ";
        } else if (mode == 1) {
            out += hex2(v) + " ";
        } else {
            out += (v >= 32 && v <= 126) ? QChar(v) : QChar('.');
        }
    }
    screenView->setPlainText(out);

    refreshing = false;
}

QString MainWindow::describe(const Instruction &i) const
{
    const QString r = "R" + hex1(i.getR());
    const QString s = "R" + hex1(i.getS());
    const QString t = "R" + hex1(i.getT());
    const QString xy = hex2(i.getXY());

    switch (i.getOpcode()) {
    case 0x1: return QString("LOAD %1 <- memory[%2]").arg(r, xy);
    case 0x2: return QString("LOAD %1 <- value %2").arg(r, xy);
    case 0x3:
        if (i.getXY() == 0xFF)
            return QString("STORE %1 -> SCREEN").arg(r);
        return QString("STORE %1 -> memory[%2]").arg(r, xy);
    case 0x4: return QString("MOVE %1 -> %2").arg(s, t);
    case 0x5: return QString("%1 = %2 + %3 (integer)").arg(r, s, t);
    case 0x6: return QString("%1 = %2 + %3 (float)").arg(r, s, t);
    case 0x7: return QString("%1 = %2 OR %3").arg(r, s, t);
    case 0x8: return QString("%1 = %2 AND %3").arg(r, s, t);
    case 0x9: return QString("%1 = %2 XOR %3").arg(r, s, t);
    case 0xA: return QString("ROTATE %1 right by %2 bit(s)").arg(r).arg(i.getT());
    case 0xB: return QString("if %1 == R0 then jump to %2").arg(r, xy);
    case 0xC: return "HALT";
    default:  return "(unknown)";
    }
}

// ---------- actions ----------
void MainWindow::onOpenFile()
{
    const QString path = QFileDialog::getOpenFileName(this, "Open program file", QString(),
                                                      "Text files (*.txt);;All files (*)");
    if (path.isEmpty())
        return;

    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        statusBar()->showMessage("Cannot open file.", 5000);
        return;
    }
    editor->setPlainText(QTextStream(&f).readAll());
    statusBar()->showMessage("File opened. Press Load to put it in memory.", 5000);
}

void MainWindow::onLoad()
{
    stopRunning();

    unsigned start = 0;
    if (!parseHex(startEdit->text(), 0xFF, start)) {
        statusBar()->showMessage("Invalid start address (use hex, e.g. 00 or 10).", 6000);
        return;
    }

    // The controller loads from a file path, so we write the editor text to a temp file.
    QTemporaryFile tmp;
    if (!tmp.open()) {
        statusBar()->showMessage("Cannot create a temporary file.", 6000);
        return;
    }
    tmp.write(editor->toPlainText().toUtf8());
    tmp.close();

    capture();
    const LoadOutcome result = controller.loadProgram(
        tmp.fileName().toLocal8Bit().toStdString(), static_cast<uint16_t>(start));

    if (result.status != LoadStatus::OK) {
        QString msg = "Load failed: " + QString::fromStdString(result.message);
        statusBar()->showMessage(msg, 10000);
        refresh();
        return;
    }

    refresh();
    statusBar()->showMessage(
        QString("Loaded %1 instruction(s) at address %2. PC set to %2.")
            .arg(result.instructions.size())
            .arg(hex2(start)), 6000);
}

void MainWindow::onStep()
{
    stopRunning();
    capture();
    handleStatus(controller.step());
    refresh();
}

void MainWindow::onRunPause()
{
    if (timer.isActive()) {
        stopRunning();
        refresh();
        return;
    }
    acc = 0.0;
    timer.start(20);
    btnRun->setText("Pause");
    btnStep->setEnabled(false);
    refresh();
}

void MainWindow::onTick()
{
    capture();
    acc += stepsPerSecond() * 0.02;

    int n = 0;
    while (acc >= 1.0 && n < 5000) {
        acc -= 1.0;
        ++n;
        const StepStatus st = controller.step();
        if (st != StepStatus::OK) {
            handleStatus(st);
            break;
        }
    }
    if (acc > 5000.0)
        acc = 0.0;

    refresh();
}

void MainWindow::onReset()
{
    stopRunning();
    controller.reset();
    prev = controller.snapshot();
    refresh();
    statusBar()->showMessage("Machine reset (memory, registers, screen, PC = 00).", 5000);
}

void MainWindow::onSetPC()
{
    unsigned v = 0;
    if (!parseHex(pcEdit->text(), 0xFE, v) || !controller.setPC(static_cast<uint16_t>(v))) {
        statusBar()->showMessage("Invalid PC: use an even hex value from 00 to FE.", 6000);
        return;
    }
    capture();
    pcEdit->clear();
    refresh();
    statusBar()->showMessage("PC updated.", 3000);
}

void MainWindow::onMemoryEdited(int row, int col)
{
    if (refreshing)
        return;

    unsigned v = 0;
    if (!parseHex(memTable->item(row, col)->text(), 0xFF, v)) {
        statusBar()->showMessage("Invalid value: use hex from 00 to FF.", 5000);
        refresh();
        return;
    }
    capture();
    controller.overwriteMemory(static_cast<uint8_t>(row * 16 + col), static_cast<uint8_t>(v));
    refresh();
}

void MainWindow::onRegisterEdited(int row, int)
{
    if (refreshing)
        return;

    unsigned v = 0;
    if (!parseHex(regTable->item(row, 0)->text(), 0xFF, v)) {
        statusBar()->showMessage("Invalid value: use hex from 00 to FF.", 5000);
        refresh();
        return;
    }
    capture();
    controller.overwriteRegister(static_cast<uint8_t>(row), static_cast<uint8_t>(v));
    refresh();
}

void MainWindow::onSpeedChanged(int level)
{
    speedLabel->setText(QString("%1 steps/sec").arg(1 << (level - 1)));
}

int MainWindow::stepsPerSecond() const
{
    return 1 << (speedSlider->value() - 1);   // 1, 2, 4, ... 512
}

void MainWindow::stopRunning()
{
    timer.stop();
    btnRun->setText("Run");
    btnStep->setEnabled(true);
}

void MainWindow::handleStatus(StepStatus status)
{
    switch (status) {
    case StepStatus::OK:
        break;
    case StepStatus::HALTED:
        stopRunning();
        statusBar()->showMessage("Program halted.", 6000);
        break;
    case StepStatus::INVALID_INSTRUCTION:
        stopRunning();
        statusBar()->showMessage("Invalid instruction at PC. Execution stopped.", 8000);
        break;
    case StepStatus::PC_OUT_OF_RANGE:
        stopRunning();
        statusBar()->showMessage("Program counter out of range. Execution stopped.", 8000);
        break;
    case StepStatus::STEP_LIMIT_REACHED:
        stopRunning();
        break;
    }
}