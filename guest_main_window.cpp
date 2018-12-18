#include "guest_main_window.hpp"
#include "ui_guest_main_window.h"

#include <QStringList>
#include <QMessageBox>
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QMenu>
#include <QDebug>
#include <QCloseEvent>
#include <QFileDialog>
#include <QPixmap>
#include <QCamera>
#include <QCameraInfo>
#include <QCameraImageCapture>
#include <QCameraViewfinder>
#include <Wt/Dbo/Transaction.h>

#include "camera_capture.hpp"
#include "utilities.hpp"

#define NO_PARENT nullptr
#define MAX_PASSPORT_WIDTH 340
#define MAX_PASSPORT_HEIGHT 340

namespace dbo = Wt::Dbo;

NewGuestMainWindow::NewGuestMainWindow( std::unique_ptr<Wt::Dbo::Session> &db_session, QWidget *parent ) :
    QMainWindow( parent ), ui( new Ui::NewGuestMainWindow ), database_session{ db_session }
{
    ui->setupUi(this);
    this->setMinimumSize( 800, 520 );
    this->setMaximumSize( 900, 520 );

    setWindowTitle( "Guest's Registration" );
    ui->tabWidget->setCurrentIndex( 0 );

    QStringList const search_by_keywords{ "Search by name", "Search by phone number", "Search by room number" };
    ui->criteria_combo->addItems( search_by_keywords );

    SetupActions();
    SetupToolbar();
    ui->passport_tab->setEnabled( false );

    QObject::connect( ui->save_button, SIGNAL(clicked(bool)), this, SLOT( OnNewAddGuestTriggered() ) );
    QObject::connect( ui->next_button, SIGNAL(clicked(bool)), this, SLOT( OnNextButtonClicked()) );
    QObject::connect( ui->upload_passport_button, SIGNAL(clicked(bool)), this, SLOT( OnUploadPassportClicked()) );
    QObject::connect( ui->use_camera_button, SIGNAL(clicked(bool)), this, SLOT( OnUseCameraClicked()));
}

void NewGuestMainWindow::SetupActions()
{
    save_action_ptr = new QAction( QIcon( ":/icons/icons/save.png" ), "Save", NO_PARENT );
    save_action_ptr->setShortcut( QKeySequence( QString( "Ctrl+N" ) ) );
    save_action_ptr->setStatusTip( "Add new guest entry" );
    save_action_ptr->setEnabled( false );
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
    if ( QPixmap const *pixmap = ui->upload_passport_label->pixmap() ){

    }
    Wt::Dbo::Transaction transaction{ *database_session };

    auto guest = std::make_unique<utilities::Guest>();
    guest->surname = ui->surname_edit->text().toStdString();
    guest->other_names = ui->other_names_edit->text().toStdString();
    guest->nationality = ui->nationality_edit->text().toStdString();
    guest->occupation = ui->occupation_edit->text().toStdString();
    guest->address = ui->address_tedit->toPlainText().toStdString();
    guest->phone_number = ui->phone_number_edit->text().toStdString();
    guest->means_of_id = ui->id_card_edit->text().toStdString();
    guest->nok_fullname = ui->nok_name_edit->text().toStdString();
    guest->nok_address = ui->nok_address_tedit->toPlainText().toStdString();
    guest->nok_phone_number = ui->nok_phone_edit->text().toStdString();

    try {
        database_session->add( std::move( guest ) );
        transaction.commit();
    } catch( dbo::Exception const & except ){
        QMessageBox::critical( this, "Error", "Unable to store guest's information, please try again later" );
        qInfo() << except.what();
    }
}

void NewGuestMainWindow::OnDeleteAddGuestTriggered()
{

}

template<typename T>
bool IsNull( T const & str ){ return str.isEmpty(); }

template<typename T, typename... U>
bool IsNull( T const & str, U &&...other_str )
{
    return IsNull( str ) || IsNull( std::forward<U>( other_str )... );
}

void NewGuestMainWindow::OnNextButtonClicked()
{
    bool const any_is_null = IsNull( ui->surname_edit->text(), ui->other_names_edit->text(),
                                     ui->nationality_edit->text(), ui->occupation_edit->text(),
                                     ui->address_tedit->toPlainText(), ui->phone_number_edit->text(),
                                     ui->vehicle_edit->text(), ui->id_card_edit->text(),
                                     ui->nok_name_edit->text(), ui->nok_address_tedit->toPlainText(),
                                     ui->nok_phone_edit->text() );
    if( any_is_null ){
        QMessageBox::critical( this, "Error", "One of the information fields is left unfilled, "
                                              "or invalid date is chosen. Please "
                                              "cross-check and try again.",
                               QMessageBox::Ok );
        return;
    } else {
        ui->passport_tab->setEnabled( true );
        ui->tabWidget->setCurrentIndex( 1 );
        save_action_ptr->setEnabled( true );
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

void NewGuestMainWindow::OnUseCameraClicked()
{
    CameraCapture * const camera_capture = new CameraCapture( this );
    QObject::connect( camera_capture, &CameraCapture::camera_failed,
                      [camera_capture]( QString const & message )
    {
        QMessageBox::critical( camera_capture, "Error", message );
        camera_capture->reject();
    });

    try {
        camera_capture->DisplayCamera();
    } catch( std::exception & exception ) {
        QMessageBox::critical( this, "Error", exception.what() );
        camera_capture->reject();
        return;
    }
    if( camera_capture->exec() == QDialog::Accepted )
    {

        ui->upload_passport_label->clear();
        ui->upload_passport_label->setPixmap( camera_capture->GetImage() );
    }
}

void NewGuestMainWindow::OnUploadPassportClicked()
{
    ui->upload_passport_button->setEnabled( false );
    QString filename = QFileDialog::getOpenFileName( this, "Passport", "",
                                                     "Pictures (*.jpg *.png *.tiff *.jpeg);; "
                                                     "All Files (*.*)" );
    if( filename.isEmpty() ){
        ui->upload_passport_label->setText( "No passport used" );
    } else {
        QPixmap const picture{ filename };
        QSize const max_size = picture.size();
        int preferred_height = max_size.height();
        int preferred_width = max_size.width();

        if( preferred_height > MAX_PASSPORT_HEIGHT ){
            preferred_height = MAX_PASSPORT_HEIGHT;
        }
        if( preferred_width > MAX_PASSPORT_WIDTH ){
            preferred_width = MAX_PASSPORT_WIDTH;
        }
        ui->upload_passport_label->clear();
        ui->upload_passport_label->setPixmap( picture.scaled( preferred_width, preferred_height ) );
    }
    ui->upload_passport_button->setEnabled( true );
}

NewGuestMainWindow::~NewGuestMainWindow()
{
    delete ui;
}

#undef NO_PARENT
