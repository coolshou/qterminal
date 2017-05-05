#include <QColor>
#include "consoletheme.h"

#include <QDebug>

const QString ConsoleThemeName[] = {
    "Green",
    "Custom"
};
//font color / base color
const QString ConsoleThemeValue[][2] = {
    {"green", "black"},
    {"white", "black"}
};

QString colorToString(QColor color) {
    qDebug() << QColor::colorNames();
    qDebug() << "======== " << QColor(color);
    return QString("test");
}
