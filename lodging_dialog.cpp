#include "lodging_dialog.hpp"
#include "ui_lodging_dialog.h"

#include <QInputDialog>
#include <QMessageBox>
#include <memory>

LodgingDialog::LodgingDialog( DboPtr &guest_data, UniqueSessionPtr &db_session, QWidget *parent ):
    QDialog( parent ), ui{ new Ui::LodgingDialog }, database_session{ db_session },
    guest_data{ guest_data }
{
    ui->setupUi( this );
    ui->lodge_surname_edit->setEnabled( false );
    ui->lodge_other_names->setEnabled( false );
    ui->lodge_reference_number->setEnabled( false );
    ui->lodge_surname_edit->setText( QString::fromStdString( guest_data->surname ) );
    ui->lodge_other_names->setText( QString::fromStdString( guest_data->other_names ) );
    ui->lodge_reference_number->setText( tr( "%1" ).arg( guest_data->guest_id ));

    for( unsigned int i = 1; i <= 10; ++i ){
        ui->adult_combo->addItem( tr( "%1" ).arg( i ) );
    }
    ui->children_combo->addItems( QStringList{ "0", "1", "2", "3" } );
    ui->visit_combo->addItems( QStringList{ "Business", "Pleasure", "Others..." } );
    ui->arrival_dt->setDateTime( QDateTime::currentDateTime() );
    ui->departure_dt->setDateTime( QDateTime::currentDateTime().addDays( 1 ) ); // the next day.
    QObject::connect( ui->lodge_save_button, SIGNAL(clicked(bool)), this, SLOT(OnSaveButtonClicked()) );
    QObject::connect( ui->visit_combo,
                      static_cast<void(QComboBox::*)(int)>( &QComboBox::currentIndexChanged ),
                      [this]() mutable {
        if( ui->visit_combo->currentText() == "Others..." ){
            QString const input = QInputDialog::getText( this, "Nature of visit", "Please specify" );
            if( !input.trimmed().isEmpty() ){
                ui->visit_combo->insertItem( 0, input );
            }
        }
    });
}

void LodgingDialog::OnSaveButtonClicked()
{
    using utilities::IsEmpty;
    bool const has_unfilled_fields = IsEmpty( ui->lodge_amount_edit->text(),
                                        ui->lodge_room_edit->text(),
                                        ui->arrival_from_edit->text() );
    if( has_unfilled_fields ){
        QMessageBox::information( this, "Error", "Please make sure you fill all important fields" );
        return;
    }
    bool valid_amount_entered = false;
    (void) ui->lodge_amount_edit->text().toDouble( &valid_amount_entered );
    if( !valid_amount_entered ){
        QMessageBox::information( this, "Error", "Invalid amount specified" );
        ui->lodge_amount_edit->setFocus();
        return;
    }
    bool valid_nights_entered = false;
    long number_of_nights = 0;
    number_of_nights = ui->lodge_nights_edit->text().toLong( &valid_nights_entered );
    if( !valid_nights_entered ){
        QMessageBox::information( this, "Error", "Invalid number of nights" );
        ui->lodge_nights_edit->setFocus();
        return;
    }
    if( ui->departure_dt->dateTime() < ui->arrival_dt->dateTime() ){
        QMessageBox::information( this, "Error", "Invalid arrival or departure dates specified" );
        ui->departure_dt->setFocus();
        return;
    }
    qint64 const days_diff = ui->arrival_dt->dateTime().daysTo( ui->departure_dt->dateTime() );
    if( days_diff != number_of_nights ){
        QMessageBox::information( this, "Error", "The difference between the "
                                                 "departure and arrival time do not match"
                                                 " the number of nights specified." );
        ui->lodge_nights_edit->setFocus();
        ui->lodge_nights_edit->setText( tr( "%1" ).arg( days_diff ) );
        return;
    }
    using utilities::Lodging;
    using utilities::QtDateTimeToWtDateTime;

    auto lodging_info{ std::make_unique<Lodging>() };
    lodging_info->room_number = ui->lodge_room_edit->text().toStdString();
    lodging_info->amount_paid = ui->lodge_amount_edit->text().toStdString();
    lodging_info->account_settler = ui->lodge_settler_edit->text().toStdString();
    lodging_info->arrived_from = ui->arrival_from_edit->text().toStdString();
    lodging_info->departing_to = ui->departure_to_edit->text().toStdString();
    lodging_info->vehicle_license_plate = ui->lodge_vehicle_edit->text().toStdString();
    lodging_info->nature_of_visit = ui->visit_combo->currentText().toStdString();
    lodging_info->date_of_arrival = QtDateTimeToWtDateTime( ui->arrival_dt->dateTime() );
    lodging_info->date_of_departure = QtDateTimeToWtDateTime( ui->departure_dt->dateTime() );
    lodging_info->number_of_nights = number_of_nights;
    lodging_info->number_of_adults = ui->adult_combo->currentText().toLong();
    lodging_info->number_of_children = ui->children_combo->currentText().toLong();

    dbo::Transaction transaction{ *database_session };
    (void) transaction;
    try {
        guest_data.modify()->lodgings.insert( std::move( lodging_info ) );
        guest_data.flush();
        QMessageBox::information( this, "Success", "Data saved successfully" );
        this->accept();
    } catch( dbo::Exception & except ) {
        QMessageBox::critical( this, "Error", "" );
        qInfo() << except.what();
    }
}

LodgingDialog::~LodgingDialog()
{
    delete ui;
}
