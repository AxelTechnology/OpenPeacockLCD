#ifndef JSONHANDLER_H
#define JSONHANDLER_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QIODevice>
#include <QFile>


class JsonHandler
{
 private:
    /**
     * @brief file is the source json file
     */
    QFile file;

    /**
     * @brief lastMod holds the last write session timestamp
     */
    qint64 lastMod;

    /**
     * @brief val contains whole json doc with utf-16 char encoding
     */
    QByteArray val;

    /**
     * @brief jerror returns status of json parsing
     */
    QJsonParseError jerror;

    /**
     * @brief jsDoc is the parsed document
     */
    QJsonDocument jsDoc;

    /**
     * @brief x is the written document
     */
    QJsonDocument x;

    /**
     * @brief req_write holds write request status
     */
    bool req_write;

    /**
     * @brief jo_MenuData contains whole json document object
     */
    QJsonObject jo_MenuData;

    /**
     * @brief js_MenuItem contains the (object) value of main key
     */
    QJsonValue  js_MenuItem;

    /**
     * @brief status says if json file is correctly opened
     */
    int status;

    /**
     * @brief menuType tells if concerning a Menu view or a VarList view
     */
    int menuType;

    /**
     * @brief Open
     * @param mode (open mode: read-only, read&write, write-only)
     * status var is updated
     */
    void Open(int mode);

    /**
     * @brief Close the file
     */
    void Close();

    /* var list handler block */

    /**
     * @brief initVarList resets internal value
     * @param p_mvList pointer to structures
     */
    void initVarList(MENU_VAR_LIST* p_mvList);

    /**
     * @brief updateVarList fills var structures with data retrieved from json object
     * @param p_mvList pointer to struct list
     * @param jsObj the json object
     * @param index of associated var structure
     */
    void updateVarList(MENU_VAR_LIST* p_mvList, QJsonObject jsObj, int index);

    /**
     * @brief initMenuList resets internal menu value
     * @param p_mvList pointer to structures
     */
    void initMenuList(MENU_VIEW_LIST* p_mvList);

    /**
     * @brief updateMenuList fills menu structures with data retrieved from json object
     * @param p_mvList pointer to struct list
     * @param jsObj the json object
     * @param index of associated menu structure
     */
    void updateMenuList(MENU_VIEW_LIST* p_mvList, QJsonObject jsObj, int index);

 public:
    /**
     * @brief JsonHandler class constructor
     * @param filename is the json filename (with file path)
     */
    JsonHandler(const QString& filename);
    ~JsonHandler();

    /**
     * @brief getStatus tells operative status
     * @return status val
     */
    int getStatus();

    /**
     * @brief getMenuItemType queries private val menuType;
     * @return the value that says if json refers to a MENU view or a Var List view.
     */
    int getMenuItemType();

    /**
     * @brief JsonWriteCmd send command to retrieve data
     * @param cmd the command
     * @param par the parameter
     */
    void JsonWriteCmd(const QString& cmd, const QString& par);

    /**
     * @brief JsonWriteCmd send command to retrieve data
     * @param cmd the READ command
     * @param Mod module for Instant Values
     * @param par the requested module
     */
    void JsonWriteCmd(const QString& cmd, const QString& mod, QString& par);

    /**
     * @brief updateWriteFile timer handled writing function
     */
    void updateWriteFile();

    /**
     * @brief setWriteStatus set req_write val
     * @param status
     */
    void setWriteStatus( bool status );

    /**
     * @brief getWriteStatus get req_write val
     * @return req_write
     */
    bool getWriteStatus();

    /**
     * @brief JsonWriteVal is the WRITEREPLY function
     * @param cmd   the command WRITEREPLY
     * @param ModName module name
     * @param VarName variable name
     * @param val the var value
     */
    void JsonWriteVal(const QString& cmd,/* const QString& ModName,*/ const QString& VarName, int val);
    void JsonWriteVal(const QString& cmd,/* const QString& ModName,*/ const QString& VarName, bool val);
    void JsonWriteVal(const QString& cmd,/* const QString& ModName,*/ const QString& VarName, double val, int precision);
    void JsonWriteVal(const QString& cmd,/* const QString& ModName,*/ const QString& VarName, const QString& val);

    /**
     * @brief ReadData retrieve ALL data needed to backgrounds, data, menus...
     */
    void ReadData();

    /**
     * @brief Read_InstantValues retrieve a QJsonObject data from file and set status (Parsing OK or Error)
     * @param js_objVal pointer to json object with instant values
     */
    void Read_InstantValues(QJsonObject *js_objVal);

    /**
     * @brief getFileTime retrieve file time stamp
     * @return the time expressed as timeSinceEpoch();
     */
    qint64 getFileTime();

    /**
     * @brief setFileTime set timestamp mark
     * @param val the timestamp
     */
    void setFileTime(qint64 val);

    /**
     * @brief checkFile tells if file exists and if its timestamp was changed
     * @return change status or error
     */
    int checkFile();

};

#endif // JSONHANDLER_H
