#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <cmath>
#include "parser.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Fix for lblAngleMode unwanted border
    ui->lblAngleMode->setAutoFillBackground(false);
    ui->lblAngleMode->setFrameStyle(QFrame::NoFrame);
    ui->lblAngleMode->raise();  // Make sure it's drawn on
    // Digit buttons
    connect(ui->btn0, &QPushButton::clicked, this, &MainWindow::digitClicked);
    connect(ui->btn1, &QPushButton::clicked, this, &MainWindow::digitClicked);
    connect(ui->btn2, &QPushButton::clicked, this, &MainWindow::digitClicked);
    connect(ui->btn3, &QPushButton::clicked, this, &MainWindow::digitClicked);
    connect(ui->btn4, &QPushButton::clicked, this, &MainWindow::digitClicked);
    connect(ui->btn5, &QPushButton::clicked, this, &MainWindow::digitClicked);
    connect(ui->btn6, &QPushButton::clicked, this, &MainWindow::digitClicked);
    connect(ui->btn7, &QPushButton::clicked, this, &MainWindow::digitClicked);
    connect(ui->btn8, &QPushButton::clicked, this, &MainWindow::digitClicked);
    connect(ui->btn9, &QPushButton::clicked, this, &MainWindow::digitClicked);
    connect(ui->btnDot, &QPushButton::clicked, this, &MainWindow::digitClicked);

    // Operators
    connect(ui->btnAdd, &QPushButton::clicked, this, &MainWindow::operatorClicked);
    connect(ui->btnSub, &QPushButton::clicked, this, &MainWindow::operatorClicked);
    connect(ui->btnMul, &QPushButton::clicked, this, &MainWindow::operatorClicked);
    connect(ui->btnDiv, &QPushButton::clicked, this, &MainWindow::operatorClicked);

    // Functions
    connect(ui->btnSqrt, &QPushButton::clicked, this, &MainWindow::functionClicked);
    connect(ui->btnPow, &QPushButton::clicked, this, &MainWindow::operatorClicked);  // e.g., '^'
    connect(ui->btnSin, &QPushButton::clicked, this, &MainWindow::functionClicked);
    connect(ui->btnCos, &QPushButton::clicked, this, &MainWindow::functionClicked);
    connect(ui->btnTan, &QPushButton::clicked, this, &MainWindow::functionClicked);

    // Special
    connect(ui->btnAns, &QPushButton::clicked, this, &MainWindow::ansClicked);
    connect(ui->btnEng, &QPushButton::clicked, this, &MainWindow::engClicked);
    connect(ui->btnDel, &QPushButton::clicked, this, &MainWindow::delClicked);
    connect(ui->btnAC, &QPushButton::clicked, this, &MainWindow::clearAll);
    connect(ui->btnEqual, &QPushButton::clicked, this, &MainWindow::equalClicked);
    // Parentheses
    connect(ui->btnOpen, &QPushButton::clicked, this, &MainWindow::appendSymbol);
    connect(ui->btnClose, &QPushButton::clicked, this, &MainWindow::appendSymbol);

    // Fraction
    connect(ui->btnFrac, &QPushButton::clicked, this, &MainWindow::fractionClicked);

    // Angle mode toggles
    connect(ui->btnRad, &QPushButton::clicked, this, &MainWindow::setRadianMode);
    connect(ui->btnDeg, &QPushButton::clicked, this, &MainWindow::setDegreeMode);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::digitClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    QString digit = button->text();
    currentInput += digit;
    ui->display->setTextFormat(Qt::RichText);
    ui->display->setText(formatForDisplay(currentInput));
}

// Operator handler (+, -, *, /, ^)
void MainWindow::operatorClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    QString buttonText = button->text();
    QString op;

    // Convert display symbol to parser symbol
    if (buttonText == "×" || buttonText == "x") op = "*";     // ✅ fix for both × and x
    else if (buttonText == "÷") op = "/";
    else if (buttonText == "xⁿ") op = "^";
    else op = buttonText;

    currentInput += op; // currentInput uses parser-friendly version
    ui->display->setTextFormat(Qt::RichText);
    ui->display->setText(formatForDisplay(currentInput)); // display shows fancy symbols
}


