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

QT_END_NAMESPACE

#endif // QTMODULES

