#include "mainwindow.hpp"

#include <QKeySequence>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QToolBar>
#include <QApplication>
#include <Wt/Dbo/Query.h>

#include "login_dialog.hpp"
#include "guest_main_window.hpp"
#include "role_definition_dialog.hpp"
#include "reservation_dialog.hpp"
#include "utilities.hpp"

namespace dbo = Wt::Dbo;

MainWindow::MainWindow( std::unique_ptr<Wt::Dbo::Session> session, QWidget *parent ) :
    QMainWindow( parent ), database_session( std::move( session ) ), login_session{}
{
    ptr_workspace = new QMdiArea( this );
    setCentralWidget( ptr_workspace );

    setWindowIcon( QIcon( ":/icons/icons/favicon.ico" ) );
    setWindowState( Qt::WindowMaximized );
    setWindowTitle( "Heringan Front Office" );
    SetActions();
    SetMenus();
    SetToolbar();
    this->setStatusTip( "Done" );

    ToggleSessionActivation( false );
    QObject::connect( this, SIGNAL(windowShown()), this, SLOT(PromptLogin()) );
}

MainWindow::~MainWindow()
{
}

void MainWindow::show()
{
    QMainWindow::show();
    QApplication::processEvents();
    emit windowShown();
}

void MainWindow::PromptLogin()
{
    LoginDialog *ptr_login_dialog = new LoginDialog( ptr_workspace );
    if( ptr_login_dialog->exec() == QDialog::Accepted ){
        QString const username = ptr_login_dialog->GetUsername();
        QString const password =  ptr_login_dialog->GetPassword();
        if( username.isNull() || password.isNull() ){
            QMessageBox::critical( this, "Error", "The username and the password may not be empty" );
            return;
        }
        try{
            dbo::Transaction transaction{ *database_session };
            dbo::Query<dbo::ptr<utilities::Staff>> user_query = database_session->find<utilities::Staff>()
                    .where( "username = ?"  ).bind( username.toLower().toStdString() );
            auto const & user_ptr = user_query.resultValue();
            if( !user_ptr ){
                QMessageBox::critical( this, "Error", "Username/Password was not found" );
            } else {
                if( !utilities::is_correct_password( password.toStdString(), user_ptr->password_hash )){
                    QMessageBox::critical( this, "Error", "Username/Password is incorrect" );
                } else {
                    login_session.is_logged_in = true;
                    login_session.name = user_ptr->name;
                    login_session.username = username.toStdString();
                    login_session.user_permission = user_ptr->assigned_role.permission;
                    ToggleSessionActivation( true );
                    QMessageBox::information( this, "Login", tr( "Welcome, %1").arg( user_ptr->name.c_str() ) );
                }
            }
        } catch( dbo::Exception const & exception ){
            qDebug() << exception.what();
        }
    }
}

void MainWindow::ToggleSessionActivation( bool activate)
{
    ptr_login_action->setEnabled( !activate );
    ptr_logout_action->setEnabled( activate );
    ptr_guest_registration_action->setEnabled( activate );
    ptr_define_role_action->setEnabled( activate );
    ptr_reservation_action->setEnabled( activate );
    ptr_reports_action->setEnabled( activate );
    ptr_expenses_action->setEnabled( activate );
}

