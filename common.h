#ifndef COMMON_H
#define COMMON_H

// here will take place vars and functions of common use for the project

#include <QString>
#include <QStringList>

// for debug purpose
#define DEBUG_MODE

// enable keyboard capture to simulate physical rotating encoder
//#define UI_VIRTUAL_ENCODER

// encoder emulator can exists only in degug mode
#ifndef DEBUG_MODE
    #undef UI_VIRTUAL_ENCODER
#endif

/** Get the stylesheet file as string
* @param the file name of the stylesheet
* @return the stylesheet file as string
*/
QString getStyleSheetAsString(const QString& p_strFileName);

/* EVENT structure */

typedef struct EVENT_STATE_T{
    bool encLeftRot;
    bool encRightRot;
    bool encPressDown;
    bool encPressUp;
    bool encClick;
    bool encDblClick;
    bool encHold;
} EVENT_STATE;


/* variable structures */

#define MAX_VARS_IN_MENU_LIST       50

typedef struct MENU_VAR_T
{
    QString RefMenu;    // ancestor's name
    QString VarName;    // true var name
    QString ModName;    // owner's mod name
    QString VarType;    // type of var
    QString VarLabel;   // shown full name of var
    QString VarMode;    // Read or Read'n'Write
    int     iVarValue;  // int var value
    double  f_VarValue; // float var value
    QString strVarValue;// string var value
    QString VarValueStr;// Var val associated string (menu label)
    int     minVal;     // minimum value
    int     maxVal;     // max value
    int     stepVal;    // step
    double  f_minVal;   // floating point min value
    double  f_maxVal;   // floating point max val
    double  f_stepVal;  // floating point step
    int     precision;  // floating point precision (number of decimals)
    QString varUnits;   // measuring units
    QStringList varList;// list of items
    int     MaxLen;     // string max length
    int     Size;       // Text size occupation % on dialog for description
} MENU_VAR;

typedef struct MENU_VAR_LIST_T
{

  int varNum;   // tells how many vars are in list
  int index;    // tells the actual position for the menu
  MENU_VAR menuVar[MAX_VARS_IN_MENU_LIST];

} MENU_VAR_LIST;


/* menu structures */

#define MAX_VIEWS_IN_MENU_LIST       50

typedef struct MENU_VIEW_T
{
    QString RefMenu;    // ancestor's name
    QString MenuName;   // menu name
    QString MenuLabel;  // shown menu name
    QString Background; // background image with path
    QString CmdID;      // Instant values request keyname
    QString ParserName; // Associated parser name
    int     ReqTime;    // Request Time Interval for instant values data
    int     itemID;     // ID number for the view
} MENU_VIEW;

typedef struct MENU_VIEW_LIST_T
{

  int viewNum; // tells how many vars are in list
  MENU_VIEW menuView[MAX_VIEWS_IN_MENU_LIST];

} MENU_VIEW_LIST;


// GRAHIC STRUCTURES

#define GPI_NUM             6
#define GPO_NUM             4

#define TCPIP_NUM           4
#define UDP_NUM             4
#define RS232_NUM           2

#define GRP_NUM             6
#define DI_NUM              5
#define RT_NUM              65

#define MPX_OUT_NUM         5
#define MPX_PROC_TYPE_NUM   8
#define MPX_PROC_INFO_NUM   8

#define AUDIO_SRC_NAME_NUM  6
#define AUDIO_SOURCE_NUM    4
#define AUDIO_GEN_NUM       8
#define AUDIO_PROC_NUM      10

#define FFT_LINES           64


typedef struct SYSTEM_MODULE_DATA_T
{

    // SYSTEM
    int Email;
    int Scheduler;
    int Logs;
    int ExtRds;

    // EXTERNAL FEED
    int Rss;
    int Rest;
    int Samba;
    int Gps;

    // ALARMS
    int alarmStatus;
    int alarmDesc[11];

    // DATE & TIME
    int GpsTime;
    int NtpTime;
    int ClockSync;
    int Date[8];
    int Time[8];

    // GPIO
    int Gpi[GPI_NUM];
    int Gpo[GPO_NUM];

    // UECP COM PORTS
    int Rs232[RS232_NUM];
    int TcpIp[TCPIP_NUM];
    int Udp[UDP_NUM];
    int Snmp;

    bool dataReady;
} SYSTEM_MODULE_DATA;


typedef union
{
    struct
       {
        unsigned short int noAudio:1;
        unsigned short int noPilot:1;
        unsigned short int noRds:1;
        unsigned short int Noise:1;
        unsigned short int PI:1;
        unsigned short int Data:1;
        unsigned short int b6:1;
        unsigned short int b7:1;
        unsigned short int b8:1;
        unsigned short int b9:1;
        unsigned short int b10:1;
        unsigned short int b11:1;
        unsigned short int b12:1;
        unsigned short int b13:1;
        unsigned short int b14:1;
        unsigned short int b15:1;
       } bit;
    unsigned short int AuxAlarm;

} MPX_AUX_STATUS;

