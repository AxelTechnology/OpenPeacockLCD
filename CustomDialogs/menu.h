#ifndef MENU_H
#define MENU_H

#include <QDialog>

namespace Ui {
class menuDialog;
}

class menuDialog : public QDialog
{
    Q_OBJECT

public:
    explicit menuDialog(QWidget *parent = 0);
    ~menuDialog();

    /**
     * @brief position keep position of the dialog on the screen
     */
    int position;

    /**
     * @brief InitMenuView prepare menu items for visualization before dialog show
     */
    void InitMenuView();

    /**
     * @brief UpdateMenuView refresh visualization data when user rotate the encoder
     */
    void UpdateMenuView();

    /**
     * @brief InitEditMode set up view for edit a variable
     */
    void InitEditMode();

    /**
     * @brief is_editing_a_String check if is being editing a string
     * @return a bool
     */
    bool is_editing_a_String();

    /**
     * @brief editStringInit custom graphical handler for string input
     */
    void editStringInit();

    /**
     * @brief editVar_Inc handle var increment and effective remote value change
     */
    void editVar_Inc();

    /**
     * @brief editVar_Dec handle var decrement and effective remote value change
     */
    void editVar_Dec();

    /**
     * @brief editVar_Confirm updates shown value
     */
    void editVar_Confirm();

    /**
     * @brief editVar_getPermissions returns variable read/write permissions
     * @return read-only or read-write
     */
    QString editVar_getPermissions();

    /**
     * @brief editVar_Undo set back previous value
     */
    void editVar_Undo();

    /**
     * @brief editVar_isChanged check if edited value is changed or not
     * @return the change state
     */
    bool editVar_isChanged();

    /**
     * @brief editShowSaveDialog prepare SAVE/ESC handler
     */
    void editShowSaveDialog();

    /**
     * @brief toggleEscSaveStatus alternates SAVE and ESC on variable save request
     */
    void toggleEscSaveStatus();

    /**
     * @brief getConfirmState returns the save/esc state
     * @return the state
     */
    bool getConfirmState();

    /**
     * @brief showSaveButton handles view/settings of SAVE button
     * @param show      show or hide button
     * @param selected  selected/not selected
     */
    void showSaveButton(bool show, bool selected);

    /**
     * @brief showESCButton handles view/settings of ESC button
     * @param show      show or hide button
     * @param selected  selected/not selected
     */
    void showEscButton(bool show, bool selected);

    /**
     * @brief showArrows shows or hide nav up/down arrows
     * @param showUP    flag for UP arrow
     * @param showDOWN  flag for DOWN arrow
     */
    void showArrows(bool showUP, bool showDOWN);

public slots:
    /**
     * @brief encoder Handlers
     */
    void encoderMenuPressDownHandler();
    void encoderMenuPressUpHandler();
    void encoderMenuRightRotHandler();
    void encoderMenuLeftRotHandler();

    void encoderMenuClickedHandler();
    void encoderMenuDblClickHandler();
    void encoderMenuLongPressHandler();

private:
    Ui::menuDialog *ui;

    /**
     * @brief xEditBackupVal saved backup value of being edited vars
     */
    int     iEditBackupVal;
    double  fEditBackupVal;
    QString sEditBackupVal;

    /**
     * @brief confirmSave tells if user want to save changes or not
     */
    bool    confirmSave;
};

#endif // MENU_H
