#include "decoder.h"

namespace {
const quint8 header = 0xC0;
const quint8 footer = 0xC2;
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
    if(!getImei(hexData)) {
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

bool Decoder::getImei(const QByteArray &data)
{
    const int imeiSize = 8;
    if(data.size() < imeiSize) {
        return false;
    }
    imei = data.mid(0, imeiSize).toHex();
    return true;
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
