#include <QApplication>
#include <QPixmap>
#include <QSplashScreen>
#include <QColor>

#include "splash_screen.hpp"
#include "mainwindow.hpp"
#include "utilities.hpp"

SplashScreen::SplashScreen( QApplication &main_app, QObject *parent) : QObject(parent),
    main_application( main_app ), splash_screen( nullptr )
{
    QPixmap const pixel_map{ ":/icons/icons/splash_screen.png" };
    splash_screen = new QSplashScreen( pixel_map.scaled( 500, 300 ) );
}

void SplashScreen::ShowSplashScreen()
{
    auto const align_bottom_center = Qt::AlignBottom | Qt::AlignCenter;
    auto const color_orange = QColor( 255, 165, 0 );

    splash_screen->show();
    splash_screen->showMessage( "Initializing application", align_bottom_center, color_orange );
    main_application.processEvents();

    splash_screen->showMessage( "Checking database access", align_bottom_center, color_orange );
    auto db_session = utilities::CheckDatabaseAccess();
    main_application.processEvents();
    splash_screen->showMessage( "Finalizing initialization", align_bottom_center, color_orange );

    MainWindow *main_window { new MainWindow( std::move( db_session ), nullptr ) };
    splash_screen->finish( main_window );
    main_window->show();
}
