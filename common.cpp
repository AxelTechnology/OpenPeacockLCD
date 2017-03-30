#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QDir>
#include "common.h"

QString getStyleSheetAsString(const QString& p_strFileName)
{
   // Use QFile and point to a resource
   QFile l_file(QApplication::applicationDirPath() + "/QSS/" + p_strFileName);
   QString l_strStyle;
   // Open the file
   if(l_file.open(QFile::ReadOnly))
   {
      QTextStream styleIn(&l_file);
      // Read file to a string
      l_strStyle = styleIn.readAll();
      l_file.close();
   }

   return l_strStyle;
}
