#ifndef UI_COMMON_H
#define UI_COMMON_H

// here will take place all common stuff for the project ui

/**
 * @brief those values tells if edit menu state
 */
#define MENU_HIDDEN     0
#define MENU_NAV_MODE   1
#define MENU_EDIT_MODE  2
#define MENU_SAVE_MODE  3


/**
 * @brief dialog position on screen
 */

#define DIALOG_POS_LOW  0
#define DIALOG_POS_HIGH 1

#define DIALOG_MENU_LEFT    36
#define DIALOG_MENU_TOP     5
#define DIALOG_MENU_BOTTOM  5


#define ACTIVE_LABEL_COLOR      (QString) "QLabel { color: white; }"
#define INACTIVE_LABEL_COLOR    (QString) "QLabel { color: rgb(30, 30, 30); }"
#define BACKUP_LABEL_COLOR      (QString) "QLabel { color: black; }"
#define READONLY_LABEL_COLOR    (QString) "QLabel { color: rgb(200, 100, 0); }"

// number of dynamic instances for display label (image-only included)
#define MAX_LABEL_NUM       50

// name of the views


#define SYSTEM_VIEW         "System"
#define MPX_VIEW            "MPX"
#define RDS_VIEW            "RDS"
#define MPX_SPECTRUM_VIEW   "MPX_Spectrum"
#define AUDIO_SPECTRUM_VIEW "Audio_Spectrum"
#define AUDIO_VIEW          "Audio"

#endif // UI_COMMON_H
