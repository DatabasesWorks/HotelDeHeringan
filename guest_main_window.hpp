#ifndef GUEST_MAIN_WINDOW_HPP
#define GUEST_MAIN_WINDOW_HPP

#include <QMainWindow>
#include <Wt/Dbo/Session.h>
#include <QDateTime>
#include <memory>
#include "utilities.hpp"

namespace Ui {
class NewGuestMainWindow;
}

class QAction;
class QCloseEvent;
namespace dbo = Wt::Dbo;

class NewGuestMainWindow : public QMainWindow
{
    Q_OBJECT

private:
    void            SetupActions();
    void            SetupToolbar();
    void            ClearAllData();
    void            UpdateSearchRecord();
    std::string     GetWhereClause( int const );
    QString         GetPictureFilename( QString const default_filename = QString{} ) const;
    void            SetEnableSearchActions( bool const enabled );
public:
    explicit NewGuestMainWindow( std::unique_ptr<Wt::Dbo::Session> & db_session, QWidget *parent = 0);
    ~NewGuestMainWindow();
    using guests_list = std::vector<dbo::ptr<utilities::Guest>>;
    void closeEvent( QCloseEvent *event ) override;
private slots:
    void OnNewAddGuestTriggered();
    void OnLodgeGuestTriggered();
    void OnAddBillTriggered();
    void OnPrintBillsTriggered();
    void OnSearchGuestTriggered();
    void OnGenerateReportTriggered();

    void OnNextButtonClicked();
    void OnUploadPassportClicked();
    void OnUseCameraClicked();
private:
    Ui::NewGuestMainWindow *ui;
    std::unique_ptr<dbo::Session> &database_session;

    QAction *lodge_action;
    QAction *add_bill_action;
    QAction *print_bills_action;
    QAction *generate_report_action;
    QAction *search_action;
    QAction *refresh_action;
    QAction *close_action;

    int      searched_guest_index {};
    QString  guest_picture_fullname {};
    guests_list searched_guests {};
};

#endif // GUEST_MAIN_WINDOW_HPP
