#include "menu.h"
#include "ui_menu.h"
#include "common.h"
#include "ui_common.h"
#include "json_common.h"
#include "jsonhandler.h"
#include "Threads/signalthread.h"
#include <QFontDatabase>
#include <QPixmap>
#include <QBitmap>
#include <QPainter>

#ifdef DEBUG_MODE
#include <QDebug>
#endif // DEBUG_MODE

extern MENU_VAR_LIST  menuVarList;
extern MENU_VIEW_LIST menuViewList;

extern JsonHandler Input;
extern JsonHandler Output;

extern SignalThread encoderThread;

extern EVENT_STATE encEvState;

menuDialog::menuDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::menuDialog)
{
    ui->setupUi(this);

    //ui->menuBackPanel->setFrameStyle(QFrame::StyledPanel);
    //Frameless window
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    this->setFixedSize(441, 60);

    this->setStyleSheet("background-color: rgba(0, 0, 0 ,0); border-radius: 3px;");

    ui->menuBackPanel->setStyleSheet(".QWidget{ background-color: rgb(55, 74, 83); border-color: white; border-style: solid; border-width: 1px; border-radius: 3px; }");

    // old background-color: rgb(36, 49, 55);
/*
    QPixmap px(this->size()); //Create pixmap with the same size of current widget
    px.fill(Qt::transparent); //Fill transparent

    QPainter p(&px);
    QBrush brush;

    brush.setColor(Qt::darkBlue);
    brush.setStyle(Qt::SolidPattern); //For fill
    p.setBrush(brush);
    p.drawRoundedRect(this->rect(), 5.0, 5.0); //Draw filled rounded rectangle on pixmap
    p.fillRect(this->rect(), brush.style());
    this->setMask(px.mask()); //The the mask for current widget.
*/
    // pre-load fonts

    QFontDatabase database;
    int result = database.addApplicationFont(":/Fonts/Calibri.ttf");
    if(result < 0) {
        // replace font
        #ifdef DEBUG_MODE
            qDebug() << "cannot load font";
        #endif // DEBUG_MODE
    }

    QFont font;
    font.setFamily("Calibri");
    font.setPointSize(16);
    font.setWeight(QFont::Bold);

    ui->menuItemDescLabel->setFont(font);

    font.setWeight(QFont::Normal);
    ui->menuItemValLabel->setFont(font);

    font.setPointSize(10);
    font.setWeight(QFont::Bold);
    ui->menuPreviousLabel->setFont(font);
    ui->menuLastValueLabel->setFont(font);

    /*
    //set up arrows images
    QPixmap pixmap;

    pixmap.load(":/Images/arrowUP.png");
    ui->UpArrow->setPixmap(pixmap);
    ui->UpArrow->setMask(pixmap.mask());

    pixmap.load(":/Images/arrowDOWN.png");
    ui->DownArrow->setPixmap(pixmap);
    ui->DownArrow->setMask(pixmap.mask());
    */


    // connect encoder signals to paired event slots...
 #ifdef UI_VIRTUAL_ENCODER
    connect(&encoderThread, SIGNAL(encoderPressUp()),
            this, SLOT(encoderMenuPressUpHandler()));

    connect(&encoderThread, SIGNAL(encoderPressDown()),
            this, SLOT(encoderMenuPressDownHandler()));

    connect(&encoderThread, SIGNAL(encoderRightRot()),
            this, SLOT(encoderMenuRightRotHandler()));

    connect(&encoderThread, SIGNAL(encoderLeftRot()),
            this, SLOT(encoderMenuLeftRotHandler()));

    connect(&encoderThread, SIGNAL(encoderClick()),
            this, SLOT(encoderMenuClickedHandler()));

    connect(&encoderThread, SIGNAL(encoderDoubleClick()),
            this, SLOT(encoderMenuDblClickHandler()));

    connect(&encoderThread, SIGNAL(encoderLongPress()),
            this, SLOT(encoderMenuLongPressHandler()));
 #endif  // UI_VIRTUAL_ENCODER

}

menuDialog::~menuDialog()
{
    delete ui;
}


