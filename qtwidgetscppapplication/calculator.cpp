/* License and Attribution
 *
 *   [ 7 ]   [ 8 ]  [ 9 ]  [ / ]   [ AC ]
 *   [ 4 ]   [ 5 ]  [ 6 ]  [ * ]   [ x^y]
 *   [ 1 ]   [ 2 ]  [ 3 ]  [ + ]
 *   [ . ]   [ 0 ]  [+/-]  [ - ]   |  = |
 *
 * C++ 4-function calculator, plus exponents.
 *
 * This code is derived from the tutorial at https://www.youtube.com/watch?v=txGRU7OrTZo and subject to the original license terms of the author.
 * I have:
 * - made the buttons work
 * - added a decimal point (and made it work)
 * - added exponents using the pow() function
 * - added + - * / and x^y to a small display at the top, so the user knows what operation is in progress.
 * - debugged and tested it
 * - user tested it.
 *
 * It works :)
 *
 * /

/* The control flow is such that the calculator keeps track of just the last value.
 * When a mathoperation such as + or * is pressed then it stores the operation so the user can enter
 * the second operand and then press equal.
 *
 * Pressing equal then completes the operation using the stored value and the stored operation.  While the user is entering the second operation
 * a small display has been added at top to show which math operation is in progress.
 */

#include "calculator.h"
#include "ui_calculator.h"
#include <cmath> //via https://www.programiz.com/cpp-programming/library-function/cmath/pow

#include <QRegularExpression> // via https://doc-snapshots.qt.io/qt6-dev/qregularexpression.html

double lastcalcVal = 0.0;  // keep track of previous number between button presses, initially set to 0.0
bool opRecd = false;       // set to true when an operation is pressed ("op received")
bool newentrybegun = true; // set to true when user has started entering the second operand
bool justcomputed = false; // this will be set to true when the equals sign has been pressed, it will be checked so that if a user starts entering a digit, it is a new input rather than appending to the answer from equals sign.
enum op {
    xNone,
    xDiv,
    xMult,
    xAdd,
    xSub,
    xExp
};

op LastOpCode = xNone; // if there is an operation such as + that has already been pressed

Calculator::Calculator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Calculator)
{
    ui->setupUi(this);


    ui->Display->setText(QString::number(lastcalcVal));
    QPushButton *numButtons[10];
    for(int i = 0; i < 10; ++i) {
        QString butName = "Button" + QString::number(i);
        numButtons[i] = Calculator::findChild<QPushButton *>(butName); //https://www.youtube.com/watch?v=txGRU7OrTZo 19.38
        connect(numButtons[i], SIGNAL(released()), this,
                                      SLOT(NumPressed()));

    }

    connect(ui->Add, SIGNAL(released()), this,
                                  SLOT(MathButtonPressed()));
    connect(ui->Subtract, SIGNAL(released()), this,
                                  SLOT(MathButtonPressed()));
    connect(ui->Multiply, SIGNAL(released()), this,
                                  SLOT(MathButtonPressed()));
    connect(ui->Divide, SIGNAL(released()), this,
                                  SLOT(MathButtonPressed()));

    connect(ui->ExponentButton, SIGNAL(released()), this,
                                  SLOT(MathButtonPressed()));


    connect(ui->ACButton, SIGNAL(released()), this,
                                  SLOT(ACButton()));


    connect(ui->Equals, SIGNAL(released()), this,
                                  SLOT(EqualButton()));

    connect(ui->ChangeSign, SIGNAL(released()), this,
                                  SLOT(ChangeNumberSign()));

    connect(ui->DecimalButton, SIGNAL(released()), this,
                                  SLOT(DecimalButton()));

    //clear everything for first use:
    ui->Display->setText(QString::number(0));
    ui->LastOpDisplay->setText("");
    LastOpCode = xNone;
    lastcalcVal = 0.0;
    newentrybegun = false;
}

Calculator::~Calculator()
{
    delete ui;
}


