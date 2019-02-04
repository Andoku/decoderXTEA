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
    quint32 key[4];

    bool unstuffPackage(QByteArray *data);
    bool getImei(QByteArray *data);
    bool decipherData(QByteArray *data);
    bool checkCRC(const QByteArray &data);
    bool convertKey(QString key);

    quint32 muladd128(quint32 quad[4], const quint32 mul, const quint32 add) const;
};

#endif // DECODER_H