void menuDialog::InitMenuView()
{
    // prepare label graphics
    //QPixmap pixmap(":/path/to/your/image.jpg");

    // set inactive ESC button
    this->showEscButton(true, false);

    // set active SAVE button and hide it
    confirmSave = true;
    this->showSaveButton(false, true);

    // set up/down arrows
    this->showArrows(false, (menuVarList.varNum > 1));

    // set main colors
    ui->menuItemDescLabel->setStyleSheet(ACTIVE_LABEL_COLOR);
    ui->menuItemValLabel->setStyleSheet(ACTIVE_LABEL_COLOR);
    ui->menuLastValueLabel->setStyleSheet(BACKUP_LABEL_COLOR);
    ui->menuPreviousLabel->setStyleSheet(BACKUP_LABEL_COLOR);

    // hide menuLastValueLabel and menuPreviousLabel
    ui->menuLastValueLabel->hide();
    ui->menuPreviousLabel->hide();

    // set up data for vert slider
    ui->menuItemPosSlider->setMaximum(menuVarList.varNum);
    ui->menuItemPosSlider->setValue(menuVarList.index);
    //ui->menuItemPosSlider->update();

    // update data for menuItemDescLabel and menuItemValLabel
    ui->menuItemDescLabel->setText(menuVarList.menuVar[menuVarList.index].VarLabel);
    ui->menuItemValLabel->setText(menuVarList.menuVar[menuVarList.index].VarValueStr);
}

void menuDialog::showSaveButton(bool show, bool selected)
{
    QPixmap pixmap;

    if(selected==true) {
        pixmap.load(":/Images/SAVE_EN.png");
        ui->menuSaveLabel->setPixmap(pixmap);
        ui->menuSaveLabel->setMask(pixmap.mask());
    }
    else {
        pixmap.load(":/Images/SAVE_DIS.png");
        ui->menuSaveLabel->setPixmap(pixmap);
        ui->menuSaveLabel->setMask(pixmap.mask());
    }

    if(show==true) {
        if(ui->menuSaveLabel->isHidden())
            ui->menuSaveLabel->show();
    }
    else {
        if(!(ui->menuSaveLabel->isHidden()))
            ui->menuSaveLabel->hide();
    }
}

void menuDialog::showEscButton(bool show, bool selected)
{
    QPixmap pixmap;

    if(selected==true) {
        pixmap.load(":/Images/ESC_EN.png");
        ui->menuEscLabel->setPixmap(pixmap);
        ui->menuEscLabel->setMask(pixmap.mask());
    }
    else {
        pixmap.load(":/Images/ESC_DIS.png");
        ui->menuEscLabel->setPixmap(pixmap);
        ui->menuEscLabel->setMask(pixmap.mask());
    }

    if(show==true) {
        if(ui->menuEscLabel->isHidden())
            ui->menuEscLabel->show();
    }
    else {
        if(!(ui->menuEscLabel->isHidden()))
            ui->menuEscLabel->hide();
    }
}


void menuDialog::showArrows(bool showUP, bool showDOWN)
{
    //set up arrows images
    QPixmap pixmap;

    if (showUP) {
        pixmap.load(":/Images/arrowUP.png");
    }
    else {
        pixmap.load(":/Images/arrowUP_Dis.png");
    }

    ui->UpArrow->setPixmap(pixmap);
    ui->UpArrow->setMask(pixmap.mask());

    if (showDOWN) {
        pixmap.load(":/Images/arrowDOWN.png");
    }
    else {
        pixmap.load(":/Images/arrowDOWN_Dis.png");
    }

    ui->DownArrow->setPixmap(pixmap);
    ui->DownArrow->setMask(pixmap.mask());
}

void menuDialog::UpdateMenuView()
{
    // update data for menuItemDescLabel and menuItemValLabel and ESC label
    if(menuVarList.index >= 0) {

        // set main colors
        if(this->editVar_getPermissions() != (QString) JSON_VAR_MODE_READONLY) {
            ui->menuItemDescLabel->setStyleSheet(ACTIVE_LABEL_COLOR);
            ui->menuItemValLabel->setStyleSheet(ACTIVE_LABEL_COLOR);
        }
        else {
            ui->menuItemDescLabel->setStyleSheet(READONLY_LABEL_COLOR);
            ui->menuItemValLabel->setStyleSheet(READONLY_LABEL_COLOR);
        }

        // hide ESC button
        if(menuVarList.index == 0)
            this->showEscButton(true, false);

        // set up data for vert slider
        ui->menuItemPosSlider->setValue(menuVarList.index);
        //ui->menuItemPosSlider->update();

        // update data for menuItemDescLabel and menuItemValLabel
        ui->menuItemDescLabel->setText(menuVarList.menuVar[menuVarList.index].VarLabel);
        ui->menuItemValLabel->setText(menuVarList.menuVar[menuVarList.index].VarValueStr);

        this->showArrows(menuVarList.index != 0, menuVarList.index<menuVarList.varNum);
    }
    else {
        // set main colors
        ui->menuItemDescLabel->setStyleSheet(INACTIVE_LABEL_COLOR);
        ui->menuItemValLabel->setStyleSheet(INACTIVE_LABEL_COLOR);

        // show ESC button
        this->showEscButton(true, true);
    }

}


