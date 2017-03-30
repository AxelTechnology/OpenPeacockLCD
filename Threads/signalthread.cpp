#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <signal.h>
#include "signalthread.h"
//#include <QDebug>

SignalThread::SignalThread(QObject *parent) :
                               QThread(parent),
                               m_boExitThread(false)
{

    int status; // tells if init is ok

    // setup connection
    status = initEncoder();

    // if init was ok, then start the thread
    if (!status) this->start();
}

SignalThread::~SignalThread()
{
    // set the exit flag to true
    m_boExitThread = true;

    // wait for the run method to be completed
    this->wait();
}

int SignalThread::initEncoder()
{
    this->size = sizeof (struct input_event);

    // start searching for encoder
    this->encoderDrvFound = false;

    int i=0;    //counter for search
    QString encoderPath, checkName;

    while (!(this->encoderDrvFound) && i<MAX_SYS_EVENTS){

        //Select device to be tracked
        encoderPath = ENCODER_PATH + QString::number(i);
        strcpy(this->device, encoderPath.toStdString().c_str());

        //Open Device
        if ((this->fd = open (this->device, O_RDONLY | O_NONBLOCK)) == -1)
        {
           // qDebug() << device << " is not a vaild device.\n";

            // do nothing: an event-driven device may be pulled out
            // simply, jump to next item
            i++;
            continue;
        }

        // check if opened device is the right encoder driver
        ioctl (fd, EVIOCGNAME (sizeof (name)), name);

        checkName = name;
        if(checkName.contains(ENCODER_NAME)) {
            // found !!
            this->encoderDrvFound = true;
            break;
        }

        close(fd);

        i++;

    }

    if(encoderDrvFound) {
        // init state machine vars
        st_stateMachineStatus = FSMS_START;
        st_stateMachineEvent = FSME_NO_EVENT;
        st_longPressCounter   = 0;
    }


    return (0);
}

void SignalThread::run()
{
    static time_t       lastEvSec=-1;
    static suseconds_t  lastEvuSec=-1;

    forever {

        if (m_boExitThread == true)
        {
            // in this case exit the run method
            break;
        }

        // sleep
        msleep(POLLING_THREAD_INTERVAL_MS);



        if ((rd = read (fd, ev, size * 64)) < size)
        {
             //qDebug() << "cannot read from " << device <<". exit with value " << this->rd;

            // this->m_boExitThread = true;
            //continue;
        }

        // each time reset event
        st_stateMachineEvent = FSME_NO_EVENT;

        if (this->ev[0].value != ' ')
        {
             //qDebug() << "Type: " << this->ev[0].type << " - Code: " << this->ev[0].code << " - Value: " << this->ev[0].value;

            if(this->ev[0].type == 2 && this->ev[0].code == 8)
            {
                // Encoder rotation
                // check event time stamp
                if( lastEvSec  != this->ev[0].time.tv_sec ||
                    lastEvuSec != this->ev[0].time.tv_usec  ) {

                    lastEvSec = this->ev[0].time.tv_sec;
                    lastEvuSec= this->ev[0].time.tv_usec;
                }
                else goto end_loop;

                if (this->ev[0].value > 0)
                    // ROT_RIGHT
                    //emit this->encoderRightRot();
                    st_stateMachineEvent = FSME_RIGHT;
                else
                    // ROT_LEFT
                    //emit this->encoderLeftRot();
                    st_stateMachineEvent = FSME_LEFT;
            }

            if(this->ev[0].type == 1 && this->ev[0].code == 256)
            {
                // Encoder press
                if (this->ev[0].value == 1)
                    // ROT_PRESS_UP
                    //emit this->encoderPressUp();
                    st_stateMachineEvent = FSME_UP;
                else
                    //ROT_PRESS_DOWN
                    //emit this->encoderPressDown();
                    st_stateMachineEvent = FSME_DOWN;
            }
        }
        else break;

        //qDebug() << "event: " << st_stateMachineEvent << "status: " <<st_stateMachineStatus;

        switch(st_stateMachineStatus) {

            case FSMS_START:
                if(st_stateMachineEvent == FSME_LEFT) {
                    emit this->encoderLeftRot();
                    //qDebug() << "event: LEFT rot";
                    break;
                }

                if(st_stateMachineEvent == FSME_RIGHT) {
                    emit this->encoderRightRot();
                    //qDebug() << "event: RIGHT rot";
                    break;
                }

                // if (st_stateMachineEvent == FSME_UP) break;

                if(st_stateMachineEvent == FSME_DOWN) {

                    // reset timers and change status
                    st_stateMachineStatus = FSMS_DOWN_1;
                    st_longPressCounter   = 0;
                    //emit this->encoderClick(); // comment this line for HOLD function
                }

            break;

            case FSMS_DOWN_1:
                st_longPressCounter   += POLLING_THREAD_INTERVAL_MS;

                if(st_stateMachineEvent == FSME_LEFT) {
                    st_longPressCounter   = 0;
                    st_stateMachineStatus = FSMS_START;
                    emit this->encoderLeftRot();
                    break;
                }

                if(st_stateMachineEvent == FSME_RIGHT) {
                    st_longPressCounter   = 0;
                    st_stateMachineStatus = FSMS_START;
                    emit this->encoderRightRot();
                    break;
                }

                if(st_longPressCounter >= LONG_CLICK_TIME_MS) {
                    // is a long-press() event
                    st_longPressCounter   = 0;
                    st_stateMachineStatus = FSMS_LPRESS;
                    emit this->encoderLongPress();
                    break;
                }


                if(st_stateMachineEvent == FSME_UP) {
                    st_longPressCounter   = 0;
                    st_stateMachineStatus = FSMS_START;
                    emit this->encoderClick(); // uncomment this line for HOLD function
                    break;
                }

            break;

            case FSMS_LPRESS:

                if(st_stateMachineEvent == FSME_UP) {
                    st_longPressCounter   = 0;
                    st_stateMachineStatus = FSMS_START;
                    //emit this->encoderClick();
                    break;
                }

                if(st_stateMachineEvent == FSME_LEFT) {
                    st_longPressCounter   = 0;
                    //st_stateMachineStatus = FSMS_START;
                    emit this->encoderLeftRot();
                    break;
                }

                if(st_stateMachineEvent == FSME_RIGHT) {
                    st_longPressCounter   = 0;
                    //st_stateMachineStatus = FSMS_START;
                    emit this->encoderRightRot();
                    break;
                }
            break;

        }
 end_loop:
        // reset event flag
        st_stateMachineEvent = FSME_NO_EVENT;
    }

}
