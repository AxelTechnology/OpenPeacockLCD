#include "graphichandler.h"


#ifdef DEBUG_MODE
    #include <QDebug>
#endif // DEBUG_MODE


extern MENU_VIEW_LIST menuViewList;
extern TARGET_DATA  TargetData;
extern JsonHandler ivInput;
extern JsonHandler ivOutput;

GraphicHandler::GraphicHandler(QObject *parent) : QObject(parent)
{
    // connect the timer's timeout signal to a private slot
     connect(&handlerTimer, SIGNAL(timeout()),
             this, SLOT(updateHandlerTimer()));

     this->handlerTimer.setInterval(HANDLER_TIMER_BASE);
}

void GraphicHandler::initHandler(JsonHandler *jshInput, JsonHandler *jshOutput)
{
   this->Input = jshInput;
   this->Output = jshOutput;
}

void GraphicHandler::setHandler(int timerInterval)
{
    pollingInterval = timerInterval;
}

void GraphicHandler::startHandler()
{
   this->handlerTimer.start();
}

void GraphicHandler::stopHandler()
{
    this->handlerTimer.stop();
}


void GraphicHandler::updateHandlerTimer()
{
    static int ghStatus = HANDLER_STATUS_INIT_START;
    static int checkCounter = 0;
    static int readCounter = 0;
    int status;
    static int loopCounter = 0;

#ifdef DEBUG_MODE
   static int oldStatus = -1;
   //static int x = 0;

   //qDebug() << "Activity counter: " << ++x;

   if (oldStatus != ghStatus) {
       oldStatus = ghStatus;
       qDebug() << "*** Graphic Handler status: " << oldStatus;
   }
#endif // DEBUG_MODE


   switch(ghStatus)
   {
        case HANDLER_STATUS_INIT_START:  // first time start
           // send menu request
           menuViewList.menuView[menuViewIndex].ParserName = JSON_IV_SYSTEM_PARSER; // "pollo test forced";

           ivOutput.JsonWriteCmd(JSON_READ_CMD, JSON_READ_QT_MOD_NAME,  menuViewList.menuView[menuViewIndex].CmdID);
           ghStatus = HANDLER_STATUS_INIT_CHECK_FILE;
        break;

        case HANDLER_STATUS_INIT_CHECK_FILE:
           // if there is no data or data was unchanged, do nothing
           status = ivInput.checkFile();
           if(status == JSON_FILE_NOT_EXISTS) break;

           if(status == JSON_FILE_CHECK_CHANGED) {
               checkCounter = 0;
               ghStatus = HANDLER_STATUS_INIT_READ_DATA;
           }
           else {
               checkCounter++;
               if (checkCounter >= MAX_CHECK_FILE_RETRIES) {
                   checkCounter = 0;
                   ghStatus = HANDLER_STATUS_ERROR;
               }
           }

        break;

        case HANDLER_STATUS_INIT_READ_DATA:
           ivInput.Read_InstantValues(&iv_Obj);

           if (ivInput.getStatus() != JSON_OK) {
               readCounter++;
               if(readCounter >= MAX_READ_FILE_RETRIES) {
                   readCounter = 0;
                   ghStatus = HANDLER_STATUS_ERROR;
                   setStatus(HANDLER_CMD_ERROR);
               }
               else {
                ghStatus = HANDLER_STATUS_INIT_START; // new request
                break;
               }
           }

           readCounter = 0;
           ghStatus = HANDLER_STATUS_SET_DATA;
        break;

        case HANDLER_STATUS_SET_DATA:
           emit(parseModuleDataReq());
           ghStatus=HANDLER_STATUS_WAIT_LOOP;
        break;

        case HANDLER_STATUS_WAIT_LOOP:
           loopCounter+=HANDLER_TIMER_BASE;
           if(loopCounter >= pollingInterval){
               loopCounter = 0;
               ghStatus = HANDLER_STATUS_INIT_START;
           }
        break;

        case HANDLER_STATUS_ERROR:
        // display should show splash screen

        break;

   }
}


int GraphicHandler::getStatus()
{
    return status;
}

void GraphicHandler::setStatus(int st)
{
    status = st;
}

void GraphicHandler::setCurrentViewIndex(int index)
{
    menuViewIndex = index;
}



