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

#include "commandlineparser.h"
#include "qtmodules.h"
#include "utils.h"
#include "jsonoutput.h"

QT_BEGIN_NAMESPACE

static inline QString msgFileDoesNotExist(const QString & file)
{
    return QLatin1Char('"') + QDir::toNativeSeparators(file)
        + QStringLiteral("\" does not exist.");
}

// Simple line wrapping at 80 character boundaries.
static inline QString lineBreak(QString s)
{
    for (int i = 80; i < s.size(); i += 80) {
        const int lastBlank = s.lastIndexOf(QLatin1Char(' '), i);
        if (lastBlank >= 0) {
            s[lastBlank] = QLatin1Char('\n');
            i = lastBlank + 1;
        }
    }
    return s;
}

// Return binary from folder
static QString findBinary(const QString &directory, Platform platform)
{
    QDir dir(QDir::cleanPath(directory));

    const QStringList nameFilters = (platform & WindowsBased) ?
                QStringList(QStringLiteral("*.exe")) : QStringList();
    foreach (const QString &binary, dir.entryList(nameFilters, QDir::Files | QDir::Executable)) {
        if (!binary.contains(QLatin1String(Options::webKitProcessC), Qt::CaseInsensitive)
                && !binary.contains(QLatin1String(Options::webEngineProcessC), Qt::CaseInsensitive)) {
            return dir.filePath(binary);
        }
    }
    return QString();
}

//-----------------------------------------------------------------------------

CommandLineParser::CommandLineParser(): m_parser(), m_optWebKit2(OptionAuto)
{
    m_parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    m_parser.setApplicationDescription(QStringLiteral("Qt Deploy Tool ") + QLatin1String(QT_VERSION_STR)
                                      + QLatin1String("\n\nThe simplest way to use windeployqt is to add the bin directory of your Qt\n"
                                                      "installation (e.g. <QT_DIR\\bin>) to the PATH variable and then run:\n  windeployqt <path-to-app-binary>\n"
                                                      "If ICU, ANGLE, etc. are not in the bin directory, they need to be in the PATH\nvariable. "
                                                      "If your application uses Qt Quick, run:\n  windeployqt --qmldir <path-to-app-qml-files> <path-to-app-binary>"));
}

QString CommandLineParser::helpText()
{
    QString result = m_parser.helpText();
    // Replace the default-generated text which is too long by a short summary
    // explaining how to enable single libraries.
    const int moduleStart = result.indexOf(QLatin1String("\n  --bluetooth"));
    const int argumentsStart = result.lastIndexOf(QLatin1String("\nArguments:"));
    if (moduleStart >= argumentsStart)
        return result;
    QString moduleHelp = QLatin1String(
                "\n\nQt libraries can be added by passing their name (-xml) or removed by passing\n"
                "the name prepended by --no- (--no-xml). Available libraries:\n");
    moduleHelp += lineBreak(QString::fromLatin1(formatQtModules(0xFFFFFFFFFFFFFFFFull, true)));
    moduleHelp += QLatin1Char('\n');
    result.replace(moduleStart, argumentsStart - moduleStart, moduleHelp);
    return result;
}

CommandLineParser::ExlusiveOptionValue CommandLineParser::parseExclusiveOptions(const QCommandLineOption &enableOption,
                                                                                const QCommandLineOption &disableOption)
{
    const bool enabled = m_parser.isSet(enableOption);
    const bool disabled = m_parser.isSet(disableOption);
    if (enabled) {
        if (disabled) {
            std::wcerr << "Warning: both -" << enableOption.names().first()
                       << " and -" << disableOption.names().first() << " were specified, defaulting to -"
                       << enableOption.names().first() << ".\n";
        }
        return OptionEnabled;
    }
    return disabled ? OptionDisabled : OptionAuto;
}

