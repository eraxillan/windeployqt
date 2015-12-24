/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
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
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "utils.h"
#include "qtmodules.h"
#include "jsonoutput.h"
#include "commandlineparser.h"
#include "deployment.h"

QT_BEGIN_NAMESPACE

int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationVersion(QLatin1String(QT_VERSION_STR));

    const QByteArray qtBinPath = QFile::encodeName(QDir::toNativeSeparators(QCoreApplication::applicationDirPath()));
    QByteArray path = qgetenv("PATH");
    if (!path.contains(qtBinPath)) { // QTBUG-39177, ensure Qt is in the path so that qt.conf is taken into account.
        path += ';';
        path += qtBinPath;
        qputenv("PATH", path);
    }

    CommandLineParser clParser;
    Options options;
    QString errorMessage;
    const QMap<QString, QString> qmakeVariables = queryQMakeAll(&errorMessage);
    const QString xSpec = qmakeVariables.value(QStringLiteral("QMAKE_XSPEC"));
    options.platform = platformFromMkSpec(xSpec);
    if (options.platform == WindowsMinGW || options.platform == Windows)
        options.compilerRunTime = true;

    {   // Command line
        QCommandLineParser parser;
        QString errorMessage;
        const int result = clParser.parseArguments(QCoreApplication::arguments(), &parser, &options, &errorMessage);
        if (result & CommandLineParser::CommandLineParseError)
            std::wcerr << errorMessage << "\n\n";
        if (result & CommandLineParser::CommandLineParseHelpRequested)
            std::fputs(qPrintable(CommandLineParser::helpText(parser)), stdout);
        if (result & CommandLineParser::CommandLineParseError)
            return 1;
        if (result & CommandLineParser::CommandLineParseHelpRequested)
            return 0;
    }

    if (qmakeVariables.isEmpty() || xSpec.isEmpty() || !qmakeVariables.contains(QStringLiteral("QT_INSTALL_BINS"))) {
        std::wcerr << "Unable to query qmake: " << errorMessage << '\n';
        return 1;
    }

    if (options.platform == UnknownPlatform) {
        std::wcerr << "Unsupported platform " << xSpec << '\n';
        return 1;
    }

    // Create directories
    if (!createDirectory(options.directory, &errorMessage)) {
        std::wcerr << errorMessage << '\n';
        return 1;
    }
    if (!options.libraryDirectory.isEmpty() && options.libraryDirectory != options.directory
        && !createDirectory(options.libraryDirectory, &errorMessage)) {
        std::wcerr << errorMessage << '\n';
        return 1;
    }

    if (clParser.optWebKit2())
        options.additionalLibraries |= QtWebKitModule;

    Deployment worker(options, qmakeVariables);
    const DeployResult result = worker.deploy(options, &errorMessage);
    if (!result) {
        std::wcerr << errorMessage << '\n';
        return 1;
    }

    if ((clParser.optWebKit2() != CommandLineParser::OptionDisabled)
        && (clParser.optWebKit2() == CommandLineParser::OptionEnabled
            || ((result.deployedQtLibraries & QtWebKitModule)
                && (result.directlyUsedQtLibraries & QtQuickModule)))) {
        if (optVerboseLevel)
            std::wcout << "Deploying: " << Options::webKitProcessC << "...\n";
        if (!worker.deployWebProcess(Options::webKitProcessC, &errorMessage)) {
            std::wcerr << errorMessage << '\n';
            return 1;
        }
    }

    if (result.deployedQtLibraries & QtWebEngineModule) {
        if (!worker.deployWebEngine( &errorMessage)) {
            std::wcerr << errorMessage << '\n';
            return 1;
        }
    }

    if (options.json) {
        if (options.list)
            std::fputs(options.json->toList(options.list, options.directory).constData(), stdout);
        else
            std::fputs(options.json->toJson().constData(), stdout);
        delete options.json;
        options.json = 0;
    }

    return 0;
}

QT_END_NAMESPACE
