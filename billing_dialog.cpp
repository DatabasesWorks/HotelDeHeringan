#include "billing_dialog.hpp"
#include "ui_billing_dialog.h"

BillingDialog::BillingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BillingDialog)
{
    ui->setupUi(this);
}

BillingDialog::~BillingDialog()
{
    delete ui;
}
