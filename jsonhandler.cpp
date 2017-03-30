#include <math.h>
#include <QFileInfo>
#include <QDateTime>
#include "common.h"
#include "json_common.h"
#include "jsonhandler.h"

#ifdef DEBUG_MODE
    #include <QDebug>
#endif // DEBUG_MODE

extern MENU_VAR_LIST menuVarList;
extern MENU_VIEW_LIST menuViewList;

JsonHandler::JsonHandler(const QString& filename)
{
    // class constructor open json file and
    // parse its content (if is reading) or
    // prepare to write

    QString fullPathFile = JSON_FILEPATH + filename;

    // various init
    status = JSON_UNDEFINED;
    menuType = JSON_MENU_TYPE_UNDEFINED;

    file.setFileName(fullPathFile);

    if(file.exists()) {
        QFileInfo fileinfo(file.fileName());
        QDateTime fileTimeStamp = fileinfo.lastModified();
        setFileTime(fileTimeStamp.toMSecsSinceEpoch());
    }
    else {
        setFileTime( (qint64) 0);
        status = JSON_FILE_NOT_EXISTS;
    }
}


int JsonHandler::checkFile()
{
    QDateTime fileTimeStamp;

    // first of all, check if file exists
    if(!file.exists()) {
        setFileTime((qint64) 0 /*fileTimeStamp.toMSecsSinceEpoch()*/);
        status = JSON_FILE_NOT_EXISTS;
        return (JSON_FILE_CHECK_ERROR);
    }

    // check if it was modified
    QFileInfo fileinfo(file.fileName());
    fileTimeStamp = fileinfo.lastModified();

    if(getFileTime() == fileTimeStamp.toMSecsSinceEpoch())
    {
        status = JSON_FILE_UNCHANGED;
        return JSON_FILE_CHECK_UNCHANGED;
    }
    else // update timestamp
        setFileTime(fileTimeStamp.toMSecsSinceEpoch());

    return JSON_FILE_CHECK_CHANGED;
}

void JsonHandler::Open(int mode)
{
    // call Open to read a json and obtain inner object (jo_MenuData)
    // or open file to write out a json

    if(file.isOpen()) file.close();

    if(mode == JSON_FILE_READONLY) { 

        // first of all, check if file exists
        if(status == JSON_FILE_NOT_EXISTS) return;

        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
                status = JSON_FILE_ERROR;
                return;
            }
        val = file.readAll();
        file.close();
        jsDoc = QJsonDocument::fromJson(val, &jerror);
        if(jerror.error != QJsonParseError::NoError){
            status = JSON_PARSE_ERROR;
            return;
        }

        jo_MenuData = jsDoc.object();

        status = JSON_OK;
        return;
    }

    if(mode == JSON_FILE_WRITE) {
        if(!file.open(QIODevice::WriteOnly)){
                status = JSON_FILE_ERROR;
                return;
            }

        status = JSON_OK;
        return;
    }
}


int JsonHandler::getStatus()
{
    return status;
}

JsonHandler::~JsonHandler()
{
    if(file.exists()) file.close();
}

void JsonHandler::Close()
{
    if(file.exists()) file.close();
}

void JsonHandler::setFileTime(qint64 val)
{
    this->lastMod = val;
}

qint64 JsonHandler::getFileTime()
{
    return(this->lastMod);
}

void JsonHandler::JsonWriteCmd(const QString &cmd, const QString &par)
{
    QJsonObject jo_Cmd;

    // create a generic command
    // syntax: {"cmd": "par"}
    jo_Cmd[cmd] = par;

    // save it to internal Json Document
    x.setObject(jo_Cmd);

    // set a ticket for write timer
    this->setWriteStatus(true);

}

void JsonHandler::JsonWriteCmd(const QString& cmd, const QString& mod, QString& par)
{
    // create a READ command
    // exampled syntax: {"READ": {"InstantValues": "systemValues"}}

    QJsonObject jo_Cmd, jo_valData;

    jo_valData[mod] = par;

    //jo_modData[ModName] = jo_valData;

    jo_Cmd[cmd] = jo_valData; //jo_modData;

    // save it to internal Json Document
    x.setObject(jo_Cmd);

    // set a ticket for write timer
    this->setWriteStatus(true);
}

void JsonHandler::JsonWriteVal(const QString& cmd/*, const QString& ModName*/, const QString& VarName, int val)
{
    // create a WRITEREPLY command
    // syntax: {"WRITEREPLY": {"VarName": intValue}}

    QJsonObject jo_Cmd, jo_valData;

    // QJsonObject jo_modData;
    // syntax: {"WRITEREPLY": {"ModName": {"VarName": intValue}}}

    jo_valData[VarName] = val;

    // jo_modData[ModName] = jo_valData;

    jo_Cmd[cmd] = jo_valData; //jo_modData;

    // save it to internal Json Document
    x.setObject(jo_Cmd);

    // set a ticket for write timer
    this->setWriteStatus(true);
}

