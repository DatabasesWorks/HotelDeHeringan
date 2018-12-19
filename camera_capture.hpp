#ifndef CAMERACAPTURE_HPP
#define CAMERACAPTURE_HPP

#include <QDialog>
#include <QPixmap>
#include <QImage>

class QWidget;
class QCameraViewfinder;
class QCamera;
class QPushButton;
class QCameraImageCapture;
class QCloseEvent;

class CameraCapture: public QDialog
{
    Q_OBJECT
public:
    CameraCapture( QString const filename, QWidget *parent = nullptr );
    ~CameraCapture();
    void DisplayCamera();
    QPixmap GetImage() const;

protected slots:
    void closeEvent(QCloseEvent *) override;
    void OnImageCaptured( int, QImage const &image_preview );
signals:
    void camera_failed( QString );
private:
    QCameraViewfinder   *view_finder;
    QCameraImageCapture *image_capture;
    QCamera             *camera_to_use;
    QPushButton         *capture_button;
    QPixmap             captured_image;

    void UnlockCamera();
};

#endif // CAMERACAPTURE_HPP
