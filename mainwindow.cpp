#include "mainwindow.h"
#include "ui_common.h"
#include "ui_mainwindow.h"
#include "CustomDialogs/menu.h"
#include "Threads/signalthread.h"
#include "Threads/graphicthread.h"
#include "json_common.h"
#include "jsonhandler.h"
#include "leds.h"
#include "graphichandler.h"

#ifdef DEBUG_MODE
    #include <QDebug>
    #include <QTime>
    #include <QDate>
#endif // DEBUG_MODE

// instantiate thread for Encoder
SignalThread encoderThread;

// instatiate thread for graphic design
GraphicThread graphicThread;

// instantiate struct lists
MENU_VAR_LIST  menuVarList;
MENU_VIEW_LIST menuViewList;

// instantiate graphic data structure
TARGET_DATA  TargetData;


// instantiate files
JsonHandler Input("out.json");
JsonHandler Output("in.json");
JsonHandler ivInput("iv_out.json");
JsonHandler ivOutput("iv_in.json");

Leds encoderWheel;

GraphicHandler graphicHandler;

/**
 * @brief encEvState holds event status changes
 */
EVENT_STATE encEvState;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Frameless window
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    // connect the timer's timeout signal to a private slot
    connect(&mainTimer, SIGNAL(timeout()),
             this, SLOT(updateMainTimer()));

    // set interval for the timer
    mainTimer.setInterval(MAIN_THREAD_INTERVAL_MS); // 20 milliseconds

    animationTimer.setInterval(50);

    connect(&animationTimer, SIGNAL(timeout()),
             this, SLOT(updateAnimationTimerEvent()));


    // set write timer
    // connect the timer's timeout signal to a private slot
    connect(&writeTimer, SIGNAL(timeout()),
             this, SLOT(updateWriteTimer()));

    // set interval for the timer
    writeTimer.setInterval(JSON_WRITE_FILE_TIMER_INTERVAL); // milliseconds

    ui->mpxSpectrumPanel->installEventFilter(this);
    ui->mpxPanel->installEventFilter(this);
    ui->rdsPanel->installEventFilter(this);
    ui->systemPanel->installEventFilter(this);
    ui->audioPanel->installEventFilter(this);
    ui->audioSpectrumPanel->installEventFilter(this);

    mainMenuInit();

    // connect encoder signals to paired event slots...
    connect(&encoderThread, SIGNAL(encoderPressUp()),
            this, SLOT(encoderPressUpHandler()));

    connect(&encoderThread, SIGNAL(encoderPressDown()),
            this, SLOT(encoderPressDownHandler()));

    connect(&encoderThread, SIGNAL(encoderRightRot()),
            this, SLOT(encoderRightRotHandler()));

    connect(&encoderThread, SIGNAL(encoderLeftRot()),
            this, SLOT(encoderLeftRotHandler()));

    connect(&encoderThread, SIGNAL(encoderClick()),
            this, SLOT(encoderClickedHandler()));

    connect(&encoderThread, SIGNAL(encoderDoubleClick()),
            this, SLOT(encoderDblClickHandler()));

    connect(&encoderThread, SIGNAL(encoderLongPress()),
            this, SLOT(encoderLongPressHandler()));

    systemReady = false;


    // load fonts

    QFontDatabase database;

    int id = database.addApplicationFont(":/Fonts/FreeMono.ttf");

    if (QFontDatabase::applicationFontFamilies(id).count() <= 0) {

        #ifdef DEBUG_MODE
        qDebug() << "Error loading Free Mono font from resources.";
        #endif
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::getSystemReady()
{
    return systemReady;
}

void MainWindow::setSystemReady(bool cond)
{
    systemReady = cond;
}

#ifdef UI_VIRTUAL_ENCODER

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // *****  Keyboard encoder emulator  *****
    //
    //      '<- arrow' = ROT LEFT
    //      '-> arrow' = ROT RIGHT
    //      'P' = PRESS DOWN
    //      'C' = CLICK
    //      'D' = DOUBLE CLICK
    //      'L' = LONG PRESS
    //
    // ***************************************

    if(event->key() == Qt::Key_Left)
        {
            encoderLeftRotHandler();
            return;
        }

    if(event->key() == Qt::Key_Right)
        {
            encoderRightRotHandler();
            return;
        }

    if(event->key() == Qt::Key_P)
        {
            encoderPressDownHandler();
            return;
        }

    if(event->key() == Qt::Key_C)
        {
            encoderClickedHandler();
            return;
        }

    if(event->key() == Qt::Key_D)
        {
            encoderDblClickHandler();
            return;
        }

    if(event->key() == Qt::Key_L)
        {
            encoderLongPressHandler();
            return;
        }

}

