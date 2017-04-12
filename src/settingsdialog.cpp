/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QtSerialPort/QSerialPortInfo>
#include <QIntValidator>
#include <QLineEdit>
#include <QFileDialog>

#include <QDebug>

QT_USE_NAMESPACE

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    intValidator = new QIntValidator(0, 4000000, this);

    ui->baudRateBox->setInsertPolicy(QComboBox::NoInsert);

    connect(ui->applyButton, SIGNAL(clicked()),
            this, SLOT(apply()));
    connect(ui->cancelButton, SIGNAL(clicked()),
            this, SLOT(cancel()));
    connect(ui->serialPortInfoListBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(showPortInfo(int)));
    connect(ui->baudRateBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(checkCustomBaudRatePolicy(int)));
    connect(ui->serialPortInfoListBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(checkCustomDevicePathPolicy(int)));
    connect(ui->browserBtn,SIGNAL(pressed()),this, SLOT(selectLogFileName()));
    connect(ui->FontSizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(slot_changeFontSize(int)));

    fillPortsParameters();
    fillPortsInfo();

    fillConsoleParameters();

    //updateSettings();
    setDefaultSetting();
    setSettings(defaultGroupName);

    //show serial tab
    ui->tabWidget->setCurrentWidget(ui->tab_Serial);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::setCurrentTab(int idx)
{
    int i;
    if ((ui->tabWidget->count()-1) < idx) {
        i=0;
    } else {
        i=idx;
    }
    ui->tabWidget->setCurrentIndex(i);
}

void SettingsDialog::setDemo()
{
    QPalette p = palette();
    //TODO: color theme form settings
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    ui->DemoPlainTextEdit->setPalette(p);
    slot_changeFontSize(currentSettings.fontSize);
}
void SettingsDialog::setDefaultSetting()
{
    //read default setting
    defaultGroupName = ui->serialPortInfoListBox->currentText();
    //qDebug() << "setDefaultSetting: " <<defaultGroupName;

}
SettingsDialog::Settings SettingsDialog::get_settings()
{
    return currentSettings;
}

void SettingsDialog::showPortInfo(int idx)
{
    if (idx == -1)
        return;

    QStringList list = ui->serialPortInfoListBox->itemData(idx).toStringList();
    ui->descriptionLabel->setText(tr("Description: %1").arg(list.count() > 1 ? list.at(1) : tr(blankString)));
    ui->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.count() > 2 ? list.at(2) : tr(blankString)));
    ui->serialNumberLabel->setText(tr("Serial number: %1").arg(list.count() > 3 ? list.at(3) : tr(blankString)));
    ui->locationLabel->setText(tr("Location: %1").arg(list.count() > 4 ? list.at(4) : tr(blankString)));
    ui->vidLabel->setText(tr("Vendor Identifier: %1").arg(list.count() > 5 ? list.at(5) : tr(blankString)));
    ui->pidLabel->setText(tr("Product Identifier: %1").arg(list.count() > 6 ? list.at(6) : tr(blankString)));
}

void SettingsDialog::apply()
{
    if (updateSettings()) {
        //hide();
        close();
        accept();
    }
}
void SettingsDialog::cancel()
{
    close();
}

void SettingsDialog::checkCustomBaudRatePolicy(int idx)
{
    bool isCustomBaudRate = !ui->baudRateBox->itemData(idx).isValid();
    ui->baudRateBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        ui->baudRateBox->clearEditText();
        QLineEdit *edit = ui->baudRateBox->lineEdit();
        edit->setValidator(intValidator);
    }
}

void SettingsDialog::checkCustomDevicePathPolicy(int idx)
{
    bool isCustomPath = !ui->serialPortInfoListBox->itemData(idx).isValid();
    ui->serialPortInfoListBox->setEditable(isCustomPath);
    if (isCustomPath)
        ui->serialPortInfoListBox->clearEditText();
}
void SettingsDialog::slot_changeFontSize(int size)
{
    QFont f;
    //font family
    f.setFamily(ui->DemoPlainTextEdit->font().family());
    if (size<=0) {
        size = currentSettings.fontSizeDefault;
    }
    if (f.pointSize() != size) {
        f.setPointSize(size);
    }
    ui->DemoPlainTextEdit->setFont(f);
}

