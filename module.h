#ifndef MODULE_H
#define MODULE_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "common.h"
#include "graphichandler.h"

#define MODULE_STATUS_OK                0
#define MODULE_STATUS_PARSING_ERROR     1


class Module : public QObject
{
    Q_OBJECT


    /**
     * @brief Background  filename with full path
     */
    QString Background;

    /**
     * @brief CommandID json data key
     */
    QString CommandID;

    /**
     * @brief moduleStatus tells if all is OK or there ia an error
     */
    int moduleStatus;

public:
    explicit Module(QObject *parent = 0);
    ~Module();
    /**
     * @brief initModule set default vlues for some stuff
     */
    void initModule();

    /**
     * @brief instantValues is a json with module data
     */
    QJsonObject instantValues;

signals:

public slots:
    /**
     * @brief moduleParseData parse instantValues json object
     * @param parserName the name of invoked custom parser
     */
    void moduleParseData(QString* parserName);

    /**
     * @brief modulePaintData paint data on widget
     * @param painterName the name of invoked custom painter
     */
    void modulePaintData(QString* painterName);
};

#endif // MODULE_H