void menuDialog::InitEditMode()
{
  QString editText = "---";

  // clear backup values
  iEditBackupVal = 0;
  fEditBackupVal = 0.0;
  sEditBackupVal ="";

  // check variable to be edited, save a backup value and prepare menuLastValueLabel

  // hide arrows
  this->showArrows(false, false);

  if(menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_INT) {

      iEditBackupVal = menuVarList.menuVar[menuVarList.index].iVarValue;

      editText = QString::number(menuVarList.menuVar[menuVarList.index].iVarValue, 10);

  }
  else if(menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_ENUM ||
          menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_BOOL) {

      iEditBackupVal = menuVarList.menuVar[menuVarList.index].iVarValue;

      editText = menuVarList.menuVar[menuVarList.index].varList[menuVarList.menuVar[menuVarList.index].iVarValue];
  }
  else if(menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_FLOAT) {

      fEditBackupVal = menuVarList.menuVar[menuVarList.index].f_VarValue;

      editText = QString::number(menuVarList.menuVar[menuVarList.index].f_VarValue, 'f', menuVarList.menuVar[menuVarList.index].precision);
  }
  else if(menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_STRING) {

      sEditBackupVal = menuVarList.menuVar[menuVarList.index].strVarValue;

      editText = menuVarList.menuVar[menuVarList.index].strVarValue;
  }

  if(menuVarList.menuVar[menuVarList.index].varUnits != "") {
      editText += menuVarList.menuVar[menuVarList.index].varUnits;
  }

  ui->menuLastValueLabel->setText(editText);

  // show menuPreviousLabel, menuLastValueLabel and SAVE label
  ui->menuPreviousLabel->show();
  ui->menuLastValueLabel->show();

  // hide arrows

  // show SAVE & ESC buttons
  this->showEscButton(true, false);
  this->showSaveButton(true, false);
}

