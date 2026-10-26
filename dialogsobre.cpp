#include "dialogsobre.h"
#include "ui_dialogsobre.h"

DialogSobre::DialogSobre(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogSobre)
{
    ui->setupUi(this);
}

DialogSobre::~DialogSobre()
{
    delete ui;
}

void DialogSobre::on_pushButton_clicked()
{
    this->close();
}

