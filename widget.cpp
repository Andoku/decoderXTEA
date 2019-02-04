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
    QString text = ui->input->toPlainText();
    ui->result->clear();

    if(decoder.decode(text)) {
        ui->result->insertHtml("<b>IMEI: </b>" + decoder.getImei() + "<br>");
        ui->result->insertHtml("<b>Data: </b>" + decoder.getData() + "<br>");
        ui->result->insertHtml("<b>CRC: </b>" + decoder.getCrc() + "<br>");
    } else {
        ui->result->insertPlainText("Error: " + decoder.getError() + "\n");
    }
}