#endif // UI_VIRTUAL_ENCODER

void MainWindow::mainMenuInit()
{
    oi_menuDialog = new menuDialog(this);

    initEvState();

    initGraphicObjects();

    encoderWheel.Set_Wheel(LED_OFF);

    // set initial view index
    mainMenuStateIndex = 0;

    // start state machine
    mainTimer.start();

    // start write check timer
    writeTimer.start();

    //setMainView();
    paintFlag = false;
    animationTimer.start();

    // set Graphic Handler
    graphicHandler.initHandler(&ivInput, &ivOutput);
    graphicHandler.setHandler(menuViewList.menuView[mainMenuStateIndex].ReqTime);
    connect(&graphicHandler, SIGNAL(parseModuleDataReq()),
            &graphicHandler, SLOT(parseModuleData()));
    //graphicHandler.startHandler();
}

void MainWindow::updateMainTimer()
{
    static int mainStatus = MENU_STATUS_INIT_START;
    static int counter = 0;
    int status;
    static int DialogMenuCounter = DIALOG_MENU_OPEN_TIMER_COUNT;

 #ifdef DEBUG_MODE
    static int oldStatus = -1;
    //static int x = 0;

    //qDebug() << "Activity counter: " << ++x;

    if (oldStatus != mainStatus) {
        oldStatus = mainStatus;
        qDebug() << "Actual status: " << oldStatus;
    }
 #endif // DEBUG_MODE

    if(!(MainWindow::isActiveWindow())) MainWindow::activateWindow();

    // finite state machine
    switch(mainStatus)
    {
        case MENU_STATUS_INIT_START:  // first time start
        // wheel leds power off
        encoderWheel.Set_Wheel(LED_OFF);

        // send menu request
            Output.JsonWriteCmd(JSON_READ_MENU_CMD, JSON_READ_MENU_ROOT);
            mainStatus = MENU_STATUS_INIT_CHECK_FILE;
        break;

        case MENU_STATUS_INIT_CHECK_FILE:

            encoderWheel.Set_Wheel(LED_OFF);

            // if there is no data or data was unchanged, do nothing
            status = Input.checkFile();
            if(status == JSON_FILE_NOT_EXISTS) break;

            if(status == JSON_FILE_CHECK_CHANGED) {
                counter = 0;
                mainStatus = MENU_STATUS_INIT_READ_DATA;
            }
            else {
                counter++;
                if (counter >= MAX_CHECK_FILE_RETRIES) {
                    counter = 0;
                    mainStatus = MENU_STATUS_INIT_START;
                }
            }

        break;

        case MENU_STATUS_INIT_READ_DATA:
            Input.ReadData();
            if (Input.getStatus() != JSON_OK) {
                mainStatus = MENU_STATUS_INIT_CHECK_FILE;
                break;
            }

            counter = 0;
            mainStatus = MENU_STATUS_INIT_SET_MAIN_VIEW;
        break;

        case MENU_STATUS_INIT_SET_MAIN_VIEW:
            initMainView();
            //setSystemReady(true);
            //this->show();
            setMainView();
            /*
            graphicHandler.setHandler(menuViewList.menuView[mainMenuStateIndex].ReqTime);
            graphicHandler.startHandler();
            connect(&graphicHandler, SIGNAL(GraphicHandler::parseModuleDataReq()),
                    &graphicHandler, SLOT(GraphicHandler::parseModuleData()));
            */
            DialogMenuCounter = DIALOG_MENU_OPEN_TIMER_COUNT;

            mainStatus = MENU_BASE_DIALOG_CLOSE;
        break;

        case MENU_BASE_DIALOG_CLOSE: // base for all running state with dialog menu closed

            // no event, no party!
            if(!isEncTouched()) break;

            if(isEncRightRot()) {
                // wipe encoder event list
                initEvState();

                // rotate between views
                mainMenuStateIndex++;
                if(mainMenuStateIndex>menuViewList.viewNum) mainMenuStateIndex=0;
                //setMainView();

                encoderWheel.Wheel_Rotate_Right();
                break;
            }

            if (isEncLeftRot()) {
                // wipe encoder event list
                initEvState();

                // rotate between views
                mainMenuStateIndex--;
                if(mainMenuStateIndex<0) mainMenuStateIndex=menuViewList.viewNum;
                //setMainView();

                encoderWheel.Wheel_Rotate_Left();
                break;
            }

            if (isEncPressDown() || isEncClick()) {
                // wipe encoder event list
                initEvState();

                // open dialog menu
                mainStatus = MENU_BASE_DIALOG_CLOSE_REQ_MENU_DATA;
                break;
            }

            initEvState();
        break;

        case MENU_BASE_DIALOG_CLOSE_REQ_MENU_DATA:
            if(!(Output.getWriteStatus()))
            {

                encoderWheel.Set_Wheel(LED_ON);

                // send specific menu request
                Output.JsonWriteCmd(JSON_READ_MENU_CMD, menuViewList.menuView[mainMenuStateIndex].MenuName);
                mainStatus = MENU_BASE_DIALOG_CLOSE_CHECK_FILE;
            }
        break;

        case MENU_BASE_DIALOG_CLOSE_CHECK_FILE:
            if(Input.checkFile() == JSON_FILE_CHECK_CHANGED) {
                counter = 0;
                mainStatus = MENU_BASE_DIALOG_CLOSE_READ_FILE;
            }
            else {
                counter++;
                if (counter >= MAX_CHECK_FILE_RETRIES) {
                    counter = 0;
                    mainStatus = MENU_BASE_DIALOG_CLOSE_REQ_MENU_DATA;
                }
            }
        break;

        case MENU_BASE_DIALOG_CLOSE_READ_FILE:
            Input.ReadData();
            if (Input.getStatus() != JSON_OK)
                mainStatus = MENU_STATUS_INIT_CHECK_FILE; //MENU_BASE_DIALOG_CLOSE;
            // no items, no party
            if(menuVarList.varNum)
                mainStatus = MENU_SETTINGS_DIALOG_OPEN;
                else
                mainStatus = MENU_STATUS_INIT_CHECK_FILE; //MENU_BASE_DIALOG_CLOSE;
        break;

        case MENU_SETTINGS_DIALOG_OPEN:
            encoderWheel.Set_Wheel(LED_ON);

            oi_menuDialog->InitMenuView();

            if(!(oi_menuDialog->isVisible()))
                this->showMenuDialog();

            mainStatus = MENU_SETTINGS_DIALOG_OPENED;
        break;

        case MENU_SETTINGS_DIALOG_OPENED:  // base for all running state with dialog menu opened
            // no event, no party!
            if(!isEncTouched()) {
                DialogMenuCounter--;
                if(DialogMenuCounter<=0) {
                    mainStatus = MENU_SETTINGS_DIALOG_CLOSE_REQ;
                    DialogMenuCounter = DIALOG_MENU_OPEN_TIMER_COUNT;
                }
                break;
            }

            DialogMenuCounter = DIALOG_MENU_OPEN_TIMER_COUNT;

            if(isEncRightRot()) {
                // wipe encoder event list
                initEvState();

                // rotate between vars
                if(menuVarList.index < menuVarList.varNum) menuVarList.index++;
                oi_menuDialog->UpdateMenuView();
                break;
            }

            if (isEncLeftRot()) {
                // wipe encoder event list
                initEvState();

                // rotate between vars
                if(menuVarList.index >= 0) menuVarList.index--;
                oi_menuDialog->UpdateMenuView();
                break;
            }

            if(isEncDblClick()) {
                // wipe encoder event list
                initEvState();

                // start process to close dialog
                mainStatus = MENU_SETTINGS_DIALOG_CLOSE_REQ;
                break;
            }

            if(isEncHold()) {
                // wipe encoder event list
                initEvState();

                // toggle dialog position
                this->moveMenuDialog();
                break;
            }

            if(isEncClick() || isEncPressDown()) {
                // wipe encoder event list
                initEvState();

                // if ESC is selected close dialog
                if(menuVarList.index<0) {
                    menuVarList.index = 0;
                    mainStatus = MENU_SETTINGS_DIALOG_CLOSE_REQ;
                    break;
                }

                // temp workaround
                if(oi_menuDialog->is_editing_a_String()) break;

                if(oi_menuDialog->editVar_getPermissions() == (QString) JSON_VAR_MODE_READONLY) break;

                // start process to edit var
                mainStatus = MENU_SETTINGS_EDIT_MODE_INIT;
                break;
            }

            initEvState();

        break;

        case MENU_SETTINGS_DIALOG_CLOSE_REQ:
            // send specific menu request
            Output.JsonWriteCmd(JSON_READ_MENU_CMD, JSON_READ_MENU_UP);
            mainStatus = MENU_STATUS_INIT_CHECK_FILE;
            oi_menuDialog->close();
        break;

        case MENU_SETTINGS_EDIT_MODE_INIT: // enter in EDIT mode

            encoderWheel.Wheel_Blink(BLINK_START);

            // call initEditMode
            oi_menuDialog->InitEditMode();
            if(oi_menuDialog->is_editing_a_String())
                mainStatus = MENU_SETTINGS_EDIT_MODE_STRING;
            else
                mainStatus = MENU_SETTINGS_EDIT_MODE_BASE;
        break;

        case MENU_SETTINGS_EDIT_MODE_BASE: // base status for EDIT mode
            // poll encoder

            // no event, no party!
            if(!isEncTouched()) break;

            if(isEncRightRot()) {
                // wipe encoder event list
                initEvState();

                // call INC method on var
                oi_menuDialog->editVar_Inc();
                break;
            }

            if (isEncLeftRot()) {
                // wipe encoder event list
                initEvState();

                // call DEC method on var
                oi_menuDialog->editVar_Dec();
                break;
            }

            if (isEncPressDown() || isEncClick()) {
                // wipe encoder event list
                initEvState();

                if (oi_menuDialog->editVar_isChanged()) {
                    mainStatus = MENU_SETTINGS_EDIT_MODE_SAVE_DLG;
                }
                else mainStatus = MENU_SETTINGS_EDIT_MODE_CLOSE;
                break;
            }

            initEvState();
        break;

        case MENU_SETTINGS_EDIT_MODE_CLOSE: // no changes happened. Leave EDIT mode and fall back to variable navigation

            encoderWheel.Wheel_Blink(BLINK_STOP);
            // wipe encoder event list
            initEvState();

            if(!(Output.getWriteStatus())) {
                // send specific menu request
                Output.JsonWriteCmd(JSON_READ_MENU_CMD, JSON_READ_MENU_CURRENT);
                mainStatus = MENU_BASE_DIALOG_CLOSE_CHECK_FILE;
            }

        break;

        case MENU_SETTINGS_EDIT_MODE_SAVE_DLG: // EDIT SAVE Confirm/Cancel
            // wipe encoder event list
            initEvState();

            oi_menuDialog->editShowSaveDialog();
            mainStatus = MENU_SETTINGS_EDIT_MODE_SAVE_CHOOSE;
        break;

        case MENU_SETTINGS_EDIT_MODE_SAVE_CHOOSE: // wait for SAVE / ESC choose
            // no event, no party!
            if(!isEncTouched()) break;

            if(isEncRightRot()) {
                // wipe encoder event list
                initEvState();

                // call INC method on var
                oi_menuDialog->toggleEscSaveStatus();
                break;
            }

            if (isEncLeftRot()) {
                // wipe encoder event list
                initEvState();

                // call DEC method on var
                oi_menuDialog->toggleEscSaveStatus();
                break;
            }

            if (isEncPressDown() || isEncClick()) {

                encoderWheel.Wheel_Blink(BLINK_STOP);

                // wipe encoder event list
                initEvState();

                if (oi_menuDialog->getConfirmState()) {

                    oi_menuDialog->editVar_Confirm();
                    mainStatus = MENU_SETTINGS_EDIT_MODE_CLOSE;
                }
                else mainStatus = MENU_SETTINGS_EDIT_MODE_SAVE_UNDO;
                break;
            }

            initEvState();
        break;

        case MENU_SETTINGS_EDIT_MODE_SAVE_UNDO:
            // wipe encoder event list
            initEvState();

            // set back previous value
            oi_menuDialog->editVar_Undo();

            mainStatus = MENU_SETTINGS_EDIT_MODE_CLOSE;
        break;


        case MENU_COMM_ERROR:
            // show splash screen while python (or RT process) is dead
            ui->stackedWidget->setCurrentWidget(ui->splashPanel);
        break;

        default:
            ui->stackedWidget->setCurrentWidget(ui->splashPanel);
            mainStatus = MENU_STATUS_INIT_START;
        break;
    }

    if(mainStatus >= MENU_BASE_DIALOG_CLOSE && mainStatus < MENU_COMM_ERROR) setMainView();

}