void menuDialog::editVar_Inc()
{
    // (if result is beyond the maximum)
    // increment variable value
    // update visual status
    // send command with changed value

    QString editText = "";

    if(menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_INT) {

        if((menuVarList.menuVar[menuVarList.index].iVarValue + menuVarList.menuVar[menuVarList.index].stepVal) <= menuVarList.menuVar[menuVarList.index].maxVal) {

            menuVarList.menuVar[menuVarList.index].iVarValue += menuVarList.menuVar[menuVarList.index].stepVal;
            editText = QString::number(menuVarList.menuVar[menuVarList.index].iVarValue, 10);

            if(menuVarList.menuVar[menuVarList.index].varUnits != "") {
                editText += menuVarList.menuVar[menuVarList.index].varUnits;
            }

            ui->menuItemValLabel->setText(editText);

            Output.JsonWriteVal(JSON_WRITE_CMD, menuVarList.menuVar[menuVarList.index].VarName, menuVarList.menuVar[menuVarList.index].iVarValue);
        }


    }
    else if(menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_ENUM) {

        if((menuVarList.menuVar[menuVarList.index].iVarValue + menuVarList.menuVar[menuVarList.index].stepVal) <= menuVarList.menuVar[menuVarList.index].maxVal) {

            menuVarList.menuVar[menuVarList.index].iVarValue += menuVarList.menuVar[menuVarList.index].stepVal;
            editText = menuVarList.menuVar[menuVarList.index].varList[menuVarList.menuVar[menuVarList.index].iVarValue];

            if(menuVarList.menuVar[menuVarList.index].varUnits != "") {
                editText += menuVarList.menuVar[menuVarList.index].varUnits;
            }

            ui->menuItemValLabel->setText(editText);

            Output.JsonWriteVal(JSON_WRITE_CMD, menuVarList.menuVar[menuVarList.index].VarName, menuVarList.menuVar[menuVarList.index].iVarValue);
        }

    }
    else if(menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_BOOL) {

        if((menuVarList.menuVar[menuVarList.index].iVarValue + menuVarList.menuVar[menuVarList.index].stepVal) <= menuVarList.menuVar[menuVarList.index].maxVal) {

            menuVarList.menuVar[menuVarList.index].iVarValue += menuVarList.menuVar[menuVarList.index].stepVal;
            editText = menuVarList.menuVar[menuVarList.index].varList[menuVarList.menuVar[menuVarList.index].iVarValue];

            if(menuVarList.menuVar[menuVarList.index].varUnits != "") {
                editText += menuVarList.menuVar[menuVarList.index].varUnits;
            }

            ui->menuItemValLabel->setText(editText);

            Output.JsonWriteVal(JSON_WRITE_CMD, menuVarList.menuVar[menuVarList.index].VarName, menuVarList.menuVar[menuVarList.index].iVarValue?true:false);
        }

    }
    else if(menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_FLOAT) {

        if((menuVarList.menuVar[menuVarList.index].f_VarValue + menuVarList.menuVar[menuVarList.index].f_stepVal) <= menuVarList.menuVar[menuVarList.index].f_maxVal) {

            menuVarList.menuVar[menuVarList.index].f_VarValue += menuVarList.menuVar[menuVarList.index].f_stepVal;
            editText = QString::number(menuVarList.menuVar[menuVarList.index].f_VarValue, 'f', menuVarList.menuVar[menuVarList.index].precision);

            if(menuVarList.menuVar[menuVarList.index].varUnits != "") {
                editText += menuVarList.menuVar[menuVarList.index].varUnits;
            }

            ui->menuItemValLabel->setText(editText);

            Output.JsonWriteVal(JSON_WRITE_CMD, menuVarList.menuVar[menuVarList.index].VarName, menuVarList.menuVar[menuVarList.index].f_VarValue, menuVarList.menuVar[menuVarList.index].precision);
        }
    }



}

void menuDialog::editVar_Dec()
{
    // (if result is above the minimum)
    // decrement variable value
    // update visual status
    // send command with changed value

    QString editText = "";

    if(menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_INT) {

        if((menuVarList.menuVar[menuVarList.index].iVarValue - menuVarList.menuVar[menuVarList.index].stepVal) >= menuVarList.menuVar[menuVarList.index].minVal) {

            menuVarList.menuVar[menuVarList.index].iVarValue -= menuVarList.menuVar[menuVarList.index].stepVal;
            editText = QString::number(menuVarList.menuVar[menuVarList.index].iVarValue, 10);

            if(menuVarList.menuVar[menuVarList.index].varUnits != "") {
                editText += menuVarList.menuVar[menuVarList.index].varUnits;
            }

            ui->menuItemValLabel->setText(editText);

            Output.JsonWriteVal(JSON_WRITE_CMD, menuVarList.menuVar[menuVarList.index].VarName, menuVarList.menuVar[menuVarList.index].iVarValue);
        }


    }
    else if(menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_ENUM) {

        if((menuVarList.menuVar[menuVarList.index].iVarValue - menuVarList.menuVar[menuVarList.index].stepVal) >= menuVarList.menuVar[menuVarList.index].minVal) {

            menuVarList.menuVar[menuVarList.index].iVarValue -= menuVarList.menuVar[menuVarList.index].stepVal;
            editText = menuVarList.menuVar[menuVarList.index].varList[menuVarList.menuVar[menuVarList.index].iVarValue];

            if(menuVarList.menuVar[menuVarList.index].varUnits != "") {
                editText += menuVarList.menuVar[menuVarList.index].varUnits;
            }

            ui->menuItemValLabel->setText(editText);

            Output.JsonWriteVal(JSON_WRITE_CMD, menuVarList.menuVar[menuVarList.index].VarName, menuVarList.menuVar[menuVarList.index].iVarValue);
        }
    }
    else if(menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_BOOL) {

        if((menuVarList.menuVar[menuVarList.index].iVarValue - menuVarList.menuVar[menuVarList.index].stepVal) >= menuVarList.menuVar[menuVarList.index].minVal) {

            menuVarList.menuVar[menuVarList.index].iVarValue -= menuVarList.menuVar[menuVarList.index].stepVal;
            editText = menuVarList.menuVar[menuVarList.index].varList[menuVarList.menuVar[menuVarList.index].iVarValue];

            if(menuVarList.menuVar[menuVarList.index].varUnits != "") {
                editText += menuVarList.menuVar[menuVarList.index].varUnits;
            }

            ui->menuItemValLabel->setText(editText);

            Output.JsonWriteVal(JSON_WRITE_CMD, menuVarList.menuVar[menuVarList.index].VarName, menuVarList.menuVar[menuVarList.index].iVarValue?true:false);
        }
    }
    else if(menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_FLOAT) {

        if((menuVarList.menuVar[menuVarList.index].f_VarValue - menuVarList.menuVar[menuVarList.index].f_stepVal) >= menuVarList.menuVar[menuVarList.index].f_minVal) {

            menuVarList.menuVar[menuVarList.index].f_VarValue -= menuVarList.menuVar[menuVarList.index].f_stepVal;
            editText = QString::number(menuVarList.menuVar[menuVarList.index].f_VarValue, 'f', menuVarList.menuVar[menuVarList.index].precision);

            if(menuVarList.menuVar[menuVarList.index].varUnits != "") {
                editText += menuVarList.menuVar[menuVarList.index].varUnits;
            }

            ui->menuItemValLabel->setText(editText);

            Output.JsonWriteVal(JSON_WRITE_CMD, menuVarList.menuVar[menuVarList.index].VarName, menuVarList.menuVar[menuVarList.index].f_VarValue, menuVarList.menuVar[menuVarList.index].precision);
        }
    }

    // update VarValueStr
    menuVarList.menuVar[menuVarList.index].VarValueStr = ui->menuItemValLabel->text();

}

