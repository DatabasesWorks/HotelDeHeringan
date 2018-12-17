#ifndef CAMERACAPTURE_HPP
#define CAMERACAPTURE_HPP

#include <QDialog>
#include <QPixmap>

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
    CameraCapture( QWidget *parent = nullptr );
    ~CameraCapture();
    void DisplayCamera();
    QPixmap GetImage() const;

protected:
    void closeEvent(QCloseEvent *) override;
signals:
    void camera_failed( QString );
private:
    QCameraViewfinder   *view_finder;
    QCameraImageCapture *image_capture;
    QCamera             *camera_to_use;
    QPushButton         *capture_button;
    QPushButton         *capture_another_button;
    QPushButton         *use_captured_button;
    QPixmap             captured_image;

    void UnlockCamera();
};

#endif // CAMERACAPTURE_HPP