void JsonHandler::JsonWriteVal(const QString& cmd/*, const QString& ModName*/, const QString& VarName, bool val)
{

    // create a WRITEREPLY command
    // syntax: {"WRITEREPLY": {"VarName": boolValue}}

    QJsonObject jo_Cmd, jo_valData;

    // QJsonObject jo_modData;
    // syntax: {"WRITEREPLY": {"ModName": {"VarName": intValue}}}

    jo_valData[VarName] = val;

    // jo_modData[ModName] = jo_valData;

    jo_Cmd[cmd] = jo_valData; //jo_modData;

    // save it to internal Json Document
    x.setObject(jo_Cmd);

    // set a ticket for write timer
    this->setWriteStatus(true);
}

void JsonHandler::JsonWriteVal(const QString &cmd/*, const QString &ModName*/, const QString &VarName, double val, int precision)
{

    // create a WRITEREPLY command
    // syntax: {"WRITEREPLY": {"VarName": floatValue}}
    QJsonObject jo_Cmd, jo_valData;

    // QJsonObject jo_modData;


    // syntax: {"WRITEREPLY": {"ModName": {"VarName": floatValue}}}

    jo_valData[VarName] = floor((val * pow(10, precision) + 0.5)) / pow(10, precision);

    //jo_modData[ModName] = jo_valData;

    jo_Cmd[cmd] = jo_valData; //jo_modData;

    // save it to internal Json Document
    x.setObject(jo_Cmd);

    // set a ticket for write timer
    this->setWriteStatus(true);
}

void JsonHandler::JsonWriteVal(const QString &cmd/*, const QString &ModName*/, const QString &VarName, const QString &val)
{
    /*
    this->Open(JSON_FILE_WRITE);

    if (getStatus() != JSON_OK) {
        //qDebug() << "Status: " << status << "Error" << jerror.errorString();
        return;
    }*/

    // create a WRITEREPLY command
    // syntax: {"WRITEREPLY": {"VarName": "Value"}}
    QJsonObject jo_Cmd, jo_valData;

    // QJsonObject jo_modData;
    // syntax: {"WRITEREPLY": {"ModName": {"VarName": "Value"}}}

    jo_valData[VarName] = val;

    //jo_modData[ModName] = jo_valData;

    jo_Cmd[cmd] = jo_valData; //jo_modData;

    // save it to internal Json Document
    x.setObject(jo_Cmd);

    // set a ticket for write timer
    this->setWriteStatus(true);
}


void JsonHandler::initVarList(MENU_VAR_LIST* p_mvList)
{
    p_mvList->varNum = 0;
    p_mvList->index  = 0;
    //memset(p_mvList, 0, sizeof(MENU_VAR_LIST));
}


