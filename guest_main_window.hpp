#ifndef GUEST_MAIN_WINDOW_HPP
#define GUEST_MAIN_WINDOW_HPP

#include <QMainWindow>
#include <Wt/Dbo/Session.h>
#include <memory>

namespace Ui {
class NewGuestMainWindow;
}

class QAction;
class QCloseEvent;

class NewGuestMainWindow : public QMainWindow
{
    Q_OBJECT

private:
    void SetupActions();
    void SetupToolbar();
    void ClearAllData();
    QString GetPictureFilename( QString const default_filename = QString{} ) const;
public:
    explicit NewGuestMainWindow( std::unique_ptr<Wt::Dbo::Session> & db_session, QWidget *parent = 0);
    ~NewGuestMainWindow();
    void closeEvent( QCloseEvent *event ) override;
private slots:
    void OnNewAddGuestTriggered();
    void OnDeleteAddGuestTriggered();
    void OnCancelTriggered();
    void OnLogTriggered();
    void OnTakeNoteTriggered();

    void OnNextButtonClicked();
    void OnUploadPassportClicked();
    void OnUseCameraClicked();
private:
    Ui::NewGuestMainWindow *ui;

    QAction *save_action_ptr;
    QAction *delete_action_ptr;
    QAction *cancel_action_ptr;
    QAction *log_action_ptr;
    QAction *note_action_ptr;
    QAction *refresh_action_ptr;
    QAction *close_action_ptr;
    QString guest_picture_fullname;
    std::unique_ptr<Wt::Dbo::Session> &database_session;
};

#endif // GUEST_MAIN_WINDOW_HPP
