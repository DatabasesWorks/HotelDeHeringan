#ifndef RESERVATION_DIALOG_HPP
#define RESERVATION_DIALOG_HPP

#include <QDialog>

namespace Ui {
class ReservationDialog;
}

class ReservationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReservationDialog(QWidget *parent = 0);
    ~ReservationDialog();

private:
    Ui::ReservationDialog *ui;
};

#endif // RESERVATION_DIALOG_HPP
