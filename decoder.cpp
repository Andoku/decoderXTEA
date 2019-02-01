#include <QCryptographicHash>

#include "decoder.h"

namespace {
const quint8 header = 0xC0;
const quint8 footer = 0xC2;
const QString key = "01020304050607081112131415161718";
}

Decoder::Decoder() {}

bool Decoder::decode(QString package)
{
    QByteArray hexData = QByteArray::fromHex(package.toUtf8());
    if(!clampPackage(&hexData)) {
        error = "Wrong package format";
        return false;
    }
    unstuffPackage(&hexData);
    if(!getImei(&hexData)) {
        error = "Can't get IMEI - wrong package size";
        return false;
    }
    data = hexData.toHex();
    return true;
}

bool Decoder::clampPackage(QByteArray *data)
{
    const int headerIndex = data->indexOf(header);
    const int footerIndex = data->indexOf(footer);

    if(headerIndex < 0 || footerIndex < 0 || !(headerIndex < footerIndex)) {
        return false;
    }

    data->remove(footerIndex, data->size() - footerIndex);
    data->remove(0, headerIndex + 1);
    return true;
}

void Decoder::unstuffPackage(QByteArray *data)
{
    data->replace(QByteArray("\xC4\xC1"), QByteArray("\xC0"));
    data->replace(QByteArray("\xC4\xC3"), QByteArray("\xC2"));
    data->replace(QByteArray("\xC4\xC4"), QByteArray("\xC4"));
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

void Decoder::decipherData(QByteArray *data)
{
    QByteArray keyData = key.toUtf8();

}

void xteaDecipher(unsigned int num_rounds, quint32 v[2], quint32 const k[4]) {
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