void SettingsDialog::selectLogFileName()
{
    QString path;
    if (ui->logFilenameLineEdit->text().isEmpty()){
        //default home path
        path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    } else {
        path = ui->logFilenameLineEdit->text();
    }
    QString fileName;
    //use system's dialog
    fileName = QFileDialog::getSaveFileName(this,
        tr("Set Log Filename"), path, tr("Log Files (*.log *.LOG);;Text Files(*.txt *.TXT)"));
    if (!fileName.isEmpty()) {
        qDebug() << "selected file:" << fileName;
        ui->logFilenameLineEdit->setText(fileName);
    }

}

//console color parameters
void SettingsDialog::fillConsoleParameters()
{
    //TODO, theme
    //backgroung color
    QColor blackColor = Qt::black;
    ui->BaseColorComboBox->addItem(QStringLiteral("black"), blackColor);
    //font color
    QColor greenColor = Qt::green;
    ui->FontColorComboBox->addItem(QStringLiteral("green"), greenColor);

}

void SettingsDialog::fillPortsParameters()
{
    //TODO: all supported baudrate?
    ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud4800);
    ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->baudRateBox->addItem(QStringLiteral("57600"), QSerialPort::Baud57600);
    ui->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->baudRateBox->addItem(tr("Custom"));

    ui->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->dataBitsBox->setCurrentIndex(3);

    ui->parityBox->addItem(tr("None"), QSerialPort::NoParity);
    ui->parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
    ui->parityBox->addItem(tr("Odd"), QSerialPort::OddParity);
    ui->parityBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    ui->parityBox->addItem(tr("Space"), QSerialPort::SpaceParity);

    ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->stopBitsBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    ui->flowControlBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    ui->flowControlBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    ui->flowControlBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
}

void SettingsDialog::fillPortsInfo()
{
    ui->serialPortInfoListBox->clear();
    QString description;
    QString manufacturer;
    QString serialNumber;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        if (! isExistUsedSerial(info.portName())) {
            //TODO: when a serial is opened, don't show it here?
            qDebug() << "portName: " << info.portName();
            QStringList list;
            description = info.description();
            manufacturer = info.manufacturer();
            serialNumber = info.serialNumber();
            list << info.portName()
                 << (!description.isEmpty() ? description : blankString)
                 << (!manufacturer.isEmpty() ? manufacturer : blankString)
                 << (!serialNumber.isEmpty() ? serialNumber : blankString)
                 << info.systemLocation()
                 << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
                 << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

            ui->serialPortInfoListBox->addItem(list.first(), list);
        }
    }

    //ui->serialPortInfoListBox->addItem(tr("Custom"));
}

//apply UI's setting to currentSettings
bool SettingsDialog::updateSettings()
{
    currentSettings.name = ui->serialPortInfoListBox->currentText();

    if (ui->baudRateBox->currentIndex() == 4) {
        currentSettings.baudRate = ui->baudRateBox->currentText().toInt();
    } else {
        currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(
                    ui->baudRateBox->itemData(ui->baudRateBox->currentIndex()).toInt());
    }
    currentSettings.stringBaudRate = QString::number(currentSettings.baudRate);

    currentSettings.dataBits = static_cast<QSerialPort::DataBits>(
                ui->dataBitsBox->itemData(ui->dataBitsBox->currentIndex()).toInt());
    currentSettings.stringDataBits = ui->dataBitsBox->currentText();

    currentSettings.parity = static_cast<QSerialPort::Parity>(
                ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt());
    currentSettings.stringParity = ui->parityBox->currentText();

    currentSettings.stopBits = static_cast<QSerialPort::StopBits>(
                ui->stopBitsBox->itemData(ui->stopBitsBox->currentIndex()).toInt());
    currentSettings.stringStopBits = ui->stopBitsBox->currentText();

    currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
                ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt());
    currentSettings.stringFlowControl = ui->flowControlBox->currentText();

    currentSettings.localEchoEnabled = ui->localEchoCheckBox->isChecked();
    //console setting
    currentSettings.maxBlockCount = ui->maxBlockCountSpinBox->value();
    currentSettings.baseColor = static_cast<QColor>(ui->BaseColorComboBox->currentText());
    currentSettings.fontColor = static_cast<QColor>(ui->FontColorComboBox->currentText());
    //font fontFamily setting
    currentSettings.fontFamily = ui->DemoPlainTextEdit->font().family();
    currentSettings.fontSize = ui->FontSizeSpinBox->value();
    currentSettings.scrollToBottom = ui->scrollToBottomCheckBox->isChecked();
    //Log
    currentSettings.bLogEnable = ui->LogEnableGroupBox->isChecked();
    if (currentSettings.bLogEnable & ui->logFilenameLineEdit->text().isEmpty()) {
        ui->tabWidget->setCurrentWidget(ui->tab_Log);
        ui->logFilenameLineEdit->setFocus();
        return false;
    }
    currentSettings.stringLogFilename = ui->logFilenameLineEdit->text();
    currentSettings.bLogDateTime = ui->logDateTimeCheckBox->isChecked();
    //currentSettings.

    return true;
}

