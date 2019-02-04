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
    QString getCrc() const;
private:
    QString error;
    QString data;
    QString imei;
    quint16 crc;

    bool unstuffPackage(QByteArray *data);
    bool getImei(QByteArray *data);
    bool getCrc(QByteArray *data);
    bool decipherData(QByteArray *data);
    quint16 calculateCRC(const QByteArray &data);

    void xteaDecipher(unsigned int num_rounds, quint32 v[2], quint32 const k[4]) const;
};

#endif // DECODER_H
