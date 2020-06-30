/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the documentation of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "MainWindow.h"
#include <QApplication>
#include <QScreen>
#include "Log.h"
#include "CodeEditor.h"
#include "Jenkins.h"
#include <QObject>
#include <QProcess>



int main(int argc, char *argv[])
{
    QApplication EditorApp(argc, argv);






    /////

/*
    for (;;)

    {
static int i = 0;
    QProcess process;
    process.setWorkingDirectory("D:\\Users\\Ayman\\Desktop\\MerlinEmbedded\\CA20-5501 (Merlin Compatible 4-line Visible Camera)\\lint");

    // stderr has the module (file lint) progress
    // sttout has the actual data

    QByteArray data;
    DataProcess dp;
    QObject::connect(&process, &QProcess::readyReadStandardOutput, &dp, [&process,&data]()
    {
        QByteArray readStdOut = process.readAllStandardOutput();
        data.append(readStdOut);
    });

    QStringList files;
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Camera/Camera.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Camera/Cam_commands.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Camera/Cam_debug.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Camera/Cam_flash.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Camera/Cam_fpga.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Camera/Cam_if.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Camera/Cam_init.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Camera/Cam_parameters.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Camera/Cam_schedule.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Camera/Cam_serdes.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Camera/Cam_status.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Camera/Cam_temperature.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Camera/Cam_test.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Camera/Cam_types.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Camera/Cam_utilities.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Camera/Remote_update.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/CommonCodeMerlin/source/comms/cmn_com_cmd.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/CommonCodeMerlin/source/comms/cmn_com_parser.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/CommonCodeMerlin/source/comms/cmn_com_sd.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/CommonCodeMerlin/source/comms/cmn_com_sm.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/CommonCodeMerlin/source/comms/cmn_com_string.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/CommonCodeMerlin/source/drivers/cmn_drv_crc.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/CommonCodeMerlin/source/drivers/cmn_drv_sys.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/CommonCodeMerlin/source/drivers/cmn_drv_tem.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/CommonCodeMerlin/source/drivers/cmn_drv_z85.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/CommonCodeMerlin/source/mcu/cmn_mcu_adc.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/CommonCodeMerlin/source/mcu/cmn_mcu_cpu.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/CommonCodeMerlin/source/mcu/cmn_mcu_ebi.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/CommonCodeMerlin/source/mcu/cmn_mcu_eeprom.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/CommonCodeMerlin/source/mcu/cmn_mcu_gpio.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/CommonCodeMerlin/source/mcu/cmn_mcu_int.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/CommonCodeMerlin/source/mcu/cmn_mcu_spi.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/CommonCodeMerlin/source/mcu/cmn_mcu_timer.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/CommonCodeMerlin/source/mcu/cmn_mcu_twi.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/CommonCodeMerlin/source/mcu/cmn_mcu_usart.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Devices/Flash_SST26VF032B.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Devices/Interfaces/Ifc_DeviceSel.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Devices/Interfaces/Ifc_Spi.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Devices/Interfaces/Ifc_Twi.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Devices/SerDe_DS92LV242X.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Devices/Temp_ADT7410.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Devices/Temp_TMP421.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/CA20-5501 (Merlin Compatible 4-line Visible Camera)/src/test_visible/test_boot.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/CA20-5501 (Merlin Compatible 4-line Visible Camera)/src/test_visible/test_flash.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/CA20-5501 (Merlin Compatible 4-line Visible Camera)/src/test_visible/test_sensor.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/CA20-5501 (Merlin Compatible 4-line Visible Camera)/src/test_visible/test_serdes.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/CA20-5501 (Merlin Compatible 4-line Visible Camera)/src/visible_camera_application/Cam_vis_command.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/CA20-5501 (Merlin Compatible 4-line Visible Camera)/src/visible_camera_application/Test_interface_Visible.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/Buhler Sortex Shared Code/ATxmega/Utilities/Util_timing.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/CA20-5501 (Merlin Compatible 4-line Visible Camera)/src/ASF/common/boards/user_board/init.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/CA20-5501 (Merlin Compatible 4-line Visible Camera)/src/ASF/common/services/hugemem/avr8/avr8_hugemem.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/CA20-5501 (Merlin Compatible 4-line Visible Camera)/src/common_application_configuration/cmn_app_config.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/CA20-5501 (Merlin Compatible 4-line Visible Camera)/src/visible_camera_application/Cam_sensor_vis.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/CA20-5501 (Merlin Compatible 4-line Visible Camera)/src/visible_camera_application/Cam_sensor_vis_soc_comms.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/CA20-5501 (Merlin Compatible 4-line Visible Camera)/src/visible_camera_application/Cam_sensor_vis_gain.c");
    files << ("D:/Users/Ayman/Desktop/MerlinEmbedded/CA20-5501 (Merlin Compatible 4-line Visible Camera)/src/visible_camera_application/Cam_sensor_vis_dark_level_correction.c");

    QStringList args;
    args << ("+vm");
    args << ("-hFs1");
    args << ("-width(0)");
    args << ("+xml(doc)");
    args << ("-format=<m><f>%f</f><l>%l</l><t>%t</t><n>%n</n><d>%m</d></m>");
    args << ("-format_specific= ");

    args << "D:\\Users\\Ayman\\Desktop\\MerlinEmbedded\\CA20-5501 (Merlin Compatible 4-line Visible Camera)\\lint\\std.lnt";
    for (const QString& file : files)
    {
       args << (file);
    }

    process.setProgram("D:\\lint\\lint-nt.exe");
    process.setArguments(args);
    process.start();

    Q_ASSERT(process.waitForStarted());
    Q_ASSERT(process.waitForFinished(-1));

i++;
        qDebug() << "Finished " << i;

    }*/
    /////

    Log::createLogFile(LOG_FILENAME);
    DEBUG_LOG("---------- Starting Linty ----------");
    DEBUG_LOG("Linty version: " BUILD_VERSION);

    MainWindow mainWindow;

    // Center the screen
    QScreen* screen = QGuiApplication::primaryScreen();
    QSize screenSize = screen->size();

    int x = (screenSize.width()-mainWindow.width()) / 2;
    int y = (screenSize.height()-mainWindow.height() - 40) / 2;
    mainWindow.move(x, y);
    mainWindow.showMaximized();
    mainWindow.setWindowTitle(APPLICATION_NAME " " BUILD_VERSION);

    return EditorApp.exec();
}
