#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <QtCore/QCommandLineParser>

#include "options.h"

class CommandLineParser
{
public:
    // Helpers for exclusive options, "-foo", "--no-foo"
    enum ExlusiveOptionValue {
        OptionAuto,
        OptionEnabled,
        OptionDisabled
    };

    enum CommandLineParseFlag {
        CommandLineParseError = 0x1,
        CommandLineParseHelpRequested = 0x2
    };

public:
    static QString helpText(const QCommandLineParser &p);

    static ExlusiveOptionValue parseExclusiveOptions(const QCommandLineParser *parser,
                                                     const QCommandLineOption &enableOption,
                                                     const QCommandLineOption &disableOption);
public:
    CommandLineParser();

    int parseArguments(const QStringList &arguments, QCommandLineParser *parser,
                       Options *options, QString *errorMessage);

    ExlusiveOptionValue optWebKit2() const { return m_optWebKit2; }

private:
    ExlusiveOptionValue m_optWebKit2;
};

#endif // COMMANDLINEPARSER_H