void MainWindow::moveMenuDialog()
{
   /*
    * try to move background...

    static int l_iDialogHeight = 0;

        if (l_iDialogHeight == 0)
        {
            l_iDialogHeight = 1;

            this->move(0, - 64);
        }
        else {
            l_iDialogHeight = 0;

            this->move(0, 0);
        }
    */

    int l_iDialogHeight = oi_menuDialog->height();

    if (oi_menuDialog->position == DIALOG_POS_LOW)
    {
        oi_menuDialog->position = DIALOG_POS_HIGH;

        oi_menuDialog->move(this->geometry().left()+DIALOG_MENU_LEFT,
                            this->geometry().top() + DIALOG_MENU_TOP);
    }
    else {
        oi_menuDialog->position = DIALOG_POS_LOW;

        oi_menuDialog->move(this->geometry().left()+DIALOG_MENU_LEFT,
                            this->geometry().top() + l_iDialogHeight + DIALOG_MENU_BOTTOM);
    }

}

void MainWindow::showMenuDialog()
{
    // once the button is clicked, show the dialog
    //menuDialog oi_menuDialog(this);

    oi_menuDialog->setModal(false);

    //int l_iDialogWidth =  oi_menuDialog->width();
    int l_iDialogHeight = oi_menuDialog->height();

    // place the dialog in the center of the panel
    /*
    oi_menuDialog->move(this->geometry().left() +
                  (this->width() - l_iDialogWidth) / 2,
                  this->geometry().top() +
                  (this->height() - l_iDialogHeight) / 2);
    */

    // place dialog on lower half screen
    oi_menuDialog->position = DIALOG_POS_LOW;

    oi_menuDialog->move(this->geometry().left()+DIALOG_MENU_LEFT,
                        this->geometry().top() + l_iDialogHeight + DIALOG_MENU_BOTTOM);

    oi_menuDialog->show();
}

