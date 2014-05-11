#include "messagebox.h"

MessageBox::MessageBox(QWidget *parent) :
    QMessageBox(parent)
{
}

void MessageBox::showEvent(QShowEvent * /*event*/)
{
    currentTime = 0;
    if (autoClose) {
        this->startTimer(1000);
    }
}

void MessageBox::timerEvent(QTimerEvent */*event*/)
{
    currentTime++;
    if (currentTime >= timeout) {
        this->done(0);
    }
}

void MessageBox::setAutoClose(bool on)
{
    autoClose = on;
}

void MessageBox::setTimeout(qreal sec)
{
    timeout = sec;
}
