#ifndef ROLE_DEFINITION_DIALOG_HPP
#define ROLE_DEFINITION_DIALOG_HPP

#include <QDialog>

namespace Ui {
class RoleDefinitionDialog;
}

namespace Wt
{
    namespace Dbo {
        class Session;
    }
}

class RoleDefinitionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RoleDefinitionDialog(Wt::Dbo::Session & db_session, QWidget *parent = 0);
    ~RoleDefinitionDialog();

private:
    Ui::RoleDefinitionDialog *ui;
    Wt::Dbo::Session  &database_session;
};

#endif // ROLE_DEFINITION_DIALOG_HPP
