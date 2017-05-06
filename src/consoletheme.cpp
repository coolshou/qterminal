#include <QColor>
#include "consoletheme.h"

#include <QDebug>

const QString ConsoleThemeName[] = {
    "Green",
    "White",
    "Yellow",
    "Navy",
    "Custom"
};
//font color / base color
const QString ConsoleThemeValue[][2] = {
    {"green", "black"},
    {"white", "black"},
    {"Yellow", "gray"},
    {"Navy", "white"},
    {"white", "black"}
};

QString colorToString(QColor color) {
    qDebug() << QColor::colorNames();
    qDebug() << "======== " << QColor(color);
    return QString("test");
}
