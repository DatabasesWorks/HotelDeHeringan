#include "reservation_dialog.hpp"
#include "ui_reservation_dialog.h"

ReservationDialog::ReservationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReservationDialog)
{
    ui->setupUi(this);

    setWindowTitle( "Reservations" );
}

ReservationDialog::~ReservationDialog()
{
    delete ui;
}
