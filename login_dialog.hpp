#ifndef LOGIN_DIALOG_HPP
#define LOGIN_DIALOG_HPP

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog( QWidget *parent = 0 );
    ~LoginDialog();

    QString GetUsername();
    QString GetPassword();
private:
    Ui::LoginDialog *ui;
};

#endif // LOGIN_DIALOG_HPP
