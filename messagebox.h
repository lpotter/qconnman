#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QMessageBox>

class MessageBox : public QMessageBox
{
    Q_OBJECT
public:
    explicit MessageBox(QWidget *parent = 0);

signals:

public slots:

    void setAutoClose(bool);
    void setTimeout(qreal sec);
private:

    int timeout;
    bool autoClose;
    int currentTime;

    void showEvent(QShowEvent * event);
    void timerEvent(QTimerEvent *event);
};

#endif // MESSAGEBOX_H