// Equals button (=)
void MainWindow::equalClicked()
{
    QString expr = currentInput;  // ✅ Use raw input instead of formatted display
    Parser parser(isRadianMode);

    try {
        double result = parser.evaluate(expr.toStdString());
        currentInput = QString::number(result);
        ui->display->setText(currentInput);
        lastAnswer = currentInput;
    } catch (const std::exception& e) {
        ui->display->setText("Error");
    }

    pendingOperator.clear();
}


// Function handler (sqrt, sin, cos, tan)
void MainWindow::functionClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    QString function = button->text();

    if (function == "√") {
        currentInput += "sqrt(";  // ✅ FIXED: use parser-recognized name
    } else {
        currentInput += function + "(";
    }

    ui->display->setTextFormat(Qt::RichText);
    ui->display->setText(formatForDisplay(currentInput));
}

// Use Ans button
void MainWindow::ansClicked()
{
    currentInput += lastAnswer;
    ui->display->setTextFormat(Qt::RichText);  // <- already there ✅
    ui->display->setText(formatForDisplay(currentInput)); // <- this was missing ❗
}

// Clear all (CA)
void MainWindow::clearAll()
{
    currentInput.clear();
    pendingOperator.clear();
    storedValue = 0;
    ui->display->clear();
}

// Delete last character (Del)
void MainWindow::delClicked()
{
    if (!currentInput.isEmpty()) {
        currentInput.chop(1); // remove last char
        ui->display->setTextFormat(Qt::RichText);  // <- already there ✅
        ui->display->setText(formatForDisplay(currentInput)); // <- this was missing ❗
    }
}

// Engineering notation (optional)
void MainWindow::engClicked()
{
    if (!currentInput.isEmpty()) {
        double val = currentInput.toDouble();
        QString eng = QString::number(val, 'e', 2);  // scientific format
        ui->display->setText(eng);
    }
}
void MainWindow::appendSymbol()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    currentInput += button->text();  // '(' or ')'
    ui->display->setTextFormat(Qt::RichText);  // <- already there ✅
    ui->display->setText(formatForDisplay(currentInput)); // <- this was missing ❗;
}
void MainWindow::fractionClicked()
{
    currentInput += "/";  // ✅ use a real division symbol
    ui->display->setTextFormat(Qt::RichText);  // <- already there ✅
    ui->display->setText(formatForDisplay(currentInput)); // <- this was missing ❗
}

void MainWindow::setRadianMode()
{
    isRadianMode = true;
    ui->lblAngleMode->setText("RAD");
}

void MainWindow::setDegreeMode()
{
    isRadianMode = false;
    ui->lblAngleMode->setText("DEG");
}
void MainWindow::powerClicked()
{
    currentInput += "^";  // ✅ Insert math operator
    ui->display->setTextFormat(Qt::RichText);  // <- already there ✅
    ui->display->setText(formatForDisplay(currentInput)); // <- this was missing ❗
}
QString MainWindow::formatForDisplay(const QString& input) {
    QString output;
    for (int i = 0; i < input.length(); ++i) {
        QChar ch = input[i];

        // Convert "sqrt" into "√"
        if (input.mid(i, 4) == "sqrt") {
            output += "√";
            i += 3;  // skip 's','q','r','t'
        }
        // Superscript formatting for ^ (power)
        else if (ch == '^' && i + 1 < input.length()) {
            QChar next = input[i + 1];
            QString sup;
            if (next == '0') sup = "⁰";
            else if (next == '1') sup = "¹";
            else if (next == '2') sup = "²";
            else if (next == '3') sup = "³";
            else if (next == '4') sup = "⁴";
            else if (next == '5') sup = "⁵";
            else if (next == '6') sup = "⁶";
            else if (next == '7') sup = "⁷";
            else if (next == '8') sup = "⁸";
            else if (next == '9') sup = "⁹";
            else sup = "<sup>" + QString(next) + "</sup>";

            output += sup;
            ++i;  // skip the superscript digit
        }
        // Fractions like a/b
        else if (ch == '/' && i > 0 && i + 1 < input.length()) {
            QChar numerator = input[i - 1];
            QChar denominator = input[i + 1];

            QString fractionHtml =
                "<span style='display:inline-block; text-align:center;'>"
                "<span style='border-bottom:1px solid;'>" + QString(numerator) + "</span><br>" +
                QString(denominator) + "</span>";

            output.chop(1);  // remove numerator that was already added
            output += fractionHtml;
            ++i;  // skip denominator
        }
        else {
            if (ch == '*') output += "×";
            else if (ch == '/') output += "÷";
            else output += ch;
        }
    }

    return output;
}
