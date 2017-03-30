#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "CustomDialogs/menu.h"
#include "common.h"
#include <QTimer>
#include <QFontDatabase>
#include <QPainter>

#ifdef UI_VIRTUAL_ENCODER
    #include <QKeyEvent>
#endif // UI_VIRTUAL_ENCODER

namespace Ui {
class MainWindow;
}


// finite state machine block

#define MAIN_THREAD_INTERVAL_MS                 20  // msec

#define MENU_STATUS_INIT_START                  0
#define MENU_STATUS_INIT_CHECK_FILE             100
#define MENU_STATUS_INIT_READ_DATA              200
#define MENU_STATUS_INIT_SET_MAIN_VIEW          300
#define MENU_BASE_DIALOG_CLOSE                  1000
#define MENU_BASE_DIALOG_CLOSE_REQ_MENU_DATA    1100
#define MENU_BASE_DIALOG_CLOSE_CHECK_FILE       1200
#define MENU_BASE_DIALOG_CLOSE_READ_FILE        1300
#define MENU_SETTINGS_DIALOG_OPEN               1400
#define MENU_SETTINGS_DIALOG_OPENED             1500
#define MENU_SETTINGS_DIALOG_CLOSE_REQ          2000
#define MENU_SETTINGS_EDIT_MODE_INIT            3000
#define MENU_SETTINGS_EDIT_MODE_BASE            3100
#define MENU_SETTINGS_EDIT_MODE_CLOSE           3200
#define MENU_SETTINGS_EDIT_MODE_SAVE_DLG        4000
#define MENU_SETTINGS_EDIT_MODE_SAVE_CHOOSE     4100
#define MENU_SETTINGS_EDIT_MODE_SAVE_UNDO       4200
#define MENU_SETTINGS_EDIT_MODE_STRING          5000

#define MENU_COMM_ERROR                         100000

#define DIALOG_MENU_OPEN_TIMER_COUNT            10000/MAIN_THREAD_INTERVAL_MS

// /////////////////////////////////////////////////



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /**
     * @brief getSystemReady return system status
     * @return true if is running correctly
     */
    bool getSystemReady();

    /**
     * @brief setSystemReady set system status
     * @param cond = true -> OK
     */
    void setSystemReady(bool cond);
    bool paintFlag;


protected:
#ifdef UI_VIRTUAL_ENCODER
    void keyPressEvent(QKeyEvent *event);
#endif // UI_VIRTUAL_ENCODER

//    void paintEvent(QPaintEvent* event);
    bool eventFilter(QObject *o, QEvent *e);
public slots:
    /**
     * @brief encoder Handlers
     */
    void encoderPressDownHandler();
    void encoderPressUpHandler();
    void encoderRightRotHandler();
    void encoderLeftRotHandler();

    void encoderClickedHandler();
    void encoderDblClickHandler();
    void encoderLongPressHandler();

    /**
     * @brief updateReadTimer check for json updates
     */
    void updateMainTimer();

    /**
     * @brief updateWriteTimer function called at the timer tick
     */


    void updateWriteTimer();

    /**
     * @brief isEncXXX are encoder status query functions
     * @return the queried status
     */
    bool isEncLeftRot();
    bool isEncRightRot();
    bool isEncPressDown();
    bool isEncPressUp();
    bool isEncClick();
    bool isEncDblClick();
    bool isEncHold();
    bool isEncTouched();

    void updateAnimationTimerEvent();

private:
    Ui::MainWindow *ui;

    /**
     * @brief mainTimer needed to handle all system
     */
    QTimer mainTimer;

    QTimer animationTimer;

    /**
     * @brief writeTimer timer for delayed write
     */
    QTimer writeTimer;

    /**
     * @brief oi_menuDialog the menu dialog
     */
    menuDialog *oi_menuDialog;

    /**
     * @brief systemReady tells if system is ready and working
     */
    bool systemReady;

    /**
     * @brief mainMenuInit set up init values
     */
    void mainMenuInit();

    /**
     * @brief encEvState holds event status changes
     */
    //EVENT_STATE encEvState;

    /**
     * @brief initEvState resets EVENT_STATE struct
     */
    void initEvState();

    /**
     * @brief initGraphicobjects set up graphic objects for background animation
     */
    void initGraphicObjects();

    /**
     * @brief showMenuDialog displays menu dialog
     */
    void showMenuDialog();

    /**
     * @brief closeMenuDialog closes the menu dialog and returns to main menu
     */
    void closeMenuDialog();

    /**
     * @brief moveMenuDialog toggle the menu dialog position on upper/lower half screen
     */
    void moveMenuDialog();

    /**
     * @brief initMainView prepare backgrounds
     */
    void initMainView();

    /**
     * @brief setMainView reads data (background file position, visible objecs and their characteristics)
     * from a struct filled by a json file.
     * @param mainMenuStateIndex (internally referenced)
     */
    void setMainView();

    /**
     * @brief mainMenuStateIndex tells which view is to be displayed
     */
    int mainMenuStateIndex;

    //void paintEvent(QPaintEvent *event);

};

#endif // MAINWINDOW_H
