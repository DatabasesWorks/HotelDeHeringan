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
#include <QDateTime>
#include <filesystem>
#include <Wt/Date/date.h>

#include "camera_capture.hpp"
#include "utilities.hpp"
#include "lodging_dialog.hpp"
#include "billing_dialog.hpp"

#define NO_PARENT nullptr
#define MAX_PASSPORT_WIDTH 340
#define MAX_PASSPORT_HEIGHT 340

namespace dbo = Wt::Dbo;

enum TabWidgetWindow
{
    AddGuestInformationTab = 0,
    UploadPassportTab = 1,
    SearchWindowTab = 2,
    AddBillTab = 3
};

enum SearchCriteriaParam
{
    BySurname,
    ByOtherNames,
    ByPhoneNumber,
    ByRoomNumber,
    ByNationality,
    ByMotorLicense
};

NewGuestMainWindow::NewGuestMainWindow( std::unique_ptr<Wt::Dbo::Session> &db_session, QWidget *parent ) :
    QMainWindow( parent ), ui( new Ui::NewGuestMainWindow ), database_session{ db_session }
{
    ui->setupUi(this);
    this->setMinimumSize( 800, 520 );
    this->setMaximumSize( 900, 520 );

    setWindowTitle( "Guest's Registration" );
    ui->tabWidget->setCurrentIndex( 0 );

    QStringList const search_by_keywords{ "Search by surname", "Search other names",
                                          "Search by phone number", "Search by room number",
                                          "Search by nationality", "Search motor license" };
    ui->search_criteria_combo->addItems( search_by_keywords );
    ui->search_date_from->setDate( QDate::currentDate().addDays( -7 ) ); // from a week ago
    ui->search_date_to->setDate( QDate::currentDate() );

    SetupActions();
    SetupToolbar();
    SetEnableSearchActions( false );

    ui->passport_tab->setEnabled( false );
    ui->search_result_area->setEnabled( false );

    ui->search_date_from->setEnabled( false );
    ui->search_date_to->setEnabled( false );

    QObject::connect( ui->save_button, SIGNAL(clicked(bool)), this, SLOT( OnNewAddGuestTriggered() ) );
    QObject::connect( ui->next_button, SIGNAL(clicked(bool)), this, SLOT( OnNextButtonClicked()) );
    QObject::connect( ui->upload_passport_button, SIGNAL(clicked(bool)), this, SLOT( OnUploadPassportClicked()) );
    QObject::connect( ui->use_camera_button, SIGNAL(clicked(bool)), this, SLOT( OnUseCameraClicked()));
    QObject::connect( ui->find_guest_button, SIGNAL(clicked(bool)), this, SLOT( OnSearchGuestTriggered()) );
    QObject::connect( ui->search_limit_checkbox, &QCheckBox::toggled, [this]( bool const is_checked ){
        ui->search_date_from->setEnabled( is_checked );
        ui->search_date_to->setEnabled( is_checked );
    });
    QObject::connect( ui->search_previous_button, &QPushButton::clicked, [this]{
        --this->searched_guest_index;
        UpdateSearchRecord();
    });
    QObject::connect( ui->search_next_button, &QPushButton::clicked, [this]{
        ++this->searched_guest_index;
        UpdateSearchRecord();
    });
}

void NewGuestMainWindow::SetEnableSearchActions( bool const flag )
{
    search_action->setEnabled( flag );
    lodge_action->setEnabled( flag );
    add_bill_action->setEnabled( flag );
    print_bills_action->setEnabled( flag );
    generate_report_action->setEnabled( flag );
}

