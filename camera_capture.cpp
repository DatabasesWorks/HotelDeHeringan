#include "camera_capture.hpp"
#include <QWidget>
#include <QCameraViewfinder>
#include <QCamera>
#include <QCloseEvent>
#include <QCameraImageCapture>
#include <QCameraInfo>
#include <QList>
#include <exception>
#include <QImage>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>

CameraCapture::CameraCapture( QWidget *parent): QDialog( parent ),
    view_finder{ new QCameraViewfinder() }, camera_to_use{ nullptr },
    capture_button{ new QPushButton( "Capture", this ) }
{
    view_finder->setMinimumSize( 640, 640 );

    QGridLayout *grid_layout { new QGridLayout( this ) };
    grid_layout->setProperty( "styleSheet", QString{ "QPushButton { border: 2px solid; border-radius: 6px; "
                                                     "padding: 6px; }" } );
    grid_layout->addWidget( capture_button, 0, 0, 1, 3 );
    grid_layout->addWidget( view_finder, 1, 0, 10, 3 );

    QObject::connect( capture_button, &QPushButton::clicked, [this]( bool ) mutable {
        image_capture->capture();
    });

    this->setLayout( grid_layout );
}

void CameraCapture::UnlockCamera()
{
    if( camera_to_use ){
        camera_to_use->unlock();
        camera_to_use->unload();
        camera_to_use->stop();
    }
    if( this->image_capture ){
        image_capture->cancelCapture();
    }
}

CameraCapture::~CameraCapture()
{
    UnlockCamera();
}

QPixmap CameraCapture::GetImage() const
{
    return captured_image;
}

void CameraCapture::DisplayCamera() noexcept( false )
{
    QList<QCameraInfo> const available_cameras = QCameraInfo::availableCameras();
    int const all_cameras = available_cameras.size();

    if( all_cameras < 1 ){
        throw std::exception( "No cameras were found, please try again later" );
    }
    for( auto const &current_camera_info: available_cameras ){
        if( !current_camera_info.isNull() ){
            this->camera_to_use = new QCamera( current_camera_info, this );
            break;
        }
    }
    if( !camera_to_use ){
        throw std::exception( "Unable to use any of the available cameras" );
    }

    camera_to_use->setViewfinder( view_finder );
    view_finder->show();

    QObject::connect( camera_to_use, &QCamera::lockFailed, [this]{
        emit this->camera_failed( "Unable to place a lock on the WebCam" );
    });
    QObject::connect( camera_to_use, static_cast<void(QCamera::*)( bool )>( &QCamera::availabilityChanged ),
                      [this]( bool available )->void
    {
        if( !available ) emit this->camera_failed( "Camera detached or not available" );
    });
    QObject::connect( camera_to_use, static_cast<void(QCamera::*)(QCamera::Error)>(&QCamera::error),
                      [this]( QCamera::Error error ) -> void
    {
        if( error != QCamera::NoError ) emit this->camera_failed( "An error occured while using the camera" );
    });

    image_capture = new QCameraImageCapture( camera_to_use );
    camera_to_use->setCaptureMode( QCamera::CaptureStillImage );
    camera_to_use->start();
    camera_to_use->searchAndLock();
    QObject::connect( image_capture, SIGNAL(imageCaptured(int,QImage)), this,
                      SLOT(OnImageCaptured(int,QImage)) );

}

void CameraCapture::OnImageCaptured( int, QImage const & image_preview )
{
    QDialog *preview_dialog = new QDialog( this );
    preview_dialog->setWindowTitle( "Preview" );
    QVBoxLayout *layout = new QVBoxLayout();
    QPushButton *use_captured_image_button = new QPushButton( "Use image" );
    QPushButton *take_another_image_button = new QPushButton( "Take another" );
    QLabel      *preview = new QLabel();

    preview->clear();
    preview->setPixmap( QPixmap::fromImage( image_preview ));
    QGridLayout *button_grid = new QGridLayout();
    button_grid->addWidget( use_captured_image_button, 0, 0, Qt::AlignLeft );
    button_grid->addWidget( take_another_image_button, 0, 1, Qt::AlignRight );

    layout->addWidget( preview );
    layout->addLayout( button_grid );
    preview_dialog->setLayout( layout );

    QObject::connect( use_captured_image_button, &QPushButton::clicked, [=]( bool ){
        preview_dialog->accept();
        this->UnlockCamera();
        this->captured_image = QPixmap::fromImage( image_preview );
        this->accept();
    });
    QObject::connect( take_another_image_button, &QPushButton::clicked, [=]( bool ){
        preview_dialog->accept();
    });
    preview_dialog->exec();
}

void CameraCapture::closeEvent(QCloseEvent * event)
{
    UnlockCamera();
    event->accept();
}
