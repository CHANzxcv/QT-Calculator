#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void digitClicked();
    void operatorClicked();
    void functionClicked();
    void equalClicked();
    void clearAll();
    void delClicked();
    void ansClicked();
    void engClicked();
    void powerClicked();
    void appendSymbol();        // For btnOpen, btnClose
    void fractionClicked();     // For btnFrac
    void setRadianMode();       // For btnRad
    void setDegreeMode();       // For btnDeg


private:
    Ui::MainWindow *ui;
    QString formatForDisplay(const QString& input);
    QString currentInput;
    QString lastAnswer = "0";
    QString pendingOperator;
    double storedValue = 0;
    bool isRadianMode = false; // false = degree, true = radian
};

#endif // MAINWINDOW_H
