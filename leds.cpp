#include "leds.h"

Leds::Leds(QObject *parent) : QObject(parent)
{
    shutDownTimer.setInterval(LED_AUTO_TIMER);
    connect(&shutDownTimer, SIGNAL(timeout()),
            this, SLOT(updateShutDownTimer()));

    Leds::wheel1_blink = false;
    Leds::wheel2_blink = false;
    Leds::wheel3_blink = false;
    Leds::wheel4_blink = false;

    Leds::blinkStatus  = false;

    blinkTimer.setInterval(LED_BLINK_TIMER);
    // connect the timer's timeout signal to a private slot
    connect(&blinkTimer, SIGNAL(timeout()),
            this, SLOT(updateBlinkTimer()));
}

void Leds::initBlinkTimer(const QObject *recv)
{
    Leds::wheel1_blink = false;
    Leds::wheel2_blink = false;
    Leds::wheel3_blink = false;
    Leds::wheel4_blink = false;

    Leds::blinkStatus  = false;

    blinkTimer.setInterval(LED_BLINK_TIMER);
    // connect the timer's timeout signal to a private slot
    connect(&blinkTimer, SIGNAL(timeout()),
            recv, SLOT(updateBlinkTimer()));
}

void Leds::writeLed(QString fileName, int val)
{
    QFile file;

    file.setFileName(fileName);

    if(file.exists()) file.close();

    if(!file.open(QIODevice::WriteOnly)){
            return;
        }

    file.write(QByteArray::number(val));
    //file.flush();
    file.close();
}



void Leds::Set_Wheel(int val)
{
    writeLed(QString (WHEEL_1), val);
    writeLed(QString (WHEEL_2), val);
    writeLed(QString (WHEEL_3), val);
    writeLed(QString (WHEEL_4), val);
}



void Leds::Set_Wheel_Chan(QString wheelCh, int val)
{
    writeLed(wheelCh, val);
}

void Leds::Wheel_Blink(int status)
{
    if(status == BLINK_STOP) {
        wheel1_blink = false;
        wheel2_blink = false;
        wheel3_blink = false;
        wheel4_blink = false;

        blinkTimer.stop();

        Leds::Set_Wheel(LED_OFF);
        return;
    }

    wheel1_blink = true;
    wheel2_blink = true;
    wheel3_blink = true;
    wheel4_blink = true;

    blinkTimer.start();
}



void Leds::updateBlinkTimer()
{
   blinkStatus = !blinkStatus;

   if(wheel1_blink) Set_Wheel_Chan((QString) WHEEL_1, (int)blinkStatus);
   if(wheel2_blink) Set_Wheel_Chan((QString) WHEEL_2, (int)blinkStatus);
   if(wheel3_blink) Set_Wheel_Chan((QString) WHEEL_3, (int)blinkStatus);
   if(wheel4_blink) Set_Wheel_Chan((QString) WHEEL_4, (int)blinkStatus);

}

void Leds::initWheelStatus()
{
    rotationStatus = 0;
}

void Leds::Led_Rotation(int dir)
{
    shutdownCounter = ROTATION_LED_ON_TIMER_MAX;
    if(!(shutDownTimer.isActive())) shutDownTimer.start();

    if (dir == WHEEL_ROT_RIGHT) {
        rotationStatus++;
        if(rotationStatus > WHEEL_ROT_POS_MAX) rotationStatus = 0;
    }
    else {
        rotationStatus--;
        if(rotationStatus < 0) rotationStatus = WHEEL_ROT_POS_MAX;
    }

    switch(rotationStatus)
    {
        case 0:
            Set_Wheel_Chan(WHEEL_1, LED_OFF);
            Set_Wheel_Chan(WHEEL_2, LED_ON);
            Set_Wheel_Chan(WHEEL_3, LED_OFF);
            Set_Wheel_Chan(WHEEL_4, LED_OFF);
        break;

        case 1:
            Set_Wheel_Chan(WHEEL_1, LED_OFF);
            Set_Wheel_Chan(WHEEL_2, LED_OFF);
            Set_Wheel_Chan(WHEEL_3, LED_ON);
            Set_Wheel_Chan(WHEEL_4, LED_OFF);
        break;

        case 2:
            Set_Wheel_Chan(WHEEL_1, LED_OFF);
            Set_Wheel_Chan(WHEEL_2, LED_OFF);
            Set_Wheel_Chan(WHEEL_3, LED_OFF);
            Set_Wheel_Chan(WHEEL_4, LED_ON);
        break;

        case 3:
            Set_Wheel_Chan(WHEEL_1, LED_ON);
            Set_Wheel_Chan(WHEEL_2, LED_OFF);
            Set_Wheel_Chan(WHEEL_3, LED_OFF);
            Set_Wheel_Chan(WHEEL_4, LED_OFF);
        break;
    }

}

void Leds::Wheel_Rotate_Right()
{
   Led_Rotation(WHEEL_ROT_RIGHT);
}

void Leds::Wheel_Rotate_Left()
{
    Led_Rotation(WHEEL_ROT_LEFT);
}

void Leds::updateShutDownTimer()
{
    shutdownCounter--;
    if(shutdownCounter == 0) {
       shutdownCounter = ROTATION_LED_ON_TIMER_MAX;
       shutDownTimer.stop();
       Set_Wheel(LED_OFF);
    }
}
