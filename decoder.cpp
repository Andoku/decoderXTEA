#include <QCryptographicHash>

#include "decoder.h"

namespace
{
const QString keyText = "01020304050607081112131415161718";
}

Decoder::Decoder() {}

bool Decoder::decode(QString package)
{
    QByteArray hexData = QByteArray::fromHex(package.toUtf8());

    if(!unstuffPackage(&hexData)) {
        error = "Wrong package format";
        return false;
    }

    if(!getImei(&hexData)) {
        error = "Can't get IMEI - wrong package size";
        return false;
    }

    if(!convertKey(keyText)) {
        error = "Wrong key format";
        return false;
    }

    if(!decipherData(&hexData)) {
        error = "Can't decipher data";
        return false;
    }

    if(!checkCRC(hexData)) {
        error = "Wrong crc";
        return false;
    }

    data = hexData.toHex();
    return true;
}

bool Decoder::unstuffPackage(QByteArray *data)
{
    const int headerIndex = data->indexOf(0xC0);
    const int footerIndex = data->indexOf(0xC2);

    if(headerIndex < 0 || footerIndex < 0 || !(headerIndex < footerIndex)) {
        return false;
    }

    data->remove(footerIndex, data->size() - footerIndex);
    data->remove(0, headerIndex + 1);
    data->replace(QByteArray("\xC4\xC1"), QByteArray("\xC0"));
    data->replace(QByteArray("\xC4\xC3"), QByteArray("\xC2"));
    data->replace(QByteArray("\xC4\xC4"), QByteArray("\xC4"));
    return true;
}

bool Decoder::getImei(QByteArray *data)
{
    const int imeiSize = 8;
    if(data->size() < imeiSize) {
        return false;
    }
    imei = data->mid(0, imeiSize).toHex();
    data->remove(0, imeiSize);
    return true;
}

bool Decoder::decipherData(QByteArray *data)
{
    return false;
}

void xteaDecipher(unsigned int num_rounds, quint32 v[2], quint32 const k[4])
{
    const quint32 delta = 0x9E3779B9;
    quint32 sum = delta * num_rounds;
    quint32 v0 = v[0];
    quint32 v1 = v[1];

    for (unsigned int i = 0; i < num_rounds; ++i) {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum >> 11) & 3]);
        sum -= delta;
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
    }

    v[0] = v0;
    v[1] = v1;
}

bool Decoder::checkCRC(const QByteArray &data)
{
    const int len = data.size() - 2;
    if(len <= 0) {
        return false;
    }

    quint16 crc = 0xFFFF;
    for(int i = 0; i < len; ++i) {
        crc ^= data[i] << 8;
        for (int j = 0; j < 8; ++j) {
            crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
        }
    }

    const quint16 rxCrc = (data[data.size() - 1] << 8) | data[data.size() - 2];
    return rxCrc == crc;
}

bool Decoder::convertKey(QString keyText)
{
    key[0] = key[1] = key[2] = key[3] = 0;

    for(int i = 0; i < keyText.size(); ++i) {
        if (keyText[i].isDigit()) {
            if (muladd128(key, 10, keyText[i].digitValue())) {
                return false;
            }
        } else {
            return false;
        }
    }

    return true;
}

quint32 Decoder::muladd128(quint32 quad[4], const quint32 mul, const quint32 add) const
{
    quint64 temp = 0;

    temp = (quint64) quad[3] * (quint64) mul + add;
    quad[3] = temp;

    temp = (quint64) quad[2] * (quint64) mul + (temp >> 32);
    quad[2] = temp;

    temp = (quint64) quad[1] * (quint64) mul + (temp >> 32);
    quad[1] = temp;

    temp = (quint64) quad[0] * (quint64) mul + (temp >> 32);
    quad[0] = temp;

    return temp >> 32;
}

QString Decoder::getError() const
{
    return error;
}

QString Decoder::getData() const
{
    return data;
}

QString Decoder::getImei() const
{
    return imei;
}
