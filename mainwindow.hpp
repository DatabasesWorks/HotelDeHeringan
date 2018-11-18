#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QAction>
#include <QMdiArea>
#include <QCloseEvent>
#include <Wt/Dbo/Session.h>
#include <memory>

#define NO_PARENT nullptr

struct LoggedInSession
{
    std::string name {};
    std::string username {};
    int user_permission { -1 };
    bool is_logged_in { false };
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow( std::unique_ptr<Wt::Dbo::Session> session, QWidget *parent = nullptr );
    ~MainWindow();

    void closeEvent( QCloseEvent * ) override;
    void show();
private:
    void SetActions();
    void SetMenus();
    void SetToolbar();

private slots:
    void PromptLogin();
    void AddGuestInformation();
    void DefineRoles();
    void AddCheckReservation();
    void ReportClicked();
    void ExpensesClicked();
    void ToggleSessionActivation( bool activate );
signals:
    void windowShown();
private:
    QMdiArea       *ptr_workspace;

    QAction        *ptr_login_action;
    QAction        *ptr_logout_action;
    QAction        *ptr_guest_registration_action;
    QAction        *ptr_define_role_action;
    QAction        *ptr_reservation_action;
    QAction        *ptr_reports_action;
    QAction        *ptr_expenses_action;

    std::unique_ptr<Wt::Dbo::Session> database_session;
    LoggedInSession  login_session;
};

#endif // MAINWINDOW_HPP
