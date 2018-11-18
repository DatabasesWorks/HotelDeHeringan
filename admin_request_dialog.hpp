#ifndef ADMIN_REQUEST_DIALOG_HPP
#define ADMIN_REQUEST_DIALOG_HPP

#include <QDialog>
#include <string>

namespace Ui {
class AdminRequestDialog;
}

class AdminRequestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdminRequestDialog(QWidget *parent = 0);
    ~AdminRequestDialog();
    std::string GetUsername();
    std::string GetPassword();
    std::string GetConfirmPassword();
    std::string GetHost();
    std::string GetDatabaseName();
    long        GetPortNumber();

    void ClearPasswordFields();

private:
    Ui::AdminRequestDialog *ui;
};

#endif // ADMIN_REQUEST_DIALOG_HPP