void SettingsDialog::setSettings(QString gname)
{
    QSettings *settings=new QSettings();
    setSettings(gname, settings);

}

void SettingsDialog::setSettings(QString gname, QSettings *settings)
{
    settings->beginGroup(gname);
    ui->serialPortInfoListBox->setCurrentText(settings->value("name").toString());
    ui->baudRateBox->setCurrentText(settings->value("baudRate").toString());
    ui->dataBitsBox->setCurrentText(settings->value("dataBits").toString());
    ui->parityBox->setCurrentText(settings->value("parity").toString());
    ui->stopBitsBox->setCurrentText(settings->value("stopBits").toString());
    ui->flowControlBox->setCurrentText(settings->value("flowControl").toString());
    ui->localEchoCheckBox->setChecked(settings->value("localEchoEnabled").toBool());
    //console setting
    ui->maxBlockCountSpinBox->setValue(settings->value("maxBlockCount").toInt());
    ui->BaseColorComboBox->setCurrentText(settings->value("baseColor").toString());
    ui->FontColorComboBox->setCurrentText(settings->value("fontColor").toString());
    //font family
    ui->fontComboBox->setCurrentText(settings->value("fontFamily").toString());
    ui->FontSizeSpinBox->setValue(settings->value("fontSize").toInt());
    ui->scrollToBottomCheckBox->setChecked(settings->value("scrollToBottom").toBool());
    //Log
    ui->LogEnableGroupBox->setChecked(settings->value("logEnable").toBool());
    ui->logFilenameLineEdit->setText(settings->value("logFilename").toString());
    ui->logDateTimeCheckBox->setChecked(settings->value("logDateTime").toBool());
    setDemo();
    settings->endGroup();
}

bool SettingsDialog::isExistUsedSerial(QString serName)
{
    if (usedSerials.indexOf(serName)<0) {
        return false;
    } else {
        return true;
    }

}

void SettingsDialog::addUsedSerial(QString serName)
{
    if (! isExistUsedSerial(serName)) {
        qDebug() << "addUsedSerial: " << serName;
        usedSerials.append(serName);
    }
}
void SettingsDialog::delUsedSerial(QString serName)
{
    int idx = usedSerials.indexOf(serName);
    if (idx>=0) {
        qDebug() << "delUsedSerial remove: "<<idx << " - " << usedSerials.value(idx);
        usedSerials.removeAt(idx);
    }
}

void SettingsDialog::updateUsedSerials(QStringList serials)
{
    int i;
    usedSerials.clear();
    for (i=0; i<ui->serialPortInfoListBox->count()-1; i++) {
        //usedSerials.append(serials.value(i));
        //addUsedSerial(serials.value(i));
        if (serials.indexOf(ui->serialPortInfoListBox->itemText(i))>=0) {
            qDebug() << "updateUsedSerials remove: " << ui->serialPortInfoListBox->itemText(i);
            ui->serialPortInfoListBox->removeItem(i);
        }
    }
}
/*
void SettingsDialog::closeEvent(QCloseEvent event)
{
    qDebug() << "closeevent:" ;//<< event.;
//        writeSettings();
//        event.accept();
//    } else {
//        event.ignore();
//    }
}
*/