void NewGuestMainWindow::SetupActions()
{
    lodge_action= new QAction( QIcon( ":/icons/icons/save.png" ), "New lodge", NO_PARENT );
    lodge_action->setShortcut( QKeySequence( QString( "Ctrl+N" ) ) );
    lodge_action->setStatusTip( "Add lodging information for a guest" );
    QObject::connect( lodge_action, SIGNAL(triggered(bool)), this, SLOT( OnLodgeGuestTriggered() ) );

    add_bill_action= new QAction( QIcon( ":/icons/icons/delete.png"),"Add Billing", NO_PARENT );
    add_bill_action->setShortcut( QKeySequence( QString( "Ctrl+D" )));
    add_bill_action->setStatusTip( "Delete guest information from database" );
    QObject::connect( add_bill_action, SIGNAL( triggered(bool)), this, SLOT( OnAddBillTriggered() ) );

    print_bills_action= new QAction( QIcon( ":/icons/icons/cancel.png"),"Print Guest Bills", NO_PARENT );
    print_bills_action->setShortcut( QKeySequence( QString( "Ctrl+L" ) ) );
    print_bills_action->setStatusTip( "Cancel current operation and close window" );
    QObject::connect( print_bills_action, SIGNAL(triggered(bool)), this, SLOT(OnPrintBillsTriggered()) );

    generate_report_action = new QAction( QIcon( ":/icons/icons/log.png"),"Generate Reports", NO_PARENT );
    generate_report_action->setShortcut( QKeySequence( QString("Ctrl+O" ) ) );
    generate_report_action->setStatusTip( "Log current status" );
    QObject::connect( generate_report_action, SIGNAL(triggered(bool)), this, SLOT(OnGenerateReportTriggered()));

    search_action = new QAction( QIcon( ":/icons/icons/note.png"), "Search", NO_PARENT );
    search_action->setShortcut( QKeySequence( QString( "Search" ) ) );
    search_action->setStatusTip( "Add extra note to the current guest basic information" );
    QObject::connect( search_action, &QAction::triggered, [this]{
        ui->tabWidget->setCurrentIndex( TabWidgetWindow::SearchWindowTab );
        ui->tabWidget->currentWidget()->setEnabled( true );
    });

    refresh_action = new QAction( QIcon( ":/icons/icons/refresh.png"), "Refresh", NO_PARENT );
    refresh_action->setShortcut( QKeySequence( QString( "Ctrl+R" )));
    refresh_action->setStatusTip( "Refresh when hung on an operation" );

    close_action = new QAction( QIcon( ":/icons/icons/close.png"), "Close", NO_PARENT );
    close_action->setShortcut( QKeySequence( QString( "Ctrl+Q" )));
    close_action->setStatusTip( "Close Window" );
    QObject::connect( close_action, SIGNAL(triggered(bool)), this, SLOT(close()) );
}

void NewGuestMainWindow::closeEvent( QCloseEvent * )
{

}

void NewGuestMainWindow::SetupToolbar()
{
    QToolBar* toolbar{ new QToolBar( "SubMenu" ) };
    toolbar->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
    this->addToolBar( Qt::LeftToolBarArea, toolbar );

    toolbar->addAction( search_action );
    toolbar->addSeparator();
    toolbar->addAction( generate_report_action );
    toolbar->addSeparator();
    toolbar->addAction( lodge_action );
    toolbar->addSeparator();
    toolbar->addAction( add_bill_action );
    toolbar->addSeparator();
    toolbar->addAction( search_action );
    toolbar->addSeparator();
    toolbar->addAction( refresh_action );
    toolbar->addSeparator();
    toolbar->addAction( close_action );
}

void NewGuestMainWindow::OnNewAddGuestTriggered()
{
    auto guest{ std::make_unique<utilities::Guest>() };
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
    guest->vehicle_license_plate = ui->vehicle_edit->text().toStdString();

    if ( ui->upload_passport_label->pixmap() ){
        guest->picture_path = guest_picture_fullname.toStdString();
    }

    Wt::Dbo::Transaction transaction{ *database_session };
    try {
        database_session->add( std::move( guest ) );
        transaction.commit();
        ClearAllData();
        QMessageBox::information( this, "Success", "Data saved succesfully" );
    } catch( dbo::Exception const & except ){
        QMessageBox::critical( this, "Error", "Unable to store guest's information, please try again later" );
        qInfo() << except.what();
    }
}

void NewGuestMainWindow::ClearAllData()
{
    guest_picture_fullname.clear();
    ui->new_guest_tab->setEnabled( true );
    ui->passport_tab->setDisabled( true );
    ui->tabWidget->setCurrentIndex( TabWidgetWindow::AddGuestInformationTab );
    ui->surname_edit->clear();
    ui->other_names_edit->clear();
    ui->nationality_edit->clear();
    ui->occupation_edit->clear();
    ui->address_tedit->clear();
    ui->phone_number_edit->clear();
    ui->vehicle_edit->clear();
    ui->id_card_edit->clear();
    ui->nok_name_edit->clear();
    ui->nok_address_tedit->clear();
    ui->nok_phone_edit->clear();
    ui->upload_passport_label->clear();

    ui->surname_edit->setFocus();
}

void NewGuestMainWindow::OnLodgeGuestTriggered()
{
    LodgingDialog{ searched_guests[searched_guest_index], database_session, this }.exec();
}

void NewGuestMainWindow::OnGenerateReportTriggered()
{

}