bool menuDialog::editVar_isChanged()
{
   if(menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_INT  ||
      menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_ENUM ||
      menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_BOOL)
       return (menuVarList.menuVar[menuVarList.index].iVarValue != iEditBackupVal);

   if(menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_FLOAT)
       return (menuVarList.menuVar[menuVarList.index].f_VarValue != fEditBackupVal);

   if(menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_STRING)
       return (menuVarList.menuVar[menuVarList.index].strVarValue != sEditBackupVal);

   return false;
}

QString menuDialog::editVar_getPermissions()
{
    return (menuVarList.menuVar[menuVarList.index].VarMode);
}


void menuDialog::editShowSaveDialog()
{
    // focus SAVE button
    this->showSaveButton(true, true);
}

void menuDialog::toggleEscSaveStatus()
{
    // toggle status
    confirmSave = ! confirmSave;

    // set color for ESC& SAVE button
    if(confirmSave) {
        //ui->menuEscLabel->setStyleSheet(INACTIVE_LABEL_COLOR);
        //ui->menuSaveLabel->setStyleSheet(ACTIVE_LABEL_COLOR);
        this->showEscButton(true, false);
        this->showSaveButton(true, true);
    }
    else {
        //ui->menuSaveLabel->setStyleSheet(INACTIVE_LABEL_COLOR);
        //ui->menuEscLabel->setStyleSheet(ACTIVE_LABEL_COLOR);
        this->showEscButton(true, true);
        this->showSaveButton(true, false);
    }
}

bool menuDialog::getConfirmState()
{
    return confirmSave;
}

void menuDialog::editVar_Confirm()
{
    // update VarValueStr
    menuVarList.menuVar[menuVarList.index].VarValueStr = ui->menuItemValLabel->text();
}


