#ifndef QTMODULES
#define QTMODULES

#include "types.h"

QT_BEGIN_NAMESPACE

enum QtModule
#if defined(Q_COMPILER_CLASS_ENUM) || defined(Q_CC_MSVC)
    : quint64
#endif
{
    QtBluetoothModule         = 0x000000000001,
    QtCLuceneModule           = 0x000000000002,
    QtConcurrentModule        = 0x000000000004,
    QtCoreModule              = 0x000000000008,
    QtDeclarativeModule       = 0x000000000010,
    QtDesignerComponents      = 0x000000000020,
    QtDesignerModule          = 0x000000000040,
    QtGuiModule               = 0x000000000080,
    QtCluceneModule           = 0x000000000100,
    QtHelpModule              = 0x000000000200,
    QtMultimediaModule        = 0x000000000400,
    QtMultimediaWidgetsModule = 0x000000000800,
    QtMultimediaQuickModule   = 0x000000001000,
    QtNetworkModule           = 0x000000002000,
    QtNfcModule               = 0x000000004000,
    QtOpenGLModule            = 0x000000008000,
    QtPositioningModule       = 0x000000010000,
    QtPrintSupportModule      = 0x000000020000,
    QtQmlModule               = 0x000000040000,
    QtQuickModule             = 0x000000080000,
    QtQuickParticlesModule    = 0x000000100000,
    QtScriptModule            = 0x000000200000,
    QtScriptToolsModule       = 0x000000400000,
    QtSensorsModule           = 0x000000800000,
    QtSerialPortModule        = 0x000001000000,
    QtSqlModule               = 0x000002000000,
    QtSvgModule               = 0x000004000000,
    QtTestModule              = 0x000008000000,
    QtWidgetsModule           = 0x000010000000,
    QtWinExtrasModule         = 0x000020000000,
    QtXmlModule               = 0x000040000000,
    QtXmlPatternsModule       = 0x000080000000,
    QtWebKitModule            = 0x000100000000,
    QtWebKitWidgetsModule     = 0x000200000000,
    QtQuickWidgetsModule      = 0x000400000000,
    QtWebSocketsModule        = 0x000800000000,
    QtEnginioModule           = 0x001000000000,
    QtWebEngineCoreModule     = 0x002000000000,
    QtWebEngineModule         = 0x004000000000,
    QtWebEngineWidgetsModule  = 0x008000000000,
    QtQmlToolingModule        = 0x010000000000,
    Qt3DCoreModule            = 0x020000000000,
    Qt3DRendererModule        = 0x040000000000,
    Qt3DQuickModule           = 0x080000000000,
    Qt3DQuickRendererModule   = 0x100000000000,
    Qt3DInputModule           = 0x200000000000,
    QtLocationModule          = 0x400000000000,
    QtWebChannelModule        = 0x800000000000
};

struct QtModuleEntry {
    quint64 module;
    const char *option;
    const char *libraryName;
    const char *translation;
};

QtModuleEntry qtModuleEntries[] = {
    { QtBluetoothModule, "bluetooth", "Qt5Bluetooth", 0 },
    { QtCLuceneModule, "clucene", "Qt5CLucene", "qt_help" },
    { QtConcurrentModule, "concurrent", "Qt5Concurrent", "qtbase" },
    { QtCoreModule, "core", "Qt5Core", "qtbase" },
    { QtDeclarativeModule, "declarative", "Qt5Declarative", "qtquick1" },
    { QtDesignerModule, "designer", "Qt5Designer", 0 },
    { QtDesignerComponents, "designercomponents", "Qt5DesignerComponents", 0 },
    { QtEnginioModule, "enginio", "Enginio", 0 },
    { QtGuiModule, "gui", "Qt5Gui", "qtbase" },
    { QtHelpModule, "qthelp", "Qt5Help", "qt_help" },
    { QtMultimediaModule, "multimedia", "Qt5Multimedia", "qtmultimedia" },
    { QtMultimediaWidgetsModule, "multimediawidgets", "Qt5MultimediaWidgets", "qtmultimedia" },
    { QtMultimediaQuickModule, "multimediaquick", "Qt5MultimediaQuick_p", "qtmultimedia" },
    { QtNetworkModule, "network", "Qt5Network", "qtbase" },
    { QtNfcModule, "nfc", "Qt5Nfc", 0 },
    { QtOpenGLModule, "opengl", "Qt5OpenGL", 0 },
    { QtPositioningModule, "positioning", "Qt5Positioning", 0 },
    { QtPrintSupportModule, "printsupport", "Qt5PrintSupport", 0 },
    { QtQmlModule, "qml", "Qt5Qml", "qtdeclarative" },
    { QtQmlToolingModule, "qmltooling", "qmltooling", 0 },
    { QtQuickModule, "quick", "Qt5Quick", "qtdeclarative" },
    { QtQuickParticlesModule, "quickparticles", "Qt5QuickParticles", 0 },
    { QtQuickWidgetsModule, "quickwidgets", "Qt5QuickWidgets", 0 },
    { QtScriptModule, "script", "Qt5Script", "qtscript" },
    { QtScriptToolsModule, "scripttools", "Qt5ScriptTools", "qtscript" },
    { QtSensorsModule, "sensors", "Qt5Sensors", 0 },
    { QtSerialPortModule, "serialport", "Qt5SerialPort", 0 },
    { QtSqlModule, "sql", "Qt5Sql", "qtbase" },
    { QtSvgModule, "svg", "Qt5Svg", 0 },
    { QtTestModule, "test", "Qt5Test", "qtbase" },
    { QtWebKitModule, "webkit", "Qt5WebKit", 0 },
    { QtWebKitWidgetsModule, "webkitwidgets", "Qt5WebKitWidgets", 0 },
    { QtWebSocketsModule, "websockets", "Qt5WebSockets", 0 },
    { QtWidgetsModule, "widgets", "Qt5Widgets", "qtbase" },
    { QtWinExtrasModule, "winextras", "Qt5WinExtras", 0 },
    { QtXmlModule, "xml", "Qt5Xml", "qtbase" },
    { QtXmlPatternsModule, "xmlpatterns", "Qt5XmlPatterns", "qtxmlpatterns" },
    { QtWebEngineCoreModule, "webenginecore", "Qt5WebEngineCore", 0 },
    { QtWebEngineModule, "webengine", "Qt5WebEngine", 0 },
    { QtWebEngineWidgetsModule, "webenginewidgets", "Qt5WebEngineWidgets", 0 },
    { Qt3DCoreModule, "3dcore", "Qt53DCore", 0 },
    { Qt3DRendererModule, "3drenderer", "Qt53DRenderer", 0 },
    { Qt3DQuickModule, "3dquick", "Qt53DQuick", 0 },
    { Qt3DQuickRendererModule, "3dquickrenderer", "Qt53DQuickRenderer", 0 },
    { Qt3DInputModule, "3dinput", "Qt53DInput", 0 },
    { QtLocationModule, "geoservices", "Qt5Location", 0 },
    { QtWebChannelModule, "webchannel", "Qt5WebChannel", 0 }
};