void NewGuestMainWindow::OnNextButtonClicked()
{
    using utilities::IsEmpty;
    bool const any_is_null { IsEmpty( ui->surname_edit->text(), ui->other_names_edit->text(),
                                      ui->nationality_edit->text(), ui->occupation_edit->text(),
                                      ui->address_tedit->toPlainText(), ui->phone_number_edit->text(),
                                      ui->vehicle_edit->text(), ui->id_card_edit->text(),
                                      ui->nok_name_edit->text(), ui->nok_address_tedit->toPlainText(),
                                      ui->nok_phone_edit->text() ) };
    if( any_is_null ){
        QMessageBox::critical( this, "Error", "One of the information fields is left unfilled, "
                                              "or invalid date is chosen. Please "
                                              "cross-check and try again.",
                               QMessageBox::Ok );
        return;
    } else {
        ui->passport_tab->setEnabled( true );
        ui->tabWidget->setCurrentIndex( TabWidgetWindow::UploadPassportTab );
        ui->new_guest_tab->setDisabled( true );
    }
}

void NewGuestMainWindow::OnAddBillTriggered()
{
    BillingDialog{ database_session, this }.exec();
}

void NewGuestMainWindow::OnPrintBillsTriggered()
{

}

std::string NewGuestMainWindow::GetWhereClause( int const search_index )
{
    switch( search_index ){
    case SearchCriteriaParam::BySurname:
        return "surname = ?";
    case SearchCriteriaParam::ByMotorLicense:
        return "license_plate = ?";
    case SearchCriteriaParam::ByPhoneNumber:
        return "phone_number = ?";
    case SearchCriteriaParam::ByNationality:
        return "nationality = ?";
    case SearchCriteriaParam::ByOtherNames:
        return "other_names = ?";
    case SearchCriteriaParam::ByRoomNumber: default:
        return "l.room_number = ?";
    }
}

void NewGuestMainWindow::OnSearchGuestTriggered()
{
    this->searched_guests.clear();
    this->searched_guest_index = 0;
    SetEnableSearchActions( false );

    ui->search_result_area->setEnabled( false );
    std::string const search_text{ ui->search_string_edit->text().trimmed().toStdString() };
    int const criteria_parameter{ ui->search_criteria_combo->currentIndex() };
    std::string const where_clause { this->GetWhereClause( criteria_parameter ) };
    bool const searching_by_date_range{ ui->search_limit_checkbox->isChecked() };

    dbo::Transaction transaction{ *database_session };
    using utilities::Guest;

    dbo::collection<dbo::ptr<Guest>> guests {};
    if( !searching_by_date_range ){
        // room number is independent of users, so we need to associate previous lodgers with rooms
        if( criteria_parameter == SearchCriteriaParam::ByRoomNumber ){
            guests = database_session->
                    query<dbo::ptr<Guest>>( "SELECT g FROM guests g INNER JOIN lodgings l" )
                    .where( where_clause + " AND g.guest_id = l.guest_id" )
                    .bind( search_text ).resultList();
        } else {
            guests = database_session->find<Guest>().where( where_clause ).bind( search_text );
        }
    } else { // with date range, it gets a little interesting
        using utilities::QtDateTimeToWtDateTime;
        auto const from{ QtDateTimeToWtDateTime( ui->search_date_from->dateTime() ) };
        auto const to{ QtDateTimeToWtDateTime( ui->search_date_to->dateTime() ) };

        guests = database_session->query<dbo::ptr<Guest>>( "SELECT g FROM guests g INNER JOIN lodgings l" )
                                      .where( where_clause ).bind( search_text )
                                      .where( "g.guest_id = l.guest_id" )
                                      .where( "(l.arrival_date BETWEEN ? AND ? )" )
                                      .bind( from ).bind( to ).resultList();
    }
    if( guests.size() == 0 ){
        QMessageBox::information( this, "Search", "No result found" );
        return;
    }
    SetEnableSearchActions( true );
    this->searched_guests = guests_list{ guests.begin(), guests.end() };
    ui->search_result_area->setEnabled( true );
    ui->search_previous_button->setEnabled( false );
    this->UpdateSearchRecord();
}

