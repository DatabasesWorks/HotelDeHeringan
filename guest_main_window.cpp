#include "guest_main_window.hpp"
#include "ui_guest_main_window.h"

#include <QStringList>
#include <QMessageBox>
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QMenu>
#include <QCloseEvent>

#define NO_PARENT nullptr

NewGuestMainWindow::NewGuestMainWindow( QWidget *parent ) :
    QMainWindow( parent ), ui( new Ui::NewGuestMainWindow )
{
    ui->setupUi(this);
    this->setMinimumSize( 800, 520 );
    this->setMaximumSize( 900, 520 );

    setWindowTitle( "Guest's Registration" );
    ui->tabWidget->setCurrentIndex( 0 );

    QStringList const search_by_keywords{ "Search by name", "Search by phone number", "Search by room number" };
    ui->criteria_combo->addItems( search_by_keywords );
    ui->adult_combo->addItems( QStringList{ "1", "2", "3" } );
    ui->children_combo->addItems( QStringList{ "0", "1", "2" } );
    ui->visit_combo->addItems( QStringList{ "Pleasure", "Business", "Others(specify)" } );

    ui->arrival_dt->setDateTime( QDateTime::currentDateTime() );
    ui->departure_dt->setDateTime( QDateTime::currentDateTime().addDays( 1 ) );

    SetupActions();
    SetupToolbar();

    QObject::connect( ui->next_button, SIGNAL(clicked(bool)), this, SLOT(OnNextButtonClicked()) );
}

void NewGuestMainWindow::SetupActions()
{
    save_action_ptr = new QAction( QIcon( ":/icons/icons/save.png" ), "Save", NO_PARENT );
    save_action_ptr->setShortcut( QKeySequence( QString( "Ctrl+N" ) ) );
    save_action_ptr->setStatusTip( "Add new guest entry" );
    QObject::connect( save_action_ptr, SIGNAL(triggered(bool)), this, SLOT( OnNewAddGuestTriggered() ) );

    delete_action_ptr = new QAction( QIcon( ":/icons/icons/delete.png"),"Delete", NO_PARENT );
    delete_action_ptr->setShortcut( QKeySequence( QString( "Ctrl+D" )));
    delete_action_ptr->setStatusTip( "Delete guest information from database" );
    QObject::connect( delete_action_ptr, SIGNAL( triggered(bool)), this, SLOT( OnDeleteAddGuestTriggered() ) );

    cancel_action_ptr = new QAction( QIcon( ":/icons/icons/cancel.png"),"Cancel", NO_PARENT );
    cancel_action_ptr->setShortcut( QKeySequence( QString( "Ctrl+L" ) ) );
    cancel_action_ptr->setStatusTip( "Cancel current operation and close window" );
    QObject::connect( cancel_action_ptr, SIGNAL(triggered(bool)), this, SLOT(OnCancelTriggered()) );

    log_action_ptr = new QAction( QIcon( ":/icons/icons/log.png"),"Log", NO_PARENT );
    log_action_ptr->setShortcut( QKeySequence( QString("Ctrl+O" ) ) );
    log_action_ptr->setStatusTip( "Log current status" );
    QObject::connect( log_action_ptr, SIGNAL(triggered(bool)), this, SLOT(OnLogTriggered()) );

    note_action_ptr = new QAction( QIcon( ":/icons/icons/note.png"), "Note", NO_PARENT );
    note_action_ptr->setShortcut( QKeySequence( QString( "Note" ) ) );
    note_action_ptr->setStatusTip( "Add extra note to the current guest basic information" );
    QObject::connect( note_action_ptr, SIGNAL(triggered(bool)), this, SLOT(OnTakeNoteTriggered()) );

    refresh_action_ptr = new QAction( QIcon( ":/icons/icons/refresh.png"), "Refresh", NO_PARENT );
    refresh_action_ptr->setShortcut( QKeySequence( QString( "Ctrl+R" )));
    refresh_action_ptr->setStatusTip( "Refresh when hung on an operation" );

    close_action_ptr = new QAction( QIcon( ":/icons/icons/close.png"), "Close", NO_PARENT );
    close_action_ptr->setShortcut( QKeySequence( QString( "Ctrl+Q" )));
    close_action_ptr->setStatusTip( "Close Window" );
    QObject::connect( close_action_ptr, SIGNAL(triggered(bool)), this, SLOT(close()) );
}

void NewGuestMainWindow::closeEvent( QCloseEvent * )
{

}

void NewGuestMainWindow::SetupToolbar()
{
    QToolBar* toolbar = new QToolBar( "SubMenu" );
    toolbar->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
    this->addToolBar( Qt::LeftToolBarArea, toolbar );

    toolbar->addAction( save_action_ptr );
    toolbar->addSeparator();
    toolbar->addAction( delete_action_ptr );
    toolbar->addSeparator();
    toolbar->addAction( cancel_action_ptr );
    toolbar->addSeparator();
    toolbar->addAction( log_action_ptr );
    toolbar->addSeparator();
    toolbar->addAction( note_action_ptr );
    toolbar->addSeparator();
    toolbar->addAction( refresh_action_ptr );
    toolbar->addSeparator();
    toolbar->addAction( close_action_ptr );
}

void NewGuestMainWindow::OnNewAddGuestTriggered()
{

}

void NewGuestMainWindow::OnDeleteAddGuestTriggered()
{

}

template<typename T>
bool IsNull( T const & str ){ return str.isNull(); }

template<typename T, typename... U>
bool IsNull( T const & str, U &&...other_str )
{
    return IsNull( str ) || IsNull( std::forward<U>( other_str )... );
}

void NewGuestMainWindow::OnNextButtonClicked()
{
    bool const any_is_null = IsNull( ui->surname_edit->text(), ui->other_names_edit->text(),
                                     ui->nationality_edit->text(), ui->occupation_edit->text(),
                                     ui->address_tedit->toPlainText(), ui->arrival_from_edit->text(),
                                     ui->departure_to_edit->text(), ui->phone_number_edit->text(),
                                     ui->vehicle_edit->text(), ui->nok_name_edit->text(),
                                     ui->id_card_edit->text(), ui->account_settled_edit->text(),
                                     ui->nok_address_tedit->toPlainText(), ui->nok_phone_edit->text() );
    if( any_is_null ){
        QMessageBox::critical( this, "Error", "One of the information fields is left unfilled, please"
                                              " cross-check and try again.", QMessageBox::Ok );
        return;
    }
}

void NewGuestMainWindow::OnCancelTriggered()
{

}

void NewGuestMainWindow::OnLogTriggered()
{

}

void NewGuestMainWindow::OnTakeNoteTriggered()
{

}

NewGuestMainWindow::~NewGuestMainWindow()
{
    delete ui;
}

#undef NO_PARENT
