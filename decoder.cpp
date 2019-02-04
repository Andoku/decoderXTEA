#include "decoder.h"

namespace
{
const QString keyText = "01020304050607081112131415161718";
}

Decoder::Decoder() :
    crc(0) {}

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

    if(!decipherData(&hexData)) {
        error = "Can't decipher data";
        return false;
    }

    if(!getCrc(&hexData) || crc != calculateCRC(hexData)) {
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

bool Decoder::getCrc(QByteArray *data)
{
    const int crcSize = 2;
    if(data->size() < crcSize) {
        return false;
    }

    crc = ((quint8) (*data)[data->size() - 1] << 8) | (quint8) (*data)[data->size() - 2];
    data->remove(data->size() - crcSize, crcSize);
    return true;
}

bool Decoder::decipherData(QByteArray *data)
{
    const int blockSize = 8;
    if(data->size() % blockSize != 0) {
        return false;
    }

    const QByteArray key = QByteArray::fromHex(keyText.toUtf8());
    const int blockNumber = data->size() / blockSize;
    const int rounds = 32;
    for (int i = 0; i < blockNumber; ++i) {
        xteaDecipher(rounds, (quint32 *) (data->data() + i * blockSize), (quint32 *) key.data());
    }

    return true;
}

void Decoder::xteaDecipher(unsigned int rounds, quint32 v[2], quint32 const k[4]) const
{
    const quint32 delta = 0x9E3779B9;
    quint32 sum = delta * rounds;
    quint32 v0 = v[0];
    quint32 v1 = v[1];
    for (unsigned int i = 0; i < rounds; ++i) {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum >> 11) & 3]);
        sum -= delta;
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
    }

    v[0] = v0;
    v[1] = v1;
}

quint16 Decoder::calculateCRC(const QByteArray &data) const
{
    quint16 crc = 0xFFFF;
    for(int i = 0; i < data.size(); ++i) {
        crc ^= (quint8) data[i] << 8;
        for (int j = 0; j < 8; ++j) {
            crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
        }
    }

    return crc;
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

QString Decoder::getCrc() const
{
    return QString::number(crc, 16);
}