void NewGuestMainWindow::UpdateSearchRecord()
{
    ui->search_previous_button->setEnabled( true );
    ui->search_next_button->setEnabled( true );

    int const result_size = this->searched_guests.size();
    int& index = this->searched_guest_index;

    if( result_size == 0 || result_size == 1 ){
        ui->search_previous_button->setEnabled( false );
        ui->search_next_button->setEnabled( false );
        if( result_size == 0 ) return; // most likely never gonna happen but just to be on the safer side.
    }
    if( this->searched_guest_index == 0 ){
        ui->search_previous_button->setEnabled( false );
    } else if( this->searched_guest_index >= searched_guests.size() - 1 ){
        ui->search_next_button->setEnabled( false );
    }

    dbo::ptr<utilities::Guest> & guest = this->searched_guests[ index ];

    ui->search_surname_edit->setText( QString::fromStdString( guest->surname ) );
    ui->search_othername_edit->setText( QString::fromStdString( guest->other_names ) );
    ui->search_address_edit->setText( QString::fromStdString( guest->address ) );
    ui->search_nationality_edit->setText( QString::fromStdString( guest->nationality ));
    ui->search_license_edit->setText( QString::fromStdString( guest->vehicle_license_plate ) );
    ui->search_occupation_edit->setText( QString::fromStdString( guest->occupation ) );

    QSize const pic_size{ 160, 160 };
    if( !guest->picture_path.empty() ){
        QPixmap const passport{ QString::fromStdString( guest->picture_path ) };
        ui->search_passport_preview->setPixmap( passport.scaled( pic_size ) );
    } else {
        ui->search_passport_preview->setPixmap( QPixmap( ":/icons/icons/default_dp.jpg" ).scaled( pic_size ));
    }
}

QString NewGuestMainWindow::GetPictureFilename( QString const default_file_path ) const
{
    QString file_name { default_file_path };
    if( file_name.isEmpty() ){
        file_name = ui->surname_edit->text().toLower() + '_' + ui->other_names_edit->text().toLower();
        file_name.replace( ' ', '_' );
    }
    QDir const default_picture_path { QDir::currentPath() + tr( "/snapshots/" ) };
    if( !default_picture_path.exists() ) {
        default_picture_path.mkpath( default_picture_path.path() );
    }
    return default_picture_path.path() + '/' + file_name;
}

void NewGuestMainWindow::OnUseCameraClicked()
{
    QString const picture_location{ GetPictureFilename() };
    CameraCapture * const camera_capture{ new CameraCapture( picture_location, this ) };
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
        guest_picture_fullname = picture_location;
    }
}

void NewGuestMainWindow::OnUploadPassportClicked()
{
    ui->upload_passport_button->setEnabled( false );
    QString filename{ QFileDialog::getOpenFileName( this, "Passport", "",
                                                    "Pictures (*.jpg *.png *.tiff *.jpeg);; "
                                                    "All Files (*.*)" ) };
    if( filename.isEmpty() ){
        ui->upload_passport_label->setText( "No passport used" );
    } else {
        QPixmap const picture{ filename };
        QSize const max_size{ picture.size() };
        int preferred_height{ max_size.height() };
        int preferred_width{ max_size.width() };

        if( preferred_height > MAX_PASSPORT_HEIGHT ){
            preferred_height = MAX_PASSPORT_HEIGHT;
        }
        if( preferred_width > MAX_PASSPORT_WIDTH ){
            preferred_width = MAX_PASSPORT_WIDTH;
        }
        ui->upload_passport_label->clear();
        if( !guest_picture_fullname.isEmpty() ){
            auto response{ QMessageBox::information( this, "Delete", "It seems a picture was taken before this, "
                                                     "would you like to delete it from library?",
                                                     QMessageBox::Yes | QMessageBox::No ) };
            if( response == QMessageBox::Yes ){
                QFile previous_image{ guest_picture_fullname };
                if( previous_image.remove() ){
                    QMessageBox::information( this, "Delete", "Picture removed successfully" );
                } else {
                    QMessageBox::critical( this, "Delete", "Unable to remove picture " );
                }
            }
        }
        ui->upload_passport_label->setPixmap( picture.scaled( preferred_width, preferred_height ) );
        guest_picture_fullname = GetPictureFilename( QFileInfo{ filename }.fileName() );
        try {
            std::experimental::filesystem::path const from_path{ filename.toStdString() };
            std::experimental::filesystem::path const to_path{ guest_picture_fullname.toStdString() };
            std::experimental::filesystem::copy_file( from_path, to_path );

        } catch( std::experimental::filesystem::filesystem_error const & error ) {
            QMessageBox::critical( this, "Error", error.what() );
            return;
        }
    }
    ui->upload_passport_button->setEnabled( true );
}

NewGuestMainWindow::~NewGuestMainWindow()
{
    delete ui;
}

#undef NO_PARENT
