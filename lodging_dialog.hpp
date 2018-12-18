#ifndef LODGING_DIALOG_HPP
#define LODGING_DIALOG_HPP

#include <QDialog>

namespace Ui {
class LodgingDialog;
}

class LodgingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LodgingDialog(QWidget *parent = 0);
    ~LodgingDialog();

private:
    Ui::LodgingDialog *ui;
};

#endif // LODGING_DIALOG_HPP