/*
typedef union
        {
         struct
            {
             unsigned b0:1;
             unsigned b1:1;
             unsigned b2:1;
             unsigned b3:1;
             unsigned b4:1;
             unsigned b5:1;
             unsigned b6:1;
             unsigned b7:1;
             unsigned b8:1;
             unsigned b9:1;
             unsigned b10:1;
             unsigned b11:1;
             unsigned b12:1;
             unsigned b13:1;
             unsigned b14:1;
             unsigned b15:1;
            } bit;
         struct
            {
             BYTE L:8;
             BYTE H:8;
            } byte;
         unsigned short int word;
        }         WORD_BIT;

*/

typedef struct MPX_SPECTRUM_T
{
    int Mpx_LplusR_Lev;
    int Mpx_LminusR_Lo_Lev;
    int Mpx_LminusR_Hi_Lev;
    int Mpx_Pilot_Lev;
    int Mpx_Rds_Lev;
    int Mpx_PilotProt_Lev;
    int Mpx_RdsProt_Lev;
    int Mpx_OutBand_Noise_Lev;
    int Mpx_Itu_Lev;

} MPX_SPECTRUM;

typedef struct MPX_MODULE_DATA_T
{
    // MPX SIGNAL
    int MpxLev1[MPX_OUT_NUM];
    int MpxLev2[MPX_OUT_NUM];
    int LimPresence;
    int PilotPresence;
    int Sideband; // SSB or DSB
    int Out1Lev;
    int Out2Lev;

    // MPX_SCA INPUT
    MPX_AUX_STATUS Aux1Status;
    MPX_AUX_STATUS Aux2Status;
    int Input1Lev;
    int Input2Lev;

    // MPX-CHANGEOVER
    int MpxSelector;
    int IntPresence;
    int Aux1Presence;
    int Aux2Presence;
    int RdsSrc;
    int Process;

    // MPX PROCESSING
    int MpxType[MPX_PROC_TYPE_NUM];
    int MpxInfo[MPX_PROC_INFO_NUM];

    // ON-AIR_MPX SPECTRUM
    MPX_SPECTRUM OnAirSpectrum;

    // AUX-1 MPX SPECTRUM
    MPX_SPECTRUM Aux1Spectrum;

    bool dataReady;

} MPX_MODULE_DATA;

#define RDS_SYNC_INT        0
#define RDS_SYNC_EXT        1

#define RDS_LEV_MV          0
#define RDS_LEV_DB          1

typedef struct RDS_MODULE_DATA_T
{
    // RDS SIGNAL
    int SyncInSrc; // INT or EXT
    int SyncLock;
    int RdsLev[5];
    int RdsLevUnits; // mV or dB
    int RdsPhase[8];

    // PTYN-PTY
    int PTY[9];
    int PTYN[9];

    // ON AIR GROUPS
    int Grp[GRP_NUM][4];

    // PI-DI-PS-DS
    int DS;
    int PS[9];
    int PI[5];
    int DI[DI_NUM];

    // RT & SERVICES
    int RT[RT_NUM];
    int Tp;
    int Ta;
    int Ms;
    int Ct;
    int Af;
    int Rt;
    int RtPlus;
    int Tmc;
    int Eon;
    int EonTa;
    int RdsOn;
    int Rds20;

    bool dataReady;

} RDS_MODULE_DATA;

typedef struct AUDIO_SOURCE_T
{
    int SourceName[AUDIO_SRC_NAME_NUM];
    int SrcFault;
    int SrcLeftLev;
    int SrcRightLev;
} AUDIO_SOURCE;

typedef struct AUDIO_MODULE_DATA_T
{
    // SOURCES & PRIORITY
    AUDIO_SOURCE Source[AUDIO_SOURCE_NUM];

    //AUDIO CHANGEOVER
    int SourcePresence[AUDIO_SOURCE_NUM];
    int OnAirLevLeft;
    int OnAirLevRight;
    int AudioChangeoverSel;

    // Monitor Audio
    int MonLevLeft;
    int MonLevRight;
    int MonSelector;
    int MonSrcPresence[AUDIO_SOURCE_NUM+3];

    // Audio Genereator
    int GenOnOff;       // OFF, ON
    int GenWaveform;    // SINE, NOISE
    int GenMode;        // L, R, L+R, L-R
    int GenFreq[AUDIO_GEN_NUM];
    int GenModeTxt[AUDIO_GEN_NUM];

    // Audio Processing
    int Emphasis;       // OFF, 50uS, 75uS
    int ProcTyp[AUDIO_PROC_NUM];
    int ProcInf[AUDIO_PROC_NUM];

    // Spectrum
    int AudioSpectrum[FFT_LINES];
    int AudioRmsLeft;
    int AudioRmsRight;
    int AudioPeakLeft;
    int AudioPeakRight;

    bool dataReady;

} AUDIO_MODULE_DATA;


typedef struct TARGET_DATA_T
{
    SYSTEM_MODULE_DATA System;
    MPX_MODULE_DATA Mpx;
    RDS_MODULE_DATA Rds;
    AUDIO_MODULE_DATA Audio;
} TARGET_DATA;

#endif // COMMON_H
