#ifndef SPLASH_SCREEN_HPP
#define SPLASH_SCREEN_HPP

#include <QObject>

class QSplashScreen;
class QApplication;

class SplashScreen : public QObject
{
    Q_OBJECT
public:
    explicit SplashScreen( QApplication &main_app, QObject *parent = 0 );
    void ShowSplashScreen();
signals:

public slots:

private:
    QApplication &main_application;
    QSplashScreen *splash_screen;
};

#endif // SPLASH_SCREEN_HPP