inline QByteArray formatQtModules(quint64 mask, bool option = false)
{
    QByteArray result;
    const size_t qtModulesCount = sizeof(qtModuleEntries)/sizeof(QtModuleEntry);
    for (size_t i = 0; i < qtModulesCount; ++i) {
        if (mask & qtModuleEntries[i].module) {
            if (!result.isEmpty())
                result.append(' ');
            result.append(option ? qtModuleEntries[i].option : qtModuleEntries[i].libraryName);
        }
    }
    return result;
}

inline bool isQtModule(const QString &libName)
{
    // Match Standard modules, Qt5XX.dll, Qt[Commercial]Charts.dll and special cases.
    return libName.size() > 2
        && ((libName.startsWith(QLatin1String("Qt"), Qt::CaseInsensitive) && libName.at(2).isDigit())
            || libName.startsWith(QLatin1String("QtCommercial"), Qt::CaseInsensitive)
            || libName.startsWith(QLatin1String("QtCharts"), Qt::CaseInsensitive)
            || libName.startsWith(QLatin1String("DataVisualization"), Qt::CaseInsensitive)
            || libName.startsWith(QLatin1String("Enginio"), Qt::CaseInsensitive));
}

inline quint64 qtModuleForPlugin(const QString &subDirName)
{
    if (subDirName == QLatin1String("accessible") || subDirName == QLatin1String("iconengines")
        || subDirName == QLatin1String("imageformats") || subDirName == QLatin1String("platforms")
        || subDirName == QLatin1String("platforminputcontexts")) {
        return QtGuiModule;
    }
    if (subDirName == QLatin1String("bearer"))
        return QtNetworkModule;
    if (subDirName == QLatin1String("sqldrivers"))
        return QtSqlModule;
    if (subDirName == QLatin1String("audio") || subDirName == QLatin1String("mediaservice") || subDirName == QLatin1String("playlistformats"))
        return QtMultimediaModule;
    if (subDirName == QLatin1String("printsupport"))
        return QtPrintSupportModule;
    if (subDirName == QLatin1String("scenegraph"))
        return QtQuickModule;
    if (subDirName == QLatin1String("qmltooling"))
        return QtQuickModule | QtQmlToolingModule;
    if (subDirName == QLatin1String("qml1tooling"))
        return QtDeclarativeModule;
    if (subDirName == QLatin1String("position"))
        return QtPositioningModule;
    if (subDirName == QLatin1String("geoservices"))
        return QtLocationModule;
    if (subDirName == QLatin1String("sensors") || subDirName == QLatin1String("sensorgestures"))
        return QtSensorsModule;
    if (subDirName == QLatin1String("qtwebengine"))
        return QtWebEngineModule | QtWebEngineCoreModule | QtWebEngineWidgetsModule;
    if (subDirName == QLatin1String("sceneparsers"))
        return Qt3DRendererModule;
    return 0; // "designer"
}

inline quint64 qtModule(const QString &module)
{
    quint64 bestMatch = 0;
    int bestMatchLength = 0;
    const size_t qtModulesCount = sizeof(qtModuleEntries)/sizeof(QtModuleEntry);
    for (size_t i = 0; i < qtModulesCount; ++i) {
        const QString libraryName = QLatin1String(qtModuleEntries[i].libraryName);
        if (libraryName.size() > bestMatchLength && module.contains(libraryName, Qt::CaseInsensitive)) {
            bestMatch = qtModuleEntries[i].module;
            bestMatchLength = libraryName.size();
        }
    }
    return bestMatch;
}

inline QStringList translationNameFilters(quint64 modules, const QString &prefix)
{
    QStringList result;
    const size_t qtModulesCount = sizeof(qtModuleEntries)/sizeof(QtModuleEntry);
    for (size_t i = 0; i < qtModulesCount; ++i) {
        if ((qtModuleEntries[i].module & modules) && qtModuleEntries[i].translation) {
            const QString name = QLatin1String(qtModuleEntries[i].translation) +
                                 QLatin1Char('_') +  prefix + QStringLiteral(".qm");
            if (!result.contains(name))
                result.push_back(name);
        }
    }
    return result;
}

QT_END_NAMESPACE

#endif // QTMODULES