void JsonHandler::updateVarList(MENU_VAR_LIST* p_mvList, QJsonObject jsObj, int index)
{
    QString varType = jsObj.value(QString(JSON_VAR_KEY_VARTYPE)).toString();

    // common items
    p_mvList->varNum = index;

    p_mvList->menuVar[index].RefMenu    = jsObj.value(QString(JSON_VAR_KEY_REFMENU)).toString();
    p_mvList->menuVar[index].VarName    = jsObj.value(QString(JSON_VAR_KEY_VARNAME)).toString();
    p_mvList->menuVar[index].ModName    = jsObj.value(QString(JSON_VAR_KEY_MODNAME)).toString();
    p_mvList->menuVar[index].VarLabel   = jsObj.value(QString(JSON_VAR_KEY_VARLABEL)).toString();
    p_mvList->menuVar[index].VarValueStr= jsObj.value(QString(JSON_VAR_KEY_VALUE_STR)).toString();
    p_mvList->menuVar[index].VarType    = jsObj.value(QString(JSON_VAR_KEY_VARTYPE)).toString();
    p_mvList->menuVar[index].varUnits   = jsObj.value(QString(JSON_VAR_KEY_UNITS)).toString();
    p_mvList->menuVar[index].VarMode    = jsObj.value(QString(JSON_VAR_KEY_VARMODE)).toString();
    p_mvList->menuVar[index].Size       = jsObj.value(QString(JSON_VAR_KEY_SIZE)).toInt();

    if(varType == JSON_VAR_TYPE_INT)    // integer
    {
        p_mvList->menuVar[index].minVal  = jsObj.value(QString(JSON_VAR_KEY_MINVALUE)).toInt();

        p_mvList->menuVar[index].maxVal  = jsObj.value(QString(JSON_VAR_KEY_MAXVALUE)).toInt();

        p_mvList->menuVar[index].stepVal  = jsObj.value(QString(JSON_VAR_KEY_STEPVALUE)).toInt();

        p_mvList->menuVar[index].iVarValue = jsObj.value(QString(JSON_VAR_KEY_VALUE)).toInt();

        return;
    }

    if(varType == JSON_VAR_TYPE_ENUM)    // enum
    {
        p_mvList->menuVar[index].minVal  = jsObj.value(QString(JSON_VAR_KEY_MINVALUE)).toInt();

        p_mvList->menuVar[index].maxVal  = jsObj.value(QString(JSON_VAR_KEY_MAXVALUE)).toInt();

        p_mvList->menuVar[index].stepVal  = jsObj.value(QString(JSON_VAR_KEY_STEPVALUE)).toInt();

        p_mvList->menuVar[index].iVarValue = jsObj.value(QString(JSON_VAR_KEY_VALUE)).toInt();

        // var reading block
        QJsonValue jsMenuItem = jsObj.value(QString(JSON_VAR_KEY_LIST));
        QJsonArray varItems = jsMenuItem.toArray();
        QJsonValue varData;

        // if not empty, clear list
        while ( p_mvList->menuVar[index].varList.isEmpty() == false )
            p_mvList->menuVar[index].varList.removeLast();

        foreach(const QJsonValue & value, varItems)
        {
            p_mvList->menuVar[index].varList.append(value.toString()); // populate list
        }
        return;
    }

    if(varType == JSON_VAR_TYPE_BOOL)    // boolean
    {
        p_mvList->menuVar[index].minVal  = jsObj.value(QString(JSON_VAR_KEY_MINVALUE)).toInt();

        p_mvList->menuVar[index].maxVal  = jsObj.value(QString(JSON_VAR_KEY_MAXVALUE)).toInt();

        p_mvList->menuVar[index].stepVal  = jsObj.value(QString(JSON_VAR_KEY_STEPVALUE)).toInt();

        p_mvList->menuVar[index].iVarValue = (jsObj.value(QString(JSON_VAR_KEY_VALUE)).toBool()==true?1:0);

        // var reading block
        QJsonValue jsMenuItem = jsObj.value(QString(JSON_VAR_KEY_LIST));
        QJsonArray varItems = jsMenuItem.toArray();
        QJsonValue varData;

        // if not empty, clear list
        while ( p_mvList->menuVar[index].varList.isEmpty() == false )
            p_mvList->menuVar[index].varList.removeLast();

        foreach(const QJsonValue & value, varItems)
        {
            p_mvList->menuVar[index].varList.append(value.toString()); // populate list
        }
        return;
    }

    if(varType == JSON_VAR_TYPE_FLOAT)    // float
    {
        p_mvList->menuVar[index].f_minVal  = jsObj.value(QString(JSON_VAR_KEY_MINVALUE)).toDouble();

        p_mvList->menuVar[index].f_maxVal  = jsObj.value(QString(JSON_VAR_KEY_MAXVALUE)).toDouble();

        p_mvList->menuVar[index].f_stepVal  = jsObj.value(QString(JSON_VAR_KEY_STEPVALUE)).toDouble();

        p_mvList->menuVar[index].f_VarValue = jsObj.value(QString(JSON_VAR_KEY_VALUE)).toDouble();

        p_mvList->menuVar[index].precision = jsObj.value((QString(JSON_VAR_KEY_PRECISION))).toInt();

        return;
    }

    if(varType == JSON_VAR_TYPE_STRING)    // string
    {
        p_mvList->menuVar[index].MaxLen = jsObj.value(QString(JSON_VAR_KEY_MAXLEN)).toInt();

        p_mvList->menuVar[index].strVarValue = jsObj.value(QString(JSON_VAR_KEY_VALUE)).toString();

        return;
    }
}

void JsonHandler::initMenuList(MENU_VIEW_LIST* p_mvList)
{
    p_mvList->viewNum = 0;
    //memset(p_mvList, 0, sizeof(MENU_VAR_LIST));
}