int CommandLineParser::parseArguments(const QStringList &arguments, Options *options, QString *errorMessage)
{
    typedef QSharedPointer<QCommandLineOption> CommandLineOptionPtr;
    typedef QPair<CommandLineOptionPtr, quint64> OptionMaskPair;
    typedef QVector<OptionMaskPair> OptionMaskVector;

    const QCommandLineOption helpOption = m_parser.addHelpOption();
    m_parser.addVersionOption();

    QCommandLineOption dirOption(QStringLiteral("dir"),
                                 QStringLiteral("Use directory instead of binary directory."),
                                 QStringLiteral("directory"));
    m_parser.addOption(dirOption);

    QCommandLineOption libDirOption(QStringLiteral("libdir"),
                                    QStringLiteral("Copy libraries to path."),
                                    QStringLiteral("path"));
    m_parser.addOption(libDirOption);

    QCommandLineOption debugOption(QStringLiteral("debug"),
                                   QStringLiteral("Assume debug binaries."));
    m_parser.addOption(debugOption);
    QCommandLineOption releaseOption(QStringLiteral("release"),
                                     QStringLiteral("Assume release binaries."));
    m_parser.addOption(releaseOption);
    QCommandLineOption releaseWithDebugInfoOption(QStringLiteral("release-with-debug-info"),
                                                  QStringLiteral("Assume release binaries with debug information."));
    m_parser.addOption(releaseWithDebugInfoOption);

    QCommandLineOption forceOption(QStringLiteral("force"),
                                   QStringLiteral("Force updating files."));
    m_parser.addOption(forceOption);

    QCommandLineOption dryRunOption(QStringLiteral("dry-run"),
                                    QStringLiteral("Simulation mode. Behave normally, but do not copy/update any files."));
    m_parser.addOption(dryRunOption);

    QCommandLineOption noPluginsOption(QStringLiteral("no-plugins"),
                                       QStringLiteral("Skip plugin deployment."));
    m_parser.addOption(noPluginsOption);

    QCommandLineOption noLibraryOption(QStringLiteral("no-libraries"),
                                       QStringLiteral("Skip library deployment."));
    m_parser.addOption(noLibraryOption);

    QCommandLineOption qmlDirOption(QStringLiteral("qmldir"),
                                    QStringLiteral("Scan for QML-imports starting from directory."),
                                    QStringLiteral("directory"));
    m_parser.addOption(qmlDirOption);

    QCommandLineOption noQuickImportOption(QStringLiteral("no-quick-import"),
                                           QStringLiteral("Skip deployment of Qt Quick imports."));
    m_parser.addOption(noQuickImportOption);

    QCommandLineOption noTranslationOption(QStringLiteral("no-translations"),
                                           QStringLiteral("Skip deployment of translations."));
    m_parser.addOption(noTranslationOption);

    QCommandLineOption noSystemD3DCompilerOption(QStringLiteral("no-system-d3d-compiler"),
                                                 QStringLiteral("Skip deployment of the system D3D compiler."));
    m_parser.addOption(noSystemD3DCompilerOption);


    QCommandLineOption compilerRunTimeOption(QStringLiteral("compiler-runtime"),
                                             QStringLiteral("Deploy compiler runtime (Desktop only)."));
    m_parser.addOption(compilerRunTimeOption);

    QCommandLineOption noCompilerRunTimeOption(QStringLiteral("no-compiler-runtime"),
                                               QStringLiteral("Do not deploy compiler runtime (Desktop only)."));
    m_parser.addOption(noCompilerRunTimeOption);

    QCommandLineOption webKitOption(QStringLiteral("webkit2"),
                                    QStringLiteral("Deployment of WebKit2 (web process)."));
    m_parser.addOption(webKitOption);

    QCommandLineOption noWebKitOption(QStringLiteral("no-webkit2"),
                                      QStringLiteral("Skip deployment of WebKit2."));
    m_parser.addOption(noWebKitOption);

    QCommandLineOption jsonOption(QStringLiteral("json"),
                                  QStringLiteral("Print to stdout in JSON format."));
    m_parser.addOption(jsonOption);

    QCommandLineOption angleOption(QStringLiteral("angle"),
                                   QStringLiteral("Force deployment of ANGLE."));
    m_parser.addOption(angleOption);

    QCommandLineOption noAngleOption(QStringLiteral("no-angle"),
                                     QStringLiteral("Disable deployment of ANGLE."));
    m_parser.addOption(noAngleOption);

    QCommandLineOption listOption(QStringLiteral("list"),
                                  QLatin1String("Print only the names of the files copied.\n"
                                                "Available options:\n"
                                                "  source:   absolute path of the source files\n"
                                                "  target:   absolute path of the target files\n"
                                                "  relative: paths of the target files, relative\n"
                                                "            to the target directory\n"
                                                "  mapping:  outputs the source and the relative\n"
                                                "            target, suitable for use within an\n"
                                                "            Appx mapping file"),
                                  QStringLiteral("option"));
    m_parser.addOption(listOption);

    QCommandLineOption verboseOption(QStringLiteral("verbose"),
                                     QStringLiteral("Verbose level."),
                                     QStringLiteral("level"));
    m_parser.addOption(verboseOption);

    m_parser.addPositionalArgument(QStringLiteral("[files]"),
                                  QStringLiteral("Binaries or directory containing the binary."));

    OptionMaskVector enabledModules;
    OptionMaskVector disabledModules;
    for (size_t i = 0; i < qtModuleEntryCount(); ++i) {
        const QString option = QLatin1String(qtModuleEntryByIndex(i).option);
        const QString name = QLatin1String(qtModuleEntryByIndex(i).libraryName);
        const QString enabledDescription = QStringLiteral("Add ") + name + QStringLiteral(" module.");
        CommandLineOptionPtr enabledOption(new QCommandLineOption(option, enabledDescription));
        m_parser.addOption(*enabledOption.data());
        enabledModules.push_back(OptionMaskPair(enabledOption, qtModuleEntryByIndex(i).module));

        const QString disabledDescription = QStringLiteral("Remove ") + name + QStringLiteral(" module.");
        CommandLineOptionPtr disabledOption(new QCommandLineOption(QStringLiteral("no-") + option,
                                                                   disabledDescription));
        disabledModules.push_back(OptionMaskPair(disabledOption, qtModuleEntryByIndex(i).module));
        m_parser.addOption(*disabledOption.data());
    }

    const bool success = m_parser.parse(arguments);
    if (m_parser.isSet(helpOption))
        return CommandLineParseHelpRequested;
    if (!success) {
        *errorMessage = m_parser.errorText();
        return CommandLineParseError;
    }

    options->libraryDirectory = m_parser.value(libDirOption);
    options->plugins = !m_parser.isSet(noPluginsOption);
    options->libraries = !m_parser.isSet(noLibraryOption);
    options->translations = !m_parser.isSet(noTranslationOption);
    options->systemD3dCompiler = !m_parser.isSet(noSystemD3DCompilerOption);
    options->quickImports = !m_parser.isSet(noQuickImportOption);

    if (m_parser.isSet(compilerRunTimeOption))
        options->compilerRunTime = true;
    else if (m_parser.isSet(noCompilerRunTimeOption))
        options->compilerRunTime = false;

    if (options->compilerRunTime && options->platform != WindowsMinGW && options->platform != Windows) {
        *errorMessage = QStringLiteral("Deployment of the compiler runtime is implemented for Desktop only.");
        return CommandLineParseError;
    }

    if (m_parser.isSet(releaseWithDebugInfoOption)) {
        options->debugMatchAll = true; // PE analysis will detect "debug", turn off matching.
        options->debugDetection = Options::DebugDetectionForceRelease;
    } else {
        switch (parseExclusiveOptions(debugOption, releaseOption)) {
        case OptionAuto:
            break;
        case OptionEnabled:
            options->debugDetection = Options::DebugDetectionForceDebug;
            break;
        case OptionDisabled:
            options->debugDetection = Options::DebugDetectionForceRelease;
            break;
        }
    }

    switch (parseExclusiveOptions(angleOption, noAngleOption)) {
    case OptionAuto:
        break;
    case OptionEnabled:
        options->angleDetection = Options::AngleDetectionForceOn;
        break;
    case OptionDisabled:
        options->angleDetection = Options::AngleDetectionForceOff;
        break;
    }

    m_optWebKit2 = parseExclusiveOptions(webKitOption, noWebKitOption);

    if (m_parser.isSet(forceOption))
        options->updateFileFlags |= ForceUpdateFile;
    if (m_parser.isSet(dryRunOption))
        options->updateFileFlags |= SkipUpdateFile;

    for (size_t i = 0; i < qtModuleEntryCount(); ++i) {
        if (m_parser.isSet(*enabledModules.at(int(i)).first.data()))
            options->additionalLibraries |= enabledModules.at(int(i)).second;
        if (m_parser.isSet(*disabledModules.at(int(i)).first.data()))
            options->disabledLibraries |= disabledModules.at(int(i)).second;
    }

    // Add some dependencies
    if (options->additionalLibraries & QtQuickModule)
        options->additionalLibraries |= QtQmlModule;
    if (options->additionalLibraries & QtHelpModule)
        options->additionalLibraries |= QtCLuceneModule;
    if (options->additionalLibraries & QtDesignerComponents)
        options->additionalLibraries |= QtDesignerModule;

    if (m_parser.isSet(listOption)) {
        const QString value = m_parser.value(listOption);
        if (value == QStringLiteral("source")) {
            options->list = ListSource;
        } else if (value == QStringLiteral("target")) {
            options->list = ListTarget;
        } else if (value == QStringLiteral("relative")) {
            options->list = ListRelative;
        } else if (value == QStringLiteral("mapping")) {
            options->list = ListMapping;
        } else {
            *errorMessage = QStringLiteral("Please specify a valid option for -list (source, target, relative, mapping).");
            return CommandLineParseError;
        }
    }

    if (m_parser.isSet(jsonOption) || options->list) {
        optVerboseLevel = 0;
        options->json = new JsonOutput;
    } else {
        if (m_parser.isSet(verboseOption)) {
            bool ok;
            const QString value = m_parser.value(verboseOption);
            optVerboseLevel = value.toInt(&ok);
            if (!ok || optVerboseLevel < 0) {
                *errorMessage = QStringLiteral("Invalid value \"%1\" passed for verbose level.").arg(value);
                return CommandLineParseError;
            }
        }
    }

    const QStringList posArgs = m_parser.positionalArguments();
    if (posArgs.isEmpty()) {
        *errorMessage = QStringLiteral("Please specify the binary or folder.");
        return CommandLineParseError | CommandLineParseHelpRequested;
    }

    if (m_parser.isSet(dirOption))
        options->directory = m_parser.value(dirOption);

    if (m_parser.isSet(qmlDirOption))
        options->qmlDirectories = m_parser.values(qmlDirOption);

    const QString &file = posArgs.front();
    const QFileInfo fi(QDir::cleanPath(file));
    if (!fi.exists()) {
        *errorMessage = msgFileDoesNotExist(file);
        return CommandLineParseError;
    }

    if (!options->directory.isEmpty() && !fi.isFile()) { // -dir was specified - expecting file.
        *errorMessage = QLatin1Char('"') + file + QStringLiteral("\" is not an executable file.");
        return CommandLineParseError;
    }

    if (fi.isFile()) {
        options->binaries.append(fi.absoluteFilePath());
        if (options->directory.isEmpty())
            options->directory = fi.absolutePath();
    } else {
        const QString binary = findBinary(fi.absoluteFilePath(), options->platform);
        if (binary.isEmpty()) {
            *errorMessage = QStringLiteral("Unable to find binary in \"") + file + QLatin1Char('"');
            return CommandLineParseError;
        }
        options->directory = fi.absoluteFilePath();
        options->binaries.append(binary);
    } // directory.

    // Remaining files or plugin directories
    for (int i = 1; i < posArgs.size(); ++i) {
        const QFileInfo fi(QDir::cleanPath(posArgs.at(i)));
        const QString path = fi.absoluteFilePath();
        if (!fi.exists()) {
            *errorMessage = msgFileDoesNotExist(path);
            return CommandLineParseError;
        }
        if (fi.isDir()) {
            const QStringList libraries =
                    findSharedLibraries(QDir(path), options->platform, MatchDebugOrRelease, QString());
            foreach (const QString &library, libraries)
                options->binaries.append(path + QLatin1Char('/') + library);
        } else {
            options->binaries.append(path);
        }
    }
    options->translationsDirectory = options->directory + QLatin1String("/translations");
    return 0;
}

QT_END_NAMESPACE
