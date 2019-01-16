#ifndef BILLING_DIALOG_HPP
#define BILLING_DIALOG_HPP

#include <QDialog>

namespace Ui {
class BillingDialog;
}

class BillingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BillingDialog(QWidget *parent = 0);
    ~BillingDialog();

private:
    Ui::BillingDialog *ui;
};

#endif // BILLING_DIALOG_HPP