void MainWindow::closeMenuDialog()
{
    oi_menuDialog->close();
}


// ******************************************
//               ENCODER BLOCK
// ******************************************


void MainWindow::initEvState()
{
    memset(&encEvState, 0, sizeof(EVENT_STATE));
}

bool MainWindow::isEncLeftRot()
{
    return encEvState.encLeftRot;
}

bool MainWindow::isEncRightRot()
{
    return encEvState.encRightRot;
}

bool MainWindow::isEncPressDown()
{
    return encEvState.encPressDown;
}

bool MainWindow::isEncPressUp()
{
    return encEvState.encPressUp;
}

bool MainWindow::isEncClick()
{
    return encEvState.encClick;
}

bool MainWindow::isEncDblClick()
{
    return encEvState.encDblClick;
}

bool MainWindow::isEncHold()
{
    return encEvState.encHold;
}

bool MainWindow::isEncTouched()
{
#ifdef DEBUG_MODE
    return (encEvState.encLeftRot || encEvState.encRightRot || encEvState.encPressDown || encEvState.encPressUp ||
            encEvState.encClick   || encEvState.encDblClick || encEvState.encHold);
#else
    return (encEvState.encLeftRot || encEvState.encRightRot || encEvState.encPressDown || encEvState.encPressUp);
#endif // DEBUG_MODE
}