void JsonHandler::updateMenuList(MENU_VIEW_LIST *p_mvList, QJsonObject jsObj, int index)
{
    p_mvList->viewNum = index;

    p_mvList->menuView[index].RefMenu   = jsObj.value(QString(JSON_VAR_KEY_REFMENU)).toString();
    p_mvList->menuView[index].MenuName  = jsObj.value(QString(JSON_MENU_KEY_MENUNAME)).toString() ;
    p_mvList->menuView[index].MenuLabel = jsObj.value(QString(JSON_MENU_KEY_MENULABEL)).toString();
    p_mvList->menuView[index].Background= jsObj.value(QString(JSON_MENU_KEY_BACK)).toString();    
    p_mvList->menuView[index].CmdID     = jsObj.value(QString(JSON_MENU_KEY_CMD_ID)).toString();
    p_mvList->menuView[index].ParserName= jsObj.value(QString(JSON_MENU_KEY_PARSER_NAME)).toString();
    p_mvList->menuView[index].ReqTime   = jsObj.value(QString(JSON_MENU_KEY_REQ_FREQ)).toInt();

    p_mvList->menuView[index].itemID    = index;
}


void JsonHandler::ReadData()
{
    this->Open(JSON_FILE_READONLY);

    if (getStatus() != JSON_OK) {
        //qDebug() << "Status: " << status << "Error" << jerror.errorString();
        return;
    }

    //QJsonObject jo_Cmd, jo_valData;
    int i = 0;
    QJsonValue jsonItem, jsVarData;
    QJsonObject jsVarObject;

    QJsonValue jsMenuItem = jo_MenuData.value(QString(JSON_READ_QT_MENU_KEY));

    QJsonObject jsMenuItemObj = jsMenuItem.toObject();

    QJsonValue js_MenuTypeVar= jsMenuItemObj[QString(JSON_READ_MENU_TYPE_KEY)];

    if(js_MenuTypeVar.toString() == QString(JSON_READ_MENU_VAR_KEY)) {

        menuType = JSON_MENU_TYPE_VAR_LIST_VIEW;
        // var reading block
        jsMenuItem = jsMenuItemObj.value(QString(JSON_READ_MENU_VARLIST_KEY));
        QJsonArray varItems = jsMenuItem.toArray();

        initVarList(&menuVarList);

        foreach(const QJsonValue & value, varItems)
        {
            // take the object associated to the indexed value from list

            jsonItem = varItems[i];

            QJsonValue varData = jsMenuItemObj[jsonItem.toString()];    // take value as a key...

            jsVarObject = varData.toObject();   // transform it to json object

            updateVarList(&menuVarList, jsVarObject, i);    // update indexed item of structure that holds the var
            i++;
        }

        return;
    }

    if(js_MenuTypeVar.toString() == QString(JSON_READ_MENU_KEY)) {

        menuType = JSON_MENU_TYPE_MENU_VIEW;

        // var reading block
        jsMenuItem = jsMenuItemObj.value(QString(JSON_READ_MENU_VARLIST_KEY));
        QJsonArray varItems = jsMenuItem.toArray();

        initMenuList(&menuViewList);

        foreach(const QJsonValue & value, varItems)
        {
            // take the object associated to the indexed value from list

            jsonItem = varItems[i];

            QJsonValue varData = jsMenuItemObj[jsonItem.toString()];    // take value as a key...

            jsVarObject = varData.toObject();   // transform it to json object

            updateMenuList(&menuViewList, jsVarObject, i);    // update indexed item of structure that holds the var
            i++;
        }

        return;
    }

    // no known data was found
    status = JSON_PARSE_ERROR;
}

void JsonHandler::Read_InstantValues(QJsonObject* js_objVal)
{
    this->Open(JSON_FILE_READONLY);

    if (getStatus() != JSON_OK) {
        //qDebug() << "Status: " << status << "Error" << jerror.errorString();
        return;
    }


    if(!(jo_MenuData.contains(QString(JSON_READ_QT_MOD_NAME)))) {
        // this json doesn't contains instant values data
        // no known data was found
        status = JSON_PARSE_ERROR;
        return;
    }

    // parse jo_MenuData object
    QJsonValue jsIVItem = jo_MenuData.value(QString(JSON_READ_QT_MOD_NAME));

    *js_objVal = jsIVItem.toObject();

    status = JSON_OK;
}


void JsonHandler::updateWriteFile()
{
    // process write requests
    if(this->getWriteStatus()) {

        // set a ticket for write timer
        this->setWriteStatus(false);

        this->Open(JSON_FILE_WRITE);

        if (getStatus() != JSON_OK) {
            //qDebug() << "Status: " << status << "Error" << jerror.errorString();
            this->Close();
            return;
        }

        // save it to file
        this->file.write(x.toJson());

        this->Close();
    }
}

bool JsonHandler::getWriteStatus()
{
    return req_write;
}

void JsonHandler::setWriteStatus(bool status)
{
    req_write = status;
}
