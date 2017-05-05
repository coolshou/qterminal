#ifndef CONSOLETHEME_H
#define CONSOLETHEME_H

//#include <wchar.h>
#include <QString>
#include <QColor>

// ConsoleTheme enumeration
enum ConsoleTheme
{
    Green,
    Custom,
    NUM_ConsoleTheme
};
extern const QString ConsoleThemeName[NUM_ConsoleTheme];
extern const QString ConsoleThemeValue[NUM_ConsoleTheme][2];

QString colorToString(QColor color);

#endif // CONSOLETHEME_H
