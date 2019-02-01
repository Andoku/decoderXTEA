#ifndef DECODER_H
#define DECODER_H

#include <QString>

class Decoder
{
public:
    Decoder();
    bool decode(QString package);

    QString getError() const;
    QString getData() const;
    QString getImei() const;
private:
    QString error;
    QString data;
    QString imei;

    bool clampPackage(QByteArray *data);
    void unstuffPackage(QByteArray *data);
    bool getImei(const QByteArray &data);
    bool checkCRC(const QByteArray &data);
};

#endif // DECODER_H
