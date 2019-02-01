#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    connect(ui->decodeButton, SIGNAL(released()), this, SLOT(handleDecodeButton()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::handleDecodeButton()
{
    QString text = ui->plainTextEdit->toPlainText();
    ui->result->clear();

    if(decoder.decode(text)) {
        ui->result->insertPlainText("IMEI: " + decoder.getImei() + "\n");
        ui->result->insertPlainText("Data: " + decoder.getData() + "\n");
    } else {
        ui->result->insertPlainText("Error: " + decoder.getError() + "\n");
    }
}