void GraphicHandler::parseModuleData()
{
    QJsonValue ivVal = iv_Obj.value(QString(JSON_READ_IV_KEY_SYSTEM));
    QJsonObject ivTmpObj = ivVal.toObject();
    QJsonArray varItems;
    int i=0, pos1=0, pos2=0;

    if(menuViewList.menuView[menuViewIndex].ParserName == JSON_IV_SYSTEM_PARSER)
    {
      // systemValues
      /*
      "InstantValues": {
            "systemValues": {
              "Email": false,
              "Scheduler": true,
              "Logs": true,
              "ExtRdsMode": false,

              "RssFeed": false,
              "Rest": true,
              "Samba": false,
              "Gps": true,

              "SystemOk": false,
              "SYS_INFO": [83, 89, 83, 84, 69, 77, 32, 79, 75, 32],

              "GpsTime": false,
              "Ntp": false,
              "Clock": false,
              "DATE_TIME": [50, 56, 48, 51, 49, 55, 48, 56, 53, 55, 48, 49],

              "GPI_STATUS": [0, 0, 0, 0, 0, 0],
              "GPO_STATUS": [0, 0, 0, 0],

              "SER_CON": [0, 0],
              "TCP_CON": [0, 0, 0, 0],
              "UDP_CON": [0, 0, 0, 0]
              "SNMP_CON": [0],

              "Memory_Usage": 21,
              "CPU_Usage": 24,
              "CPU_Temp": 25.0,
            }
        }
        */


        TargetData.System.Email         = (int)((ivTmpObj.value("Email")).toBool());
        TargetData.System.Scheduler     = (int)((ivTmpObj.value("Scheduler")).toBool());
        TargetData.System.Logs          = (int)((ivTmpObj.value("Logs")).toBool());
        TargetData.System.ExtRds        = (int)((ivTmpObj.value("ExtRdsMode")).toBool());

        TargetData.System.Rss           = (int)((ivTmpObj.value("RssFeed")).toBool());
        TargetData.System.Rest          = (int)((ivTmpObj.value("Rest")).toBool());
        TargetData.System.Samba         = (int)((ivTmpObj.value("Samba")).toBool());
        TargetData.System.Gps           = (int)((ivTmpObj.value("Gps")).toBool());

        TargetData.System.alarmStatus   = (int)((!(ivTmpObj.value("SystemOk")).toBool()));
        TargetData.System.GpsTime       = (int)((ivTmpObj.value("GpsTime")).toBool());
        TargetData.System.NtpTime       = (int)((ivTmpObj.value("Ntp")).toBool());
        TargetData.System.ClockSync     = (int)((ivTmpObj.value("Clock")).toBool());

        varItems = ivTmpObj.value("SYS_INFO").toArray();
        for(i=0; i< varItems.count(); ++i)
        {
          TargetData.System.alarmDesc[i] = varItems.at(i).toInt();
        }
        TargetData.System.alarmDesc[i] = 0;

        varItems = ivTmpObj.value("DATE_TIME").toArray();
        for(i=0; i< varItems.count(); ++i)
        {
            if (i<6)
            {
                TargetData.System.Date[pos1++] = varItems.at(i).toInt();
                if(i && !(i%2)) TargetData.System.Date[pos1++] = 45; // char '-'
            }
            else
            {
                TargetData.System.Time[pos2++] = varItems.at(i).toInt();
                if(i!=6 && !(i%2)) TargetData.System.Time[pos2++] = 58; // char ':'
            }
        }
        TargetData.System.Date[6]=0;
        TargetData.System.Time[6]=0;

        varItems = ivTmpObj.value("GPI_STATUS").toArray();
        for(i=0; i< varItems.count(); ++i)
        {
          TargetData.System.Gpi[i] = varItems.at(i).toInt();
        }

        varItems = ivTmpObj.value("GPO_STATUS").toArray();
        for(i=0; i< varItems.count(); ++i)
        {
          TargetData.System.Gpo[i] = varItems.at(i).toInt();
        }

        varItems = ivTmpObj.value("SER_CON").toArray();
        for(i=0; i< varItems.count(); ++i)
        {
          TargetData.System.Rs232[i] = varItems.at(i).toInt();
        }

        varItems = ivTmpObj.value("TCP_CON").toArray();
        for(i=0; i< varItems.count(); ++i)
        {
          TargetData.System.TcpIp[i] = varItems.at(i).toInt();
        }

        varItems = ivTmpObj.value("UDP_CON").toArray();
        for(i=0; i< varItems.count(); ++i)
        {
          TargetData.System.Udp[i] = varItems.at(i).toInt();
        }

        varItems = ivTmpObj.value("SNMP_CON").toArray();
        TargetData.System.Snmp = varItems.at(0).toInt();

        TargetData.System.dataReady = true;

        return;
    }

    if(menuViewList.menuView[menuViewIndex].ParserName == JSON_IV_RDS_PARSER)
    {
        // rdsValues
        /*
        "rdsValues": {
            "RdsLevel": [48, 48, 48, 53, 48],
            "RdsLevelUnit": [1, 0],
            "RdsPhase": [48, 46, 48, 32, 32],
            RdsPhaseUnit": [1],
            "RdsSync": [1, 0, 0],
            "SERVICES_CON": [0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0],
            "DS": [49],
            "PTY": [78, 79, 32, 80, 82, 79, 71, 82],
            "PTYN": [32, 32, 32, 45, 45, 45, 45, 45],
            "PI": [53, 48, 48, 48],
            "GROUPS_ONAIR": [32, 48, 65, 32, 50, 65, 49, 48, 65, 32, 32, 32, 32, 32, 32, 32, 32, 32],
            "DI": [83, 84],
            "PS": [80, 83, 78, 65, 77, 69, 48, 48],
            "RT": [32, 32, 32, 32, 32, 32, 32, 60, 32, 78, 79, 32, 82, 84, 32, 68, 65, 84, 65, 32, 62, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32],

            "DSTAT_CHARTS": [33, 33, 0],
            "DSTAT1": [" 0A", "33%", "331", "91"],
            "DSTAT2": [" 2A", "33%", "331", "91"],
            "DSTAT3": [" 4A", "0%", "7", "2"]}
        */

        varItems = ivTmpObj.value("RdsSync").toArray();
        i = varItems.at(0).toInt();
        TargetData.Rds.SyncInSrc = (i?RDS_SYNC_EXT:RDS_SYNC_INT);
        i = varItems.at(2).toInt();
        TargetData.Rds.SyncLock = i;

        varItems = ivTmpObj.value("RdsLevel").toArray();
        for(i=0; i< varItems.count(); ++i)
        {
         TargetData.Rds.RdsLev[i] = varItems.at(i).toInt();
        }
        TargetData.Rds.RdsLev[i] = 0;

        varItems = ivTmpObj.value("RdsLevelUnit").toArray();
        i = varItems.at(0).toInt();
        TargetData.Rds.RdsLevUnits = (i?RDS_LEV_MV:RDS_LEV_DB);

        varItems = ivTmpObj.value("RdsPhase").toArray();
        for(i=0; i< varItems.count(); ++i)
        {
         TargetData.Rds.RdsPhase[i] = varItems.at(i).toInt();
        }
        TargetData.Rds.RdsPhase[i] = 0;

        varItems = ivTmpObj.value("DS").toArray();
        TargetData.Rds.DS = varItems.at(0).toInt();

        varItems = ivTmpObj.value("PTY").toArray();
        for(i=0; i< varItems.count(); ++i)
        {
         TargetData.Rds.PTY[i] = varItems.at(i).toInt();
        }
        TargetData.Rds.PTY[i] = 0;

        varItems = ivTmpObj.value("PTYN").toArray();
        for(i=0; i< varItems.count(); ++i)
        {
         TargetData.Rds.PTYN[i] = varItems.at(i).toInt();
        }
        TargetData.Rds.PTYN[i] = 0;

        varItems = ivTmpObj.value("PI").toArray();
        for(i=0; i< varItems.count(); ++i)
        {
         TargetData.Rds.PI[i] = varItems.at(i).toInt();
        }
        TargetData.Rds.PI[i] = 0;

        varItems = ivTmpObj.value("DI").toArray();
        for(i=0; i< varItems.count(); ++i)
        {
         TargetData.Rds.DI[i] = varItems.at(i).toInt();
        }
        TargetData.Rds.DI[i] = 0;

        varItems = ivTmpObj.value("PS").toArray();
        for(i=0; i< varItems.count(); ++i)
        {
         TargetData.Rds.PS[i] = varItems.at(i).toInt();
        }
        TargetData.Rds.PS[i] = 0;

        varItems = ivTmpObj.value("RT").toArray();
        for(i=0; i< varItems.count(); ++i)
        {
         TargetData.Rds.PS[i] = varItems.at(i).toInt();
        }
        TargetData.Rds.PS[i] = 0;

        varItems = ivTmpObj.value("GROUPS_ONAIR").toArray();

        pos1=0; pos2=0;
        for(i=0; i< varItems.count(); ++i)
        {
            if(i && !(i%3)) {
                TargetData.Rds.Grp[pos1][pos2] = 0;
                pos1++;
                pos2=0;
            }
            TargetData.Rds.Grp[pos1][pos2++] = varItems.at(i).toInt();

        }
        TargetData.Rds.Grp[pos1][pos2] = 0;

        varItems = ivTmpObj.value("SERVICES_CON").toArray();

        TargetData.Rds.Tp      = varItems.at(0).toInt();
        TargetData.Rds.Ta      = varItems.at(1).toInt();
        TargetData.Rds.Ms      = varItems.at(2).toInt();
        TargetData.Rds.Ct      = varItems.at(3).toInt();
        TargetData.Rds.Af      = varItems.at(4).toInt();
        TargetData.Rds.Rt      = varItems.at(5).toInt();
        TargetData.Rds.RtPlus  = varItems.at(6).toInt();
        TargetData.Rds.Tmc     = varItems.at(7).toInt();
        TargetData.Rds.Eon     = varItems.at(8).toInt();
        TargetData.Rds.EonTa   = varItems.at(9).toInt();
        TargetData.Rds.RdsOn   = varItems.at(10).toInt();
        TargetData.Rds.Rds20   = varItems.at(11).toInt();

        TargetData.Rds.dataReady = true;

        return;
    }
}