void MainWindow::encoderPressDownHandler()
{
    encEvState.encPressDown = true;
    #ifdef DEBUG_MODE
        qDebug() << "encoder PressDown!";
    #endif // DEBUG_MODE
}

void MainWindow::encoderPressUpHandler()
{
    encEvState.encPressUp = true;
    #ifdef DEBUG_MODE
        //qDebug() << "encoder Released";
    #endif // DEBUG_MODE
}

void MainWindow::encoderLeftRotHandler()
{
    encEvState.encLeftRot = true;
    #ifdef DEBUG_MODE
        qDebug() << "encoder LEFT ROT";
    #endif // DEBUG_MODE
}

void MainWindow::encoderRightRotHandler()
{
    encEvState.encRightRot = true;
    #ifdef DEBUG_MODE
        qDebug() << "encoder RIGHT ROT";
    #endif // DEBUG_MODE
}

void MainWindow::encoderClickedHandler() {

    encEvState.encClick = true;

    #ifdef DEBUG_MODE
        qDebug() << "encoder CLICKED!";
    #endif // DEBUG_MODE
}

void MainWindow::encoderDblClickHandler() {

    encEvState.encDblClick = true;

    #ifdef DEBUG_MODE
        qDebug() << "encoder DOUBLE CLICKED !!";
    #endif // DEBUG_MODE
}

