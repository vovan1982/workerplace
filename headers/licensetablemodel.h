#ifndef LICENSEMODELFORDEVICE_H
#define LICENSEMODELFORDEVICE_H

#include <QObject>

class LicenseModelForDevice : public QObject
{
    Q_OBJECT
public:
    explicit LicenseModelForDevice(QObject *parent = nullptr);

signals:

public slots:
};

#endif // LICENSEMODELFORDEVICE_H