#ifndef LODGING_DIALOG_HPP
#define LODGING_DIALOG_HPP

#include <QDialog>
#include <memory>
#include <Wt/Dbo/Session.h>
#include "utilities.hpp"

namespace Ui {
class LodgingDialog;
}

namespace dbo = Wt::Dbo;

class LodgingDialog : public QDialog
{
    Q_OBJECT
    using UniqueSessionPtr = std::unique_ptr<dbo::Session>;
    using DboPtr = dbo::ptr<utilities::Guest>;
public:
    explicit LodgingDialog( DboPtr& guest_data, UniqueSessionPtr& db_session, QWidget *parent = nullptr );
    ~LodgingDialog();

private slots:
    void OnSaveButtonClicked();
private:
    Ui::LodgingDialog *ui;
    UniqueSessionPtr  &database_session;
    DboPtr            &guest_data;
};

#endif // LODGING_DIALOG_HPP