void menuDialog::editVar_Undo()
{
    // (if result is above the minimum)
    // decrement variable value
    // update visual status
    // send command with changed value

    QString editText = "";

    if(menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_INT) {

        menuVarList.menuVar[menuVarList.index].iVarValue = iEditBackupVal;
        editText = QString::number(menuVarList.menuVar[menuVarList.index].iVarValue, 10);

        if(menuVarList.menuVar[menuVarList.index].varUnits != "") {
            editText += menuVarList.menuVar[menuVarList.index].varUnits;
        }

        ui->menuLastValueLabel->setText(editText);

        Output.JsonWriteVal(JSON_WRITE_CMD, menuVarList.menuVar[menuVarList.index].VarName, menuVarList.menuVar[menuVarList.index].iVarValue);
    }
    else if(menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_ENUM) {

        menuVarList.menuVar[menuVarList.index].iVarValue = iEditBackupVal;
        editText = menuVarList.menuVar[menuVarList.index].varList[menuVarList.menuVar[menuVarList.index].iVarValue];

        if(menuVarList.menuVar[menuVarList.index].varUnits != "") {
            editText += menuVarList.menuVar[menuVarList.index].varUnits;
        }

        ui->menuLastValueLabel->setText(editText);

        Output.JsonWriteVal(JSON_WRITE_CMD, menuVarList.menuVar[menuVarList.index].VarName, menuVarList.menuVar[menuVarList.index].iVarValue);

    }
    else if(menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_BOOL) {

        menuVarList.menuVar[menuVarList.index].iVarValue = iEditBackupVal;
        editText = menuVarList.menuVar[menuVarList.index].varList[menuVarList.menuVar[menuVarList.index].iVarValue];

        if(menuVarList.menuVar[menuVarList.index].varUnits != "") {
            editText += menuVarList.menuVar[menuVarList.index].varUnits;
        }

        ui->menuLastValueLabel->setText(editText);

        Output.JsonWriteVal(JSON_WRITE_CMD, menuVarList.menuVar[menuVarList.index].VarName, menuVarList.menuVar[menuVarList.index].iVarValue);

    }
    else if(menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_FLOAT) {

        menuVarList.menuVar[menuVarList.index].f_VarValue = fEditBackupVal;
        editText = QString::number(menuVarList.menuVar[menuVarList.index].f_VarValue, 'f', menuVarList.menuVar[menuVarList.index].precision);

        if(menuVarList.menuVar[menuVarList.index].varUnits != "") {
            editText += menuVarList.menuVar[menuVarList.index].varUnits;
        }

        ui->menuLastValueLabel->setText(editText);

        Output.JsonWriteVal(JSON_WRITE_CMD, menuVarList.menuVar[menuVarList.index].VarName, menuVarList.menuVar[menuVarList.index].f_VarValue, menuVarList.menuVar[menuVarList.index].precision);
    }
    else if(menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_STRING) {

        menuVarList.menuVar[menuVarList.index].strVarValue = sEditBackupVal;

        ui->menuLastValueLabel->setText(editText);

        Output.JsonWriteVal(JSON_WRITE_CMD, menuVarList.menuVar[menuVarList.index].VarName, menuVarList.menuVar[menuVarList.index].strVarValue);
    }

    // update VarValueStr
    menuVarList.menuVar[menuVarList.index].VarValueStr = ui->menuLastValueLabel->text();
}

bool menuDialog::is_editing_a_String()
{
    return (menuVarList.menuVar[menuVarList.index].VarType == JSON_VAR_TYPE_STRING);
}

void menuDialog::editStringInit()
{
    // insert here graphical view for string edit (see stramerMax code)
}


void menuDialog::encoderMenuPressDownHandler()
{
    encEvState.encPressDown = true;
    #ifdef DEBUG_MODE
        qDebug() << "encoder PressDown!";
    #endif // DEBUG_MODE
}

void menuDialog::encoderMenuPressUpHandler()
{
    encEvState.encPressUp = true;
    #ifdef DEBUG_MODE
        //qDebug() << "encoder Released";
    #endif // DEBUG_MODE
}

void menuDialog::encoderMenuLeftRotHandler()
{
    encEvState.encLeftRot = true;
    #ifdef DEBUG_MODE
        qDebug() << "encoder LEFT ROT";
    #endif // DEBUG_MODE
}

void menuDialog::encoderMenuRightRotHandler()
{
    encEvState.encRightRot = true;
    #ifdef DEBUG_MODE
        qDebug() << "encoder RIGHT ROT";
    #endif // DEBUG_MODE
}

void menuDialog::encoderMenuClickedHandler() {

    encEvState.encClick = true;

    #ifdef DEBUG_MODE
        qDebug() << "encoder CLICKED!";
    #endif // DEBUG_MODE
}

void menuDialog::encoderMenuDblClickHandler() {

    encEvState.encDblClick = true;

    #ifdef DEBUG_MODE
        qDebug() << "encoder DOUBLE CLICKED !!";
    #endif // DEBUG_MODE
}

void menuDialog::encoderMenuLongPressHandler() {

    encEvState.encHold = true;

    #ifdef DEBUG_MODE
        qDebug() << "encoder LONG PRESSED !!!";
    #endif // DEBUG_MODE
}


