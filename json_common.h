#ifndef JSON_COMMON_H
#define JSON_COMMON_H

#define MAX_CHECK_FILE_RETRIES          100
#define JSON_WRITE_FILE_TIMER_INTERVAL  100

/* file block */
#define JSON_FILEPATH               "/tmp/product/"
#define JSON_FILE_READONLY          0
#define JSON_FILE_READWRITE         1
#define JSON_FILE_WRITE             2

#define JSON_UNDEFINED              100
#define JSON_OK                     0
#define JSON_FILE_ERROR             1
#define JSON_PARSE_ERROR            2
#define JSON_FILE_UNCHANGED         3
#define JSON_FILE_NOT_EXISTS        4

#define JSON_FILE_CHECK_ERROR       -1
#define JSON_FILE_CHECK_UNCHANGED   0
#define JSON_FILE_CHECK_CHANGED     1

/* command block */
#define JSON_READ_MENU_CMD          "READ_QT_MENU"
#define JSON_READ_MENU_ROOT         "/"
#define JSON_READ_MENU_CURRENT      "."
#define JSON_READ_MENU_UP           ".."

#define JSON_WRITE_CMD              "WRITEREPLY"
#define JSON_READ_CMD               "READ"

/* keys and values block */
#define JSON_READ_QT_MOD_NAME       "InstantValues"

#define JSON_READ_IV_KEY_SYSTEM     "systemValues"
#define JSON_READ_IV_KEY_RDS        "rdsValues"
#define JSON_READ_IV_KEY_MPX        "mpxValues"
#define JSON_READ_IV_KEY_AUDIO      "audioValues"
#define JSON_READ_IV_KEY_STREAMER   "streamerValues"

#define JSON_IV_SYSTEM_PARSER       "systemParser"
#define JSON_IV_RDS_PARSER          "rdsParser"
#define JSON_IV_MPX_PARSER          "mpxParser"
#define JSON_IV_AUDIO_PARSER        "audioParser"
#define JSON_IV_STREAMER_PARSER     "streamerParser"


#define JSON_READ_QT_MENU_KEY       "QT_MENU"
#define JSON_READ_MENU_VAR_KEY      "Vars"
#define JSON_READ_MENU_KEY          "Menu"
#define JSON_READ_MENU_TYPE_KEY     "Type"
#define JSON_READ_MENU_VARLIST_KEY  "VarList"

#define JSON_MENU_TYPE_UNDEFINED     0
#define JSON_MENU_TYPE_MENU_VIEW     1
#define JSON_MENU_TYPE_VAR_LIST_VIEW 2

#define JSON_VAR_KEY_REFMENU        "RefMenu"
#define JSON_VAR_KEY_VARNAME        "VarName"
#define JSON_VAR_KEY_MODNAME        "ModName"
#define JSON_VAR_KEY_VARMODE        "VarMode"
#define JSON_VAR_KEY_VARTYPE        "VarType"
#define JSON_VAR_KEY_VARLABEL       "VarLabel"
#define JSON_VAR_KEY_MINVALUE       "MinValue"
#define JSON_VAR_KEY_MAXVALUE       "MaxValue"
#define JSON_VAR_KEY_STEPVALUE      "StepValue"
#define JSON_VAR_KEY_PRECISION      "Precision"
#define JSON_VAR_KEY_VALUE          "VarValue"
#define JSON_VAR_KEY_VALUE_STR      "VarValueStr"
#define JSON_VAR_KEY_UNITS          "VarUnits"
#define JSON_VAR_KEY_LIST           "List"
#define JSON_VAR_KEY_LIST_MINVAL    "ListMinValue"
#define JSON_VAR_KEY_LIST_MAXVAL    "ListMaxValue"
#define JSON_VAR_KEY_LIST_STEP      "ListStepValue"
#define JSON_VAR_KEY_PRELIST        "PreList"
#define JSON_VAR_KEY_POSTLIST       "PostList"
#define JSON_VAR_KEY_MAXLEN         "MaxLength"
#define JSON_VAR_KEY_SIZE           "SizeFactor"

#define JSON_MENU_KEY_MENUNAME      "MenuName"
#define JSON_MENU_KEY_MENULABEL     "MenuLabel"
#define JSON_MENU_KEY_BACK          "Background"
#define JSON_MENU_KEY_CMD_ID        "CommandId"
#define JSON_MENU_KEY_PARSER_NAME   "ParserName"
#define JSON_MENU_KEY_REQ_FREQ      "ReqFrequency"

#define JSON_MENU_KEYVAL_SYSTEM     "systemValues"
#define JSON_MENU_KEYVAL_MPX        "mpxValues"
#define JSON_MENU_KEYVAL_RDS        "rdsValues"
#define JSON_MENU_KEYVAL_AUDIO      "audioValues"

#define JSON_VAR_TYPE_INT           "Integer"
#define JSON_VAR_TYPE_ENUM          "Enum"
#define JSON_VAR_TYPE_FLOAT         "Float"
#define JSON_VAR_TYPE_STRING        "String"
#define JSON_VAR_TYPE_BOOL          "Boolean"

#define JSON_VAR_MODE_READONLY      "R"
#define JSON_VAR_MODE_READWRITE     "RW"

/////////////////////////////////////////////////

#endif // JSON_COMMON_H

