#include "role_definition_dialog.hpp"
#include "ui_role_definition_dialog.h"
#include <Wt/Dbo/Session.h>

RoleDefinitionDialog::RoleDefinitionDialog( Wt::Dbo::Session &db_session, QWidget *parent ) :
    QDialog( parent ), ui( new Ui::RoleDefinitionDialog ), database_session( db_session )
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex( 0 );

    QSize const min_max_size{ 458, 430 };
    this->setMinimumSize( min_max_size );
    this->setMaximumSize( min_max_size );

    setWindowTitle( "Role definition" );
}

RoleDefinitionDialog::~RoleDefinitionDialog()
{
    delete ui;
}
