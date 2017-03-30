#ifndef GRAPHICHANDLER_H
#define GRAPHICHANDLER_H

#include <QObject>
#include "common.h"
#include "json_common.h"
#include "jsonhandler.h"
#include <QTimer>


#define HANDLER_STATUS_INIT_START           0
#define HANDLER_STATUS_INIT_CHECK_FILE      100
#define HANDLER_STATUS_INIT_READ_DATA       200
#define HANDLER_STATUS_SET_DATA             300
#define HANDLER_STATUS_WAIT_LOOP            400

#define HANDLER_STATUS_ERROR                10000

#define MAX_READ_FILE_RETRIES               3

#define HANDLER_CMD_RUN_OK                  0
#define HANDLER_CMD_STOPPED                 1
#define HANDLER_CMD_ERROR                   2

#define HANDLER_TIMER_BASE                  50 // msec


class GraphicHandler : public QObject
{
    Q_OBJECT

    /**
     * @brief status tells if it is sleeping, running ok or there is an error
     */
    int status;

    /**
     * @brief menuViewIndex current handled menu view
     */
    int menuViewIndex;

    /**
     * @brief pollingInterval request rate
     */
    int pollingInterval;

public:
    explicit GraphicHandler(QObject *parent = 0);

    /**
     * @brief mainTimer needed to handle all system
     */
    QTimer handlerTimer;

    /**
     * @brief startHandler runs status machine timer
     */
    void startHandler();
    /**
     * @brief stopHandler stops status machine timer
     */
    void stopHandler();

    /**
     * @brief initHandler loads parameters useful for status machine
     */
    void initHandler(JsonHandler* jshInput, JsonHandler* jshOutput);

    /**
     * @brief setHandler changes some data for module handling
     * @param timerInterval msecs
     */
    void setHandler(int timerInterval);


    /**
     * @brief Input, Output pointers to IV files
     */
    JsonHandler* Input;
    JsonHandler* Output;

    /**
     * @brief iv_Obj actual placeholder for retrieved instant values json
     */
    QJsonObject iv_Obj;

    /**
     * @brief getStatus retrieve internal status
     * @return the status
     */
    int getStatus();

    /**
     * @brief setStatus set internal status
     * @param status to be changed
     */
    void setStatus(int st);

    /**
     * @brief setCurrentViewIndex set current handled menu view
     * @param index of the current view
     */
    void setCurrentViewIndex(int index);

signals:
    void parseModuleDataReq();
    void paintModuleDataReq();

public slots:
    void parseModuleData();
    /**
     * @brief updateHandlerTimer check for json updates and calls module methods
     */
    void updateHandlerTimer();
};

#endif // GRAPHICHANDLER_H
