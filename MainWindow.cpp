#include "mainwindow.h"
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFontDatabase>
#include <QFrame>
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Vole Machine Simulator");
    resize(1280, 780);

    buildUi();
    applyTheme();

    connect(&timer, &QTimer::timeout, this, &MainWindow::onTick);

    prev = controller.snapshot();
    refresh();
    statusBar()->showMessage("Ready. Write or open a program, then press Load.");
}

// UI
void MainWindow::buildUi()
{
    const QFont mono = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    auto makeTitle = [](const QString &text) {
        auto *l = new QLabel(text);
        l->setObjectName("panelTitle");
        return l;
    };

    auto *central = new QWidget;
    setCentralWidget(central);
    auto *root = new QVBoxLayout(central);
    root->setContentsMargins(24, 20, 24, 12);
    root->setSpacing(14);

    // top control bar
    auto *bar = new QHBoxLayout;
    bar->setSpacing(12);

    auto *title = new QLabel("Vole Machine");
    title->setObjectName("appTitle");

    btnRun = new QPushButton("Run");
    btnStep = new QPushButton("Step");
    btnReset = new QPushButton("Reset");
    btnRun->setObjectName("btnRun");
    btnStep->setObjectName("btnStep");
    btnReset->setObjectName("btnReset");
    for (QPushButton *b : {btnRun, btnStep, btnReset})
        b->setCursor(Qt::PointingHandCursor);

    speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setRange(1, 10);
    speedSlider->setValue(4);
    speedSlider->setFixedWidth(180);
    speedLabel = new QLabel;

    stateLabel = new QLabel;
    stateLabel->setObjectName("stateBadge");

    bar->addWidget(title);
    bar->addSpacing(16);
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

    auto *headerLine = new QFrame;
    headerLine->setObjectName("headerLine");
    headerLine->setFixedHeight(1);
    root->addWidget(headerLine);

    // left: program editor
    auto *progBox = new QGroupBox;
    progBox->setToolTip("Hex, one instruction per line");
    auto *pl = new QVBoxLayout(progBox);
    pl->addWidget(makeTitle("Program"));

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

    auto *startRow = new QHBoxLayout;
    startRow->addWidget(new QLabel("Start (hex):"));
    startRow->addWidget(startEdit);
    startRow->addStretch();

    auto *loadRow = new QHBoxLayout;
    loadRow->addWidget(btnOpen);
    loadRow->addWidget(btnLoad);

    pl->addWidget(editor);
    pl->addLayout(startRow);
    pl->addLayout(loadRow);

    // center: memory
    auto *memBox = new QGroupBox;
    memBox->setToolTip("Double-click a cell to edit");
    auto *ml = new QVBoxLayout(memBox);
    ml->addWidget(makeTitle("Memory"));

    memTable = new QTableWidget(16, 16);
    QStringList hdr;
    for (int i = 0; i < 16; ++i)
        hdr << hex1(i);
    memTable->setHorizontalHeaderLabels(hdr);
    memTable->setVerticalHeaderLabels(hdr);
    memTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    memTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    memTable->setFont(mono);
    memTable->setShowGrid(false);
    for (int r = 0; r < 16; ++r) {
        for (int c = 0; c < 16; ++c) {
            auto *it = new QTableWidgetItem("00");
            it->setTextAlignment(Qt::AlignCenter);
            memTable->setItem(r, c, it);
        }
    }

    auto *lgPC = new QLabel("PC");
    auto *lgChanged = new QLabel("just changed");
    auto *lgScreen = new QLabel("FF screen");
    lgPC->setObjectName("legendPC");
    lgChanged->setObjectName("legendChanged");
    lgScreen->setObjectName("legendScreen");
    auto *legend = new QHBoxLayout;
    legend->setSpacing(8);
    legend->addWidget(lgPC);
    legend->addWidget(lgChanged);
    legend->addWidget(lgScreen);
    legend->addStretch();

    ml->addWidget(memTable);
    ml->addLayout(legend);

    // right: CPU + screen
    auto *rightWidget = new QWidget;
    auto *rl = new QVBoxLayout(rightWidget);
    rl->setContentsMargins(0, 0, 0, 0);
    auto *cpuBox = new QGroupBox;
    auto *cl = new QVBoxLayout(cpuBox);
    cl->addWidget(makeTitle("CPU"));

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
    nextLabel->setObjectName("nextLabel");

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
    regTable->setShowGrid(false);
    for (int r = 0; r < 16; ++r) {
        auto *it = new QTableWidgetItem("00");
        it->setTextAlignment(Qt::AlignCenter);
        regTable->setItem(r, 0, it);
    }

    cl->addLayout(pcirRow);
    cl->addWidget(nextLabel);
    cl->addLayout(pcSetRow);
    cl->addWidget(regTable);

    //screen
    auto *screenBox = new QGroupBox;
    screenBox->setToolTip("Memory cell FF");
    auto *sl = new QVBoxLayout(screenBox);
    sl->addWidget(makeTitle("Screen"));

    screenMode = new QComboBox;
    screenMode->addItems({"Decimal", "Hexadecimal", "ASCII"});
    screenView = new QPlainTextEdit;
    screenView->setReadOnly(true);
    QFont screenFont("Georgia", 36);
    screenView->setObjectName("screenView");
    screenView->setFont(screenFont);
    screenView->setMinimumHeight(220);
    auto *smRow = new QHBoxLayout;
    smRow->addWidget(new QLabel("Show as:"));
    smRow->addWidget(screenMode);
    smRow->addStretch();
    sl->addLayout(smRow);
    sl->addWidget(screenView);

    rl->addWidget(cpuBox, 3);
    rl->addWidget(screenBox, 2);

    // assemble
    auto *splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(progBox);
    splitter->addWidget(memBox);
    splitter->addWidget(rightWidget);
    splitter->setSizes({260, 760, 380});
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 4);
    splitter->setStretchFactor(2, 2);
    root->addWidget(splitter, 1);

    // connections
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

