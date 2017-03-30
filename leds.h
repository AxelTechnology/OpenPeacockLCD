#ifndef LEDS_H
#define LEDS_H

#include <QObject>
#include <QTimer>
#include <QFileInfo>
#include <QString>
#include <QByteArray>
#include "common.h"

//#define LED_FILE_PATH "/var/gpio/"


#define WHEEL_1 "/var/gpio/WHEEL_1"
#define WHEEL_2 "/var/gpio/WHEEL_2"
#define WHEEL_3 "/var/gpio/WHEEL_3"
#define WHEEL_4 "/var/gpio/WHEEL_4"


#define LED_BLINK_TIMER         500 // msec
#define LED_AUTO_TIMER          100

#define ROTATION_LED_ON_TIMER_MAX   20


#define LED_OFF                 0
#define LED_ON                  1

#define BLINK_STOP              0
#define BLINK_START             1

#define WHEEL_ROT_POS_MAX       3

#define WHEEL_ROT_RIGHT         0
#define WHEEL_ROT_LEFT          1

class Leds : public QObject
{
    Q_OBJECT

    void writeLed(QString fileName, int val);

    bool   blinkStatus;
    QTimer blinkTimer;
    QTimer shutDownTimer;

    bool wheel1_blink;
    bool wheel2_blink;
    bool wheel3_blink;
    bool wheel4_blink;

    int rotationStatus;

    int shutdownCounter;

    /**
     * @brief Led_Rotation rule to be applied to leds
     * @param dir is the direction
     */
    void Led_Rotation(int dir);

public:
    explicit Leds(QObject *parent = 0);

    /**
     * @brief Set_Wheel commands all wheel leds
     * @param val OFF or ON
     */
     void Set_Wheel(int val);

    /**
     * @brief Set_Wheel_Chan command single wheel led channel
     * @param wheelCh the led channel
     * @param val OFF or ON
     */
     void Set_Wheel_Chan(QString wheelCh, int val);

     void Wheel_Rotate_Right();

     void Wheel_Rotate_Left();

    /**
     * @brief Wheel_Blink blinks all wheel leds
     * @param status is BLINK_START or BLINK_STOP
     */
     void Wheel_Blink(int status);

     /**
      * @brief initBlinkTimer initialze blinker
      * @param recv object connected to timer
      */
     void initBlinkTimer(const QObject* recv);

     void initWheelStatus();

signals:

public slots:
    void updateBlinkTimer();
    void updateShutDownTimer();
};

#endif // LEDS_H
