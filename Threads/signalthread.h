#ifndef SIGNALTHREAD_H
#define SIGNALTHREAD_H

#include <linux/input.h>
#include <QThread>

#define ENCODER_NAME "badff-encoder"
#define ENCODER_PATH "/dev/input/event"
#define MAX_SYS_EVENTS  3   // last searched path: /dev/input/event2

#define POLLING_THREAD_INTERVAL_MS  20  // msec
#define DOUBLE_CLICK_TIME_MS        POLLING_THREAD_INTERVAL_MS*20
#define LONG_CLICK_TIME_MS          POLLING_THREAD_INTERVAL_MS*70

// Finite State Machine Status
#define FSMS_START                  0
#define FSMS_DOWN_1                 10
#define FSMS_UP_1                   20
#define FSMS_DOWN_2                 30
#define FSMS_UP_2                   40
#define FSMS_LPRESS                 50

// Finite State Machine Events
#define FSME_DOWN                   0
#define FSME_UP                     1
#define FSME_LEFT                   2
#define FSME_RIGHT                  3
#define FSME_NO_EVENT               4


class SignalThread : public QThread
{
    Q_OBJECT

public:
    SignalThread(QObject *parent = 0);
    ~SignalThread();

protected:
    /**
     * @brief Override of run method
     */
    void run();

private:
    /**
     * @brief Default is true, when false, exit the run method
     */
    bool m_boExitThread;

    /**
     * @brief encoderDrvFound tells when encoder driver was correctly detected and opened
     */
    bool encoderDrvFound;

    /**
     * @brief name holds encoder driver name
     */
    char name[128];

    /**
     * @brief array of input_event type variables
     */
    struct input_event ev[64];

    /**
     * @brief internal use variables
     */
    int fd, rd, size;
    char device[128];

    /**
     * @brief st_doubleClickCounter is for double-click function
     */
    int st_doubleClickCounter;

    /**
     * @brief st_longPressCounter is for long-press function
     */
    int st_longPressCounter;

    /**
     * @brief st_stateMachineStatus indicates status of finite state machine
     */
    int st_stateMachineStatus;

    /**
     * @brief st_stateMachineEvent records catched encoder events
     */
    int st_stateMachineEvent;

    /**
     * @brief init file asssociated to Encoder
     */
    int initEncoder();

signals:
    /**
     * @brief functions asssociated to Encoder events
     */
    void encoderRightRot();
    void encoderLeftRot();
    void encoderPressUp();
    void encoderPressDown();

    /**
     * @brief higher level functions for encoder events
     */
    void encoderClick();
    void encoderLongPress();
    void encoderDoubleClick();
};
#endif // SIGNALTHREAD_H