void Calculator::NumPressed(){
    if((opRecd && !newentrybegun) || justcomputed) {
        ui->Display->setText(""); //if there was an operation received we have to clear the number that is still stored from before it was pressed.
                                  // for example user presses 3 * 5 then when pressing the 5 the 3 needs to be cleared.
                                  // in the original logic the * in this example would clear the first input, however most calculators let the user
                                  // continue to see it. so we show the op in progress in a small display at top instead.
        newentrybegun = true; // a new number has been entered (if the user presses more numbers they will be appended)

        if (justcomputed) {justcomputed = false;} // reset the justcomputed flag.
        }
    QPushButton *button = (QPushButton *)sender();
    QString butVal = button->text();
    QString displayVal = ui->Display->text();

    bool isitzeropoint = false; // track whether it's like 0. or 0.0 or 0.00 etc
    QRegularExpression zeropoint("^0\\.0*$"); //0. and then any number of zeros
    QRegularExpressionMatch anymatchesthough = zeropoint.match(ui->Display->text());
    if (anymatchesthough.hasMatch()) { // it reads 0. or 0.0 or 0.00 or 0.000 etc
        isitzeropoint = true;
         }


    /* if the button value is 0 and we show 0. or 0.0 or 0.00 then we need to add another zero.  */
    if (butVal.toDouble() == 0){

        if (isitzeropoint) { // it reads 0. or 0.0 or 0.00 or 0.000 etc
            QString newVal = displayVal + butVal;
            ui->Display->setText(newVal);
        }


        else
            if (ui->Display->text() == "0") { //if it's exactly 0 and the user presses zero, it doesn't start display "00", we don't do anything
                // do nothing here. user pressed 0 when it read "0" that results in no change.
               }
            else { //otherwise add the zero
            ui->Display->setText(displayVal + butVal);
            }
    }

    else
    if(((displayVal.toDouble() == 0) || (displayVal.toDouble() == 0.0)) && (!isitzeropoint)) { //add exception for when user has started to input "0." to start decimal
        ui->Display->setText(butVal);
    } else {
        QString newVal = displayVal + butVal;
        ui->Display->setText(newVal);
    }
    if (isitzeropoint){
        QString newVal = displayVal + butVal;
        ui->Display->setText(newVal);
        }

}


//from https://stackoverflow.com/questions/650162/why-the-switch-statement-cannot-be-applied-on-strings
op hashit (QString const& inString) {
    if (inString == "/") return xDiv;
    if (inString == "*") return xMult;
    if (inString == "+") return xAdd;
    if (inString == "-") return xSub;
    if (inString == "x^y") return xExp;
    return xNone;
}


void Calculator::MathButtonPressed(){

    QString displayVal = ui->Display->text();
    lastcalcVal = displayVal.toDouble(); //store the previous value since we can't read it anymore later
    QPushButton *button = (QPushButton *)sender();
    QString butVal = button->text();
    LastOpCode = hashit(butVal);
    ui->LastOpDisplay->setText(butVal); //put the math button at the small LastOpCode display at top so user remembers it

    switch(LastOpCode){
        case xDiv:
            opRecd = true;
            newentrybegun = false;
            LastOpCode = xDiv;
            break;

        case xMult:
            opRecd = true;
            LastOpCode = xMult;
            newentrybegun = false;
            break;

        case xAdd:
            opRecd = true;
            LastOpCode = xAdd;
            newentrybegun = false;
            break;

        case xSub:
            opRecd = true;
            LastOpCode = xSub;
            newentrybegun = false;
            break;

        case xExp:
            opRecd = true;
            LastOpCode = xExp;
            newentrybegun = false;
            break;

        case xNone:
            //opRecd = true;
            break;

    }

    //ui->Display->setText("");  //this line commented out since most calculators don't clear the display when an operation is pressed.
                                 // instead we check if opRecd and if so clear it on first button press.
}

void Calculator::EqualButton(){
    //clears the last op display
    ui->LastOpDisplay->setText("");

    double solution = 0.0;
    QString displayVal = ui->Display->text();
    double dblDisplayVal = displayVal.toDouble();

    if(opRecd) { //if there is an op to process

      switch(LastOpCode){
            case xDiv:
                solution = lastcalcVal / dblDisplayVal;
                break;

            case xMult:
                solution = lastcalcVal * dblDisplayVal;
                break;

            case xAdd:
                solution = lastcalcVal + dblDisplayVal;
                break;

            case xSub:
                solution = lastcalcVal - dblDisplayVal;
                break;

            case xExp:
                solution = pow(lastcalcVal, dblDisplayVal);
                break;

           case xNone:
                break;

            }
        ui->Display->setText(QString::number(solution));
        opRecd = false; //now there is no op in the queue
        }
    LastOpCode = xNone;
    newentrybegun = false; //reset this so the user can start anew.
    justcomputed = true; //it's just been cleared so any new digit will be a new one.
}

void Calculator::ChangeNumberSign(){
    QString displayVal = ui->Display->text();
    double dblDisplayVal = displayVal.toDouble();
    ui->Display->setText(QString::number(-1 * dblDisplayVal));
}


void Calculator::ACButton(){

    ui->Display->setText(QString::number(0));
    ui->LastOpDisplay->setText("");
    LastOpCode = xNone;
    lastcalcVal = 0.0;
    newentrybegun = false;

}


void Calculator::DecimalButton(){
    if (justcomputed) {return;} // don't allow decimal if just used equals sign
    QRegularExpression onlydigitsre("^\\d+$"); //user can only just add one decimal point
    QRegularExpressionMatch anymatchesthough = onlydigitsre.match(ui->Display->text());
    if (anymatchesthough.hasMatch()) {
       QString newVal = ui->Display->text() + ".";
       ui->Display->setText(newVal);

       newentrybegun = true; //in case the user started entry by pressing decimal point after clearing or starting calculator.
       }
    // else it's not all digits so we ignore.

}
