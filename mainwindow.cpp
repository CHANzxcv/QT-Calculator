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
    connect(ui->btnPow, &QPushButton::clicked, this, &MainWindow::operatorClicked);
    connect(ui->btnMod, &QPushButton::clicked, this, &MainWindow::modClicked);

    // Functions
    connect(ui->btnSqrt, &QPushButton::clicked, this, &MainWindow::functionClicked);
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

    // Arrow keys
    connect(ui->btnLeft, &QPushButton::clicked, this, &MainWindow::arrowClicked);
    connect(ui->btnRight, &QPushButton::clicked, this, &MainWindow::arrowClicked);
    connect(ui->btnUp, &QPushButton::clicked, this, &MainWindow::arrowClicked);
    connect(ui->btnDown, &QPushButton::clicked, this, &MainWindow::arrowClicked);
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
    currentInput.insert(cursorPosition, digit); // insert at cursor
    cursorPosition += digit.length();           // move cursor forward
    ui->display->setTextFormat(Qt::RichText);
    ui->display->setText(formatForDisplay(currentInput));
}

// Modulo handler (Mod)
void MainWindow::modClicked() {
    currentInput.insert(cursorPosition, "mod");
    cursorPosition += 3;
    ui->display->setTextFormat(Qt::RichText);
    ui->display->setText(formatForDisplay(currentInput));
}

// Operator handler (+, -, *, /, ^)
void MainWindow::operatorClicked() {
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    QString buttonText = button->text();
    QString op;

    // Convert GUI symbols to parser-friendly code
    if (buttonText == "×") op = "*";
    else if (buttonText == "÷") op = "/";
    else if (buttonText == "xⁿ") op = "^";
    else if (buttonText == "mod") op = "%";
    else op = buttonText;

    currentInput.insert(cursorPosition, op);
    cursorPosition += op.length();

    ui->display->setTextFormat(Qt::RichText);
    ui->display->setText(formatForDisplay(currentInput));
}



// Equals button (=)
void MainWindow::equalClicked()
{
    QString expr = currentInput;
    expr.replace("mod", "%", Qt::CaseInsensitive); // Replace 'mod' with '%'
    Parser parser(isRadianMode);

    // Debug: Show the expression being evaluated
    qDebug() << "Evaluating expression:" << expr;

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
void MainWindow::functionClicked() {
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    QString function = button->text();
    QString funcText = (function == "√") ? "sqrt(" : function + "(";

    currentInput.insert(cursorPosition, funcText);
    cursorPosition += funcText.length();

    ui->display->setTextFormat(Qt::RichText);
    ui->display->setText(formatForDisplay(currentInput));
}


// Use Ans button
void MainWindow::ansClicked()
{
    currentInput.insert(cursorPosition, lastAnswer);
    cursorPosition += lastAnswer.length();
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
        if (cursorPosition > 0) {
            currentInput.remove(cursorPosition - 1, 1);
            cursorPosition--;
        }
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

    QString symbol = button->text();
    currentInput.insert(cursorPosition, symbol);
    cursorPosition += symbol.length();
    ui->display->setTextFormat(Qt::RichText);  // <- already there ✅
    ui->display->setText(formatForDisplay(currentInput)); // <- this was missing ❗;
}
void MainWindow::fractionClicked()
{
    currentInput.insert(cursorPosition, "/");
    cursorPosition++;
    ui->display->setTextFormat(Qt::RichText);
    ui->display->setText(formatForDisplay(currentInput));
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

void MainWindow::arrowClicked() {
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    QString name = button->objectName();

    if (name == "btnLeft" && cursorPosition > 0) {
        cursorPosition--;
    } else if (name == "btnRight" && cursorPosition < currentInput.length()) {
        cursorPosition++;
    } else if (name == "btnUp") {
        // Optional: add scroll-up functionality
    } else if (name == "btnDown") {
        // Optional: add scroll-down functionality
    }

    // Insert cursor visually in display
    QString inputWithCursor = currentInput;
    inputWithCursor.insert(cursorPosition, "|");

    ui->display->setTextFormat(Qt::RichText);
    ui->display->setText(formatForDisplay(inputWithCursor));
}
QString MainWindow::formatForDisplay(const QString& input) {
    QString output;
    int i = 0;

    while (i < input.length()) {
        QChar ch = input[i];

        // Convert "sqrt" to "√"
        if (input.mid(i, 4) == "sqrt") {
            output += "√";
            i += 4;
            continue;
        }

        // Power formatting (superscript)
        if (ch == '^' && i + 1 < input.length()) {
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
            i += 2;
            continue;
        }

        // Fraction formatting (just show as "numerator/denominator")
        if (ch == '/' && i > 0 && i + 1 < input.length()) {
            QString numerator, denominator;
            int j = i - 1;
            while (j >= 0 && (input[j].isDigit() || input[j] == '.')) {
                numerator.prepend(input[j]);
                --j;
            }
            int k = i + 1;
            while (k < input.length() && (input[k].isDigit() || input[k] == '.')) {
                denominator += input[k];
                ++k;
            }
            if (!numerator.isEmpty() && !denominator.isEmpty() &&
                (j < 0 || !input[j].isLetterOrNumber()) &&
                (k >= input.length() || !input[k].isLetterOrNumber())) {
                output.chop(numerator.length());
                output += numerator + "/" + denominator;
                i = k;
                continue;
            }
        }

        // Operator symbols
        if (ch == '*') output += "×";
        else if (ch == '/') output += "÷";
        else output += ch;

        ++i;
    }

    return output;
}