void MainWindow::encoderLongPressHandler() {

    encEvState.encHold = true;

    #ifdef DEBUG_MODE
        qDebug() << "encoder LONG PRESSED !!!";
    #endif // DEBUG_MODE
}

/*
void MainWindow::encoderLedRightRotHandler()
{
   encoderWheel.Wheel_Rotate_Right();
}

void MainWindow::encoderLedLeftRotHandler()
{
   encoderWheel.Wheel_Rotate_Left();
}
*/

// ******************************************


void MainWindow::initGraphicObjects()
{
    // pre-load splash-screen view
    ui->splashPanel->setStyleSheet("QWidget#splashPanel {background: url(./Images/splash.png);}");
    ui->stackedWidget->setCurrentWidget(ui->splashPanel);

    // init structures

    // pre-load fonts
    //ui->systemPanel->setStyleSheet("QLabel {font: 14px FreeMono; background-color: rgba(0,0,0,0), color: rgb(255, 128,0)}");

    // set labels and other graphic stuff on stacked panels

}



void MainWindow::initMainView()
{
    // load backgrounds
    QString panelStyleSheet, filePath;

    for (int index = 0; index <= menuViewList.viewNum; index++) {

        if(menuViewList.menuView[index].MenuName == (QString)SYSTEM_VIEW) {
            panelStyleSheet = "QWidget#systemPanel {background: url(";
            filePath = QApplication::applicationDirPath() + menuViewList.menuView[index].Background;
            panelStyleSheet += filePath;
            panelStyleSheet += ");}";
            ui->systemPanel->setStyleSheet(panelStyleSheet);

            continue;
        }

        if(menuViewList.menuView[index].MenuName == (QString)MPX_VIEW) {
            panelStyleSheet = "QWidget#mpxPanel {background: url(";
            filePath = QApplication::applicationDirPath() + menuViewList.menuView[index].Background;
            panelStyleSheet += filePath;
            panelStyleSheet += ");}";
            ui->mpxPanel->setStyleSheet(panelStyleSheet);

            continue;
        }

        if(menuViewList.menuView[index].MenuName == (QString)RDS_VIEW) {
            panelStyleSheet = "QWidget#rdsPanel {background: url(";
            filePath = QApplication::applicationDirPath() + menuViewList.menuView[index].Background;
            panelStyleSheet += filePath;
            panelStyleSheet += ");}";
            ui->rdsPanel->setStyleSheet(panelStyleSheet);

            continue;
        }

        if(menuViewList.menuView[index].MenuName == (QString)MPX_SPECTRUM_VIEW) {
            panelStyleSheet = "QWidget#mpxSpectrumPanel {background: url(";
            filePath = QApplication::applicationDirPath() + menuViewList.menuView[index].Background;
            panelStyleSheet += filePath;
            panelStyleSheet += ");}";
            ui->mpxSpectrumPanel->setStyleSheet(panelStyleSheet);

            continue;
        }

        if(menuViewList.menuView[index].MenuName == (QString)AUDIO_SPECTRUM_VIEW) {
            panelStyleSheet = "QWidget#audioSpectrumPanel {background: url(";
            filePath = QApplication::applicationDirPath() + menuViewList.menuView[index].Background;
            panelStyleSheet += filePath;
            panelStyleSheet += ");}";
            ui->audioSpectrumPanel->setStyleSheet(panelStyleSheet);

            continue;
        }

        if(menuViewList.menuView[index].MenuName == (QString)AUDIO_VIEW) {
            panelStyleSheet = "QWidget#audioPanel {background: url(";
            filePath = QApplication::applicationDirPath() + menuViewList.menuView[index].Background;
            panelStyleSheet += filePath;
            panelStyleSheet += ");}";
            ui->audioPanel->setStyleSheet(panelStyleSheet);

            continue;
        }
    }

    //ui->stackedWidget->setCurrentIndex(mainMenuStateIndex);

    // power off wheel leds
    encoderWheel.Set_Wheel(LED_OFF);
}

