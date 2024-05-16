#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <QObject>
#include <QtSensors/QAccelerometer>
#include <QTimer>
#include <QVector>

#define calibrationDuration 1000 // 1sec
#define sampling_interval 100 // 100ms = 0.1sec

class KalmanFilter
{
public:
    KalmanFilter(double processNoise, double measurementNoise, double estimationError, double initialValue)
        : q(processNoise), r(measurementNoise), p(estimationError), x(initialValue) {}

    double update(double measurement)
    {
        // Prediction update
        p = p + q;

        // Measurement update
        double k = p / (p + r); // Kalman gain
        x = x + k * (measurement - x); // Update estimate
        p = (1 - k) * p; // Update error covariance

        return x;
    }

private:
    double q; // Process noise covariance
    double r; // Measurement noise covariance
    double p; // Estimation error covariance
    double x; // Value
};

class Accelerometer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)

public:
    explicit Accelerometer(QObject *parent = nullptr);
    ~Accelerometer();

    bool isActive() const { return sensor->isActive(); }

    Q_INVOKABLE double getXBias() const { return x_bias; }
    Q_INVOKABLE double getYBias() const { return y_bias; }

public slots:
    void start();
    void stop();
    void calibration();

signals:
    void readingUpdated(const QString &output);
    void activeChanged();
    void calibrationFinished(const QString &output);
    void newAcceleration(double x, double y);

private slots:
    void onSensorReadingChanged();
    void onCalibrationReadingChanged();
    void onCalibrationFinished();

private:
    QAccelerometer *sensor;
    QTimer *timer;
    QTimer *calibrationTimer;
    QVector<double> x_values;
    QVector<double> y_values;
    double x_bias;
    double y_bias;
    KalmanFilter xKalman; // Kalman filter for x-axis
    KalmanFilter yKalman; // Kalman filter for y-axis
};

#endif // ACCELEROMETER_H
