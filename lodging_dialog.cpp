#include "lodging_dialog.hpp"
#include "ui_lodging_dialog.h"

LodgingDialog::LodgingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LodgingDialog)
{
    ui->setupUi(this);
}

LodgingDialog::~LodgingDialog()
{
    delete ui;
}
