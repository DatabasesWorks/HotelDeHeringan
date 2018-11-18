#include "login_dialog.hpp"
#include "ui_login_dialog.h"

#include <QSize>

LoginDialog::LoginDialog( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::LoginDialog )
{
    ui->setupUi( this );
    ui->password_text->setEchoMode( QLineEdit::Password );
    setMaximumSize( QSize( 314, 166 ) );
    setMinimumSize( QSize( 314, 166 ) );

    this->setWindowTitle( "Login" );
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

QString LoginDialog::GetUsername()
{
    return ui->username_text->text();
}

QString LoginDialog::GetPassword()
{
    return ui->password_text->text();
}
