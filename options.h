#ifndef OPTIONS
#define OPTIONS

#include "types.h"

QT_BEGIN_NAMESPACE

class JsonOutput;

struct Options {
    enum DebugDetection {
        DebugDetectionAuto,
        DebugDetectionForceDebug,
        DebugDetectionForceRelease
    };

    enum AngleDetection {
        AngleDetectionAuto,
        AngleDetectionForceOn,
        AngleDetectionForceOff
    };

    Options() : plugins(true), libraries(true), quickImports(true), translations(true), systemD3dCompiler(true), compilerRunTime(false)
              , angleDetection(AngleDetectionAuto), platform(Windows), additionalLibraries(0), disabledLibraries(0)
              , updateFileFlags(0), json(0), list(ListNone), debugDetection(DebugDetectionAuto)
              , debugMatchAll(false) {}

    bool plugins;
    bool libraries;
    bool quickImports;
    bool translations;
    bool systemD3dCompiler;
    bool compilerRunTime;
    AngleDetection angleDetection;
    Platform platform;
    quint64 additionalLibraries;
    quint64 disabledLibraries;
    quint64 updateFileFlags;
    QStringList qmlDirectories; // Project's QML files.
    QString directory;
    QString translationsDirectory; // Translations target directory
    QString libraryDirectory;
    QStringList binaries;
    JsonOutput *json;
    ListOption list;
    DebugDetection debugDetection;
    bool debugMatchAll;

    static const char webKitProcessC[];
    static const char webEngineProcessC[];

    inline bool isWinRtOrWinPhone() const {
        return (platform == WinPhoneArm || platform == WinPhoneIntel
                || platform == WinRtArm || platform == WinRtIntel);
    }
};

QT_END_NAMESPACE

#endif // OPTIONS

