
#include "qtmodules.h"

static QtModuleEntry qtModuleEntries[] = {
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
static const size_t qtModulesCount = sizeof(qtModuleEntries)/sizeof(QtModuleEntry);

QtModuleEntry qtModuleEntryByIndex(int idx)
{
    Q_ASSERT((idx >= 0) && (idx < qtModulesCount));
    return qtModuleEntries[idx];
}

size_t qtModuleEntryCount()
{
    return qtModulesCount;
}