// theme
void MainWindow::applyTheme()
{
    setStyleSheet(R"(
QMainWindow {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #5e433c, stop:0.55 #5d403a, stop:1 #cab3ab);
}
QWidget { color: #c7c4c3; }
QToolTip { background: #513b39; color: #b5b3b2; border: 1px solid #cec5e1; padding: 4px; }

QLabel#appTitle { font-family: Palatino Linotype, 'Times New Roman', serif; font-size: 30pt; color: #c4c3c6; }
QFrame#headerLine { background: rgba(255,255,255,25%); border: none; }

QGroupBox {
    background: rgba(255,255,255,8%);
    border: 1px solid rgba(255,255,255,28%);
    border-radius: 16px;
    padding: 14px 14px 12px 14px;
}
QLabel#panelTitle {
    font-family: Georgia, 'Times New Roman', serif;
    font-size: 20pt;
    color: #ab9aca;
    padding-bottom: 4px;
}

QPushButton {
    background: transparent;
    border: 1px solid rgba(255,255,255,70%);
    border-radius: 10px;
    padding: 8px 20px;
    color: #f6efec;
    font-weight: 600;
}
QPushButton:hover    { background: rgba(255,255,255,14%); }
QPushButton:pressed  { background: rgba(255,255,255,24%); }
QPushButton:disabled { color: rgba(255,255,255,40%); border-color: rgba(255,255,255,20%); }
QPushButton#btnRun   { background: #b598ef; border: 1px solid #c6b0f1; color: #2a1f33; }
QPushButton#btnRun:hover  { background: #d1bef7; }
QPushButton#btnStep  { background: #f4e6e6; border: 1px solid #ececec; color: #2a1f33; }
QPushButton#btnStep:hover { background: #cabdbd; }
QPushButton#btnStep:disabled { background: rgba(236,236,236,30%); border-color: transparent; color: rgba(42,31,51,50%); }

QLineEdit, QPlainTextEdit, QComboBox {
    background: rgba(0,0,0,18%);
    border: 1px solid rgba(255,255,255,25%);
    border-radius: 8px;
    padding: 6px 8px;
    selection-background-color: #cebaf6;
    selection-color: #2a1f33;
}
QLineEdit:focus, QPlainTextEdit:focus, QComboBox:focus { border: 1px solid #b7a2e1; }
QPlainTextEdit#screenView { background: transparent; border: none; color: #ffffff; }
QComboBox::drop-down { border: none; width: 22px; }
QComboBox QAbstractItemView {
    background: #4a3836; border: 1px solid rgba(255,255,255,25%);
    selection-background-color: #b89cf0; selection-color: #2a1f33;
}

QTableWidget {
    background: transparent;
    border: none;
    gridline-color: transparent;
    selection-background-color: rgba(184,156,240,35%);
    selection-color: #ffffff;
}
QHeaderView { background: transparent; }
QHeaderView::section {
    background: transparent; color: #d8cbc7;
    border: none; padding: 4px; font-weight: 600;
}
QTableCornerButton::section { background: transparent; border: none; }

QLabel#nextLabel {
    background: #c9b0f5; color: #2a1f33;
    border-radius: 10px; padding: 8px 12px; font-weight: 600;
}
QLabel#legendPC      { background: #b89cf0; color: #312a36; border-radius: 6px; padding: 4px 10px; }
QLabel#legendChanged { background: #ececec; color: #3a353e; border-radius: 6px; padding: 4px 10px; }
QLabel#legendScreen  { border: 1px solid rgba(255,255,255,70%); border-radius: 6px; padding: 4px 10px; }

QSlider::groove:horizontal { height: 4px; background: rgba(255,255,255,30%); border-radius: 2px; }
QSlider::sub-page:horizontal { background: #d5c6f4; border-radius: 2px; }
QSlider::handle:horizontal { background: #d6c5f9; width: 16px; height: 16px; margin: -6px 0; border-radius: 8px; }

QScrollBar:vertical   { background: transparent; width: 10px; margin: 0; }
QScrollBar:horizontal { background: transparent; height: 10px; margin: 0; }
QScrollBar::handle:vertical   { background: rgba(255,255,255,25%); border-radius: 5px; min-height: 24px; }
QScrollBar::handle:horizontal { background: rgba(255,255,255,25%); border-radius: 5px; min-width: 24px; }
QScrollBar::add-line, QScrollBar::sub-line { width: 0; height: 0; }
QScrollBar::add-page, QScrollBar::sub-page { background: transparent; }

QSplitter::handle { background: transparent; }
QStatusBar { background: transparent; color: #d8cbc7; }
QStatusBar::item { border: none; }
)");
}

// state -> widgets 
void MainWindow::capture()
{
    prev = controller.snapshot();
}

void MainWindow::refresh()
{
    const MachineState s = controller.snapshot();
    refreshing = true;

    const QColor pcColor("#b89cf0");
    const QColor changedColor("#ececec");
    const QColor screenColor(255, 255, 255, 45);
    const QColor darkText("#2a1f33");
    const QColor brightText("#ffffff");
    const QColor mutedText("#9a8884");

    // memory
    const bool pcValid = (s.pc <= 0xFE);
    for (int a = 0; a < 256; ++a) {
        QTableWidgetItem *it = memTable->item(a / 16, a % 16);
        it->setText(hex2(s.memory[a]));

        QColor bg, fg;
        if (pcValid && (a == s.pc || a == s.pc + 1)) {
            bg = pcColor;      fg = darkText;
        } else if (s.memory[a] != prev.memory[a]) {
            bg = changedColor; fg = darkText;
        } else if (a == 0xFF) {
            bg = screenColor;  fg = brightText;
        }

        if (bg.isValid()) {
            it->setBackground(bg);
            it->setForeground(fg);
        } else {
            it->setBackground(QBrush());
            it->setForeground(s.memory[a] ? brightText : mutedText);
        }
    }

    // registers
    for (int i = 0; i < 16; ++i) {
        QTableWidgetItem *it = regTable->item(i, 0);
        it->setText(hex2(s.registers[i]));
        if (s.registers[i] != prev.registers[i]) {
            it->setBackground(changedColor);
            it->setForeground(darkText);
        } else {
            it->setBackground(QBrush());
            it->setForeground(brightText);
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
    const QString badgeBase = "font-weight:600;padding:6px 16px;border-radius:10px;";
    if (s.halted) {
        stateLabel->setText("HALTED");
        stateLabel->setStyleSheet(badgeBase + "color:white;background:transparent;border:1px solid rgba(255,255,255,80%);");
    } else if (timer.isActive()) {
        stateLabel->setText("RUNNING");
        stateLabel->setStyleSheet(badgeBase + "color:#2a1f33;background:#b89cf0;border:1px solid #b89cf0;");
    } else {
        stateLabel->setText("READY");
        stateLabel->setStyleSheet(badgeBase + "color:#d8cbc7;background:transparent;border:1px solid rgba(255,255,255,35%);");
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

// actions 
void MainWindow::onOpenFile()
{
    const QString path = QFileDialog::getOpenFileName(this, "Open program file", QString(), "Text files (*.txt);;All files (*)");
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
    return 1 << (speedSlider->value() - 1); 
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