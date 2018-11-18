#include "admin_request_dialog.hpp"
#include "ui_admin_request_dialog.h"

AdminRequestDialog::AdminRequestDialog(QWidget *parent) :
    QDialog(parent),
    ui( new Ui::AdminRequestDialog )
{
    ui->setupUi(this);

    this->setWindowTitle( "Administrator's Request" );
    auto const constant_windows_size = QSize( 310, 350 );
    this->setMinimumSize( constant_windows_size );
    this->setMaximumSize( constant_windows_size );
}

AdminRequestDialog::~AdminRequestDialog()
{
    delete ui;
}

void AdminRequestDialog::ClearPasswordFields()
{
    ui->confirm_password->clear();
    ui->password_edit->clear();
    ui->password_edit->setFocus();
}

std::string AdminRequestDialog::GetUsername()
{
    return ui->username_edit->text().toStdString();
}

std::string AdminRequestDialog::GetPassword()
{
    return ui->password_edit->text().toStdString();
}

std::string AdminRequestDialog::GetConfirmPassword()
{
    return ui->confirm_password->text().toStdString();
}

std::string AdminRequestDialog::GetHost()
{
    return ui->host_edit->text().toStdString();
}

std::string AdminRequestDialog::GetDatabaseName()
{
    return ui->dbname_edit->text().toStdString();
}

long AdminRequestDialog::GetPortNumber()
{
    return ui->port_edit->text().toLong();
}