void MainWindow::setMainView()
{    
    // call initStackedWidget(mainMenuStateIndex) for content cleanse
    ui->stackedWidget->setCurrentIndex(mainMenuStateIndex);
    QWidget* currentWidget = ui->stackedWidget->currentWidget();
    currentWidget->update();
}

void MainWindow::updateWriteTimer()
{
    if(Output.getWriteStatus()) Output.updateWriteFile();
    if(ivOutput.getWriteStatus()) ivOutput.updateWriteFile();
}

// ----------------------------------------------------------------


/*
void MainWindow::paintEvent(QPaintEvent *event)
{
 Q_UNUSED(event);
}
*/

void MainWindow::updateAnimationTimerEvent()
{
  paintFlag = true;
}

bool MainWindow::eventFilter(QObject *o, QEvent *e)
{
    /*
    static int LplusR_Lev      = 6;    // -6 dB
    static int Pilot_Lev       = 20;   // -20 dB
    static int LminusRlow_Lev  = 25;   // -25 dB
    static int LminusRhigh_Lev = 25;   // -25 dB
    static int Rds_Lev         = 30;   // -30 dB
    static int NoiseLow_Lev    = 60;   // -60 dB
    static int NoiseHigh_Lev   = 60;   // -60 dB
    static int NoiseOver_Lev   = 60;   // -60 dB
    */

/*
    static int LplusR_Lev      = 80;
    static int Pilot_Lev       = 80;
    static int LminusRlow_Lev  = 80;
    static int LminusRhigh_Lev = 80;
    static int Rds_Lev         = 80;
    static int NoiseLow_Lev    = 80;
    static int NoiseHigh_Lev   = 80;
    static int NoiseOver_Lev   = 80;

    static int dir = 0;
*/





    if (o == ui->mpxSpectrumPanel &&
            ( e->type() == QEvent::UpdateRequest ||
              e->type() == QEvent::Paint))         {return true;}

    if (o == ui->mpxPanel &&
            ( e->type() == QEvent::UpdateRequest ||
              e->type() == QEvent::Paint))         {return true;}

    if (o == ui->audioSpectrumPanel &&
            ( e->type() == QEvent::UpdateRequest ||
              e->type() == QEvent::Paint))         {return true;}

    if (o == ui->systemPanel &&
            ( e->type() == QEvent::UpdateRequest ||
              e->type() == QEvent::Paint))         {

        paintFlag = false;

        QFont swFont;
        QPen  swPen;
        swFont.setFamily("FreeMono");
        swFont.setWeight(450);
        swFont.setPointSize(15);
        swPen.setColor(QColor("#fda921"));
        QDate tmpDate;
        tmpDate = tmpDate.currentDate();
        QTime tmpTime;
        tmpTime = tmpTime.currentTime();

        QPainter painter(ui->systemPanel);

        painter.setFont(swFont);
        painter.setPen(swPen);

        painter.drawText(344,27,tmpDate.toString("dd-MM-yy"));
        painter.drawText(344,57,tmpTime.toString("hh:mm:ss"));

        return true;
    }

    if (o == ui->rdsPanel &&
            ( e->type() == QEvent::UpdateRequest ||
              e->type() == QEvent::Paint))         {return true;}

    if (o == ui->audioPanel &&
            ( e->type() == QEvent::UpdateRequest ||
              e->type() == QEvent::Paint))         {return true;}
/*
        //process paint event of needed widget
        QPainter painter(ui->mpxSpectrumPanel);

        //painter.setWindow(77,38,386,81);

        // draw signal bars
        painter.setBrush(QColor(255,128,0));
        painter.setPen(QColor(255,128,0));

        QRect RectLpR(77, 39+LplusR_Lev, 74, 80-LplusR_Lev);
        QRect RectPilot(182,39+Pilot_Lev, 4, 80-Pilot_Lev);
        QRect RectLmRL(218, 39+LminusRlow_Lev, 74, 80-LminusRlow_Lev);
        QRect RectLmRH(297, 39+LminusRhigh_Lev, 74, 80-LminusRhigh_Lev);
        QRect RectRds(376, 39+Rds_Lev, 7, 80-Rds_Lev);

        painter.drawRect(RectLpR);
        painter.drawRect(RectPilot);
        painter.drawRect(RectLmRL);
        painter.drawRect(RectLmRH);
        painter.drawRect(RectRds);

        // draw noise bars
        painter.setBrush(QColor(43,59,65));
        painter.setPen(QColor(43,59,65));

        QRect RectNoiseL(155, 39+NoiseLow_Lev, 23, 80-NoiseLow_Lev);
        QRect RectNoiseH(190, 39+NoiseHigh_Lev, 23, 80-NoiseHigh_Lev);
        QRect RectNoiseO(388, 39+NoiseOver_Lev, 23, 80-NoiseOver_Lev);

        painter.drawRect(RectNoiseL);
        painter.drawRect(RectNoiseH);
        painter.drawRect(RectNoiseO);

        // draw horizontal lines
        painter.setPen(QColor(255,255,255));
        for(int i=0; i<5; i++) {
            painter.drawLine(77+0,38+1+20*i,77+386, 38+1+20*i);
        }

        if(dir) {
            LplusR_Lev+=2;
            Pilot_Lev+=2;
            LminusRlow_Lev+=2;
            LminusRhigh_Lev+=2;
            Rds_Lev+=2;
            NoiseLow_Lev+=2;
            NoiseHigh_Lev+=2;
            NoiseOver_Lev+=2;

            if (Rds_Lev==80) dir=0;
        }
        else {
            LplusR_Lev-=2;
            Pilot_Lev-=2;
            LminusRlow_Lev-=2;
            LminusRhigh_Lev-=2;
            Rds_Lev-=2;
            NoiseLow_Lev-=2;
            NoiseHigh_Lev-=2;
            NoiseOver_Lev-=2;

            if (Rds_Lev==0) dir=1;
        }
        return true;
    }
    */
    return QMainWindow::eventFilter(o, e);
}