void MainWindow::SetActions()
{
    ptr_login_action = new QAction( QIcon(":/icons/icons/login.png"), tr("Login"), NO_PARENT );
    ptr_login_action->setShortcut( QKeySequence( QString("Ctrl+L")) );
    ptr_login_action->setStatusTip( "Sign in a new user" );
    QObject::connect( ptr_login_action, SIGNAL(triggered(bool)), this, SLOT(PromptLogin()));

    ptr_logout_action = new QAction( QIcon(":/icons/icons/logout.png"), tr("Logout"), NO_PARENT );
    ptr_logout_action->setShortcut( QString( "Ctrl+O" ) );
    ptr_logout_action->setStatusTip( "Signs out a logged on user" );
    QObject::connect( ptr_logout_action, SIGNAL(triggered(bool)), this, SLOT(ToggleSessionActivation(bool)));

    ptr_guest_registration_action = new QAction( QIcon( ":/icons/icons/registration.png" ),
                                                 tr("Guest Registration"), NO_PARENT );
    ptr_guest_registration_action->setShortcut( QString("Ctrl+R") );
    ptr_guest_registration_action->setStatusTip( "Add new guests to the Guest's Registration Form" );
    QObject::connect( ptr_guest_registration_action, SIGNAL(triggered(bool)), this, SLOT(AddGuestInformation()) );

    ptr_define_role_action = new QAction( QIcon(":/icons/icons/define_roles.png"), tr("Define roles"),
                                          NO_PARENT);
    ptr_define_role_action->setShortcut( QString( "Ctrl+D" ) );
    ptr_define_role_action->setStatusTip( "Allows the administrator add new staffs and define what they can do" );
    QObject::connect( ptr_define_role_action, SIGNAL(triggered(bool)), this, SLOT(DefineRoles()) );

    ptr_reservation_action = new QAction( QIcon(":/icons/icons/reservations.png"), tr("Reservations"),
                                          NO_PARENT );
    ptr_reservation_action->setShortcut( QString( "Ctrl+H" ) );
    ptr_reservation_action->setStatusTip( "Add new and check existing reservations" );
    QObject::connect( ptr_reservation_action, SIGNAL(triggered(bool)), this, SLOT(AddCheckReservation()) );

    ptr_reports_action = new QAction( QIcon(":/icons/icons/reports.png"), tr("Reports"),
                                      NO_PARENT );
    ptr_reports_action->setShortcut( QString("Ctrl+T" ) );
    ptr_reports_action->setStatusTip( "Check reports" );
    QObject::connect( ptr_reports_action, SIGNAL(triggered(bool)), this, SLOT(ReportClicked()) );

    ptr_expenses_action = new QAction( QIcon(":/icons/icons/expenses.png"), tr("Expenses"),
                                       NO_PARENT );
    ptr_expenses_action->setShortcut( QString( "Ctrl+X" ) );
    ptr_expenses_action->setStatusTip( "Add new and check all expenses" );
    QObject::connect( ptr_expenses_action, SIGNAL(triggered(bool)), this, SLOT(ExpensesClicked()) );
}


void MainWindow::ReportClicked()
{

}

void MainWindow::ExpensesClicked()
{

}

void MainWindow::AddCheckReservation()
{
    ReservationDialog *const reservation_dialog = new ReservationDialog( ptr_workspace );
    reservation_dialog->exec();
}

void MainWindow::DefineRoles()
{
    RoleDefinitionDialog *const role_definition_dialog = new RoleDefinitionDialog( *database_session,
                                                                                   ptr_workspace );
    role_definition_dialog->exec();
}

void MainWindow::AddGuestInformation()
{
    NewGuestMainWindow *const guest_registration_window = new NewGuestMainWindow( this );
    guest_registration_window->show();
}

void MainWindow::closeEvent( QCloseEvent *event )
{
    if( login_session.is_logged_in ){
        if( QMessageBox::information( this, "Log out",
                                      "This will log you out of your account, continue to log out?",
                                      QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes )
        {
            login_session.is_logged_in = false;
        }
        event->ignore();
    } else {
        if( QMessageBox::information( this, "Close", "Exit application?", QMessageBox::Yes | QMessageBox::No )
                == QMessageBox::Yes ){
            event->accept();
        } else {
            event->ignore();
        }
    }
}

void MainWindow::SetMenus()
{
    QMenu *ptr_controls_menu;
    QMenu *ptr_privileged_controls_menu;
    QMenu *ptr_trivial_controls_menu;

    ptr_controls_menu = menuBar()->addMenu( QIcon(""), "Controls" );
    ptr_controls_menu->addAction( ptr_login_action );
    ptr_controls_menu->addSeparator();
    ptr_controls_menu->addAction( ptr_logout_action );

    ptr_privileged_controls_menu = menuBar()->addMenu( QIcon(), "Main Menu" );
    ptr_privileged_controls_menu->addSeparator();
    ptr_privileged_controls_menu->addAction( ptr_guest_registration_action );
    ptr_privileged_controls_menu->addSeparator();
    ptr_privileged_controls_menu->addAction( ptr_define_role_action );

    ptr_trivial_controls_menu = ptr_privileged_controls_menu->addMenu( QIcon(), "Operations" );
    ptr_trivial_controls_menu->addSeparator();
    ptr_trivial_controls_menu->addAction( ptr_reservation_action );
    ptr_trivial_controls_menu->addSeparator();
    ptr_trivial_controls_menu->addAction( ptr_expenses_action );
    ptr_trivial_controls_menu->addSeparator();
    ptr_trivial_controls_menu->addAction( ptr_reports_action );
}

void MainWindow::SetToolbar()
{
    QToolBar* main_toolbar = this->addToolBar( "Toolbar" );
    main_toolbar->addAction( ptr_guest_registration_action );
    main_toolbar->addSeparator();
    main_toolbar->addAction( ptr_define_role_action );
    main_toolbar->addSeparator();
    main_toolbar->addAction( ptr_reservation_action );
    main_toolbar->addSeparator();
    main_toolbar->addAction( ptr_reports_action );
    main_toolbar->addSeparator();
    main_toolbar->addAction( ptr_expenses_action );
}

#undef NO_PARENT
