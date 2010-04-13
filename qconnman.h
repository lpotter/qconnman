#ifndef QCONNMAN_H
#define QCONNMAN_H

#include <QObject>
#include <QVariant>


class QConnman : public QObject
{
Q_OBJECT
public:
    explicit QConnman(QObject *parent = 0);


signals:
    void done();
public slots:
    void getInfo();
    void stateChanged(const QString&);
    void propertyChanged(const QString&, QVariant&);
};

#endif // QCONNMAN_H
