/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <QDebug>
#include <QFile>
#include <LXQt/Application>
#include "dialog.h"

#define PID_PATH "/tmp/lxqt-runner.pid"

Dialog *d;

void catchSIGUSR1(int signal)
{
    if (d != NULL)
    {
        qDebug("SIGUSR1 signal received");
        d->show();
    }
}

int main(int argc, char *argv[])
{
    /* in case another instance is running sent it the SIGUSR1 signal
     * to show it. then exit. */
    QFile pidFile(PID_PATH);
    if(pidFile.exists())
    {
        if (pidFile.open(QIODevice::ReadOnly))
        {
            int pidLR;
            pidFile.seek(0);
            QTextStream pidStream(&pidFile);

            pidStream >> pidLR;
            pidFile.close();

            if (kill(pidLR, SIGUSR1) == 0)
            {
                qDebug() << "Sent SIGUSR1 signal to lxqt-runner instance with PID" << pidLR;
                return 0;
            }
        }
    }

    LxQt::Application a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    QWidget *hiddenPreviewParent = new QWidget(0, Qt::Tool);
    d = new Dialog(hiddenPreviewParent);

    if (d != NULL)
    {
        // create a file containing PID; another instance can send the SIGUSR1 signal to this process
        QFile pidFile(PID_PATH);
        if (pidFile.open(QIODevice::WriteOnly))
        {
            QTextStream pidStream(&pidFile);
            pidStream << getpid();
            pidFile.close();
        }
        signal(SIGUSR1, catchSIGUSR1);
    }

    //d.show();

    return a.exec();
}
