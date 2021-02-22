/****************************************************************************
** Meta object code from reading C++ file 'ukmedia_device_switch_widget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../ukui-volume-control-applet-qt/ukmedia_device_switch_widget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ukmedia_device_switch_widget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UkmediaTrayIcon_t {
    QByteArrayData data[3];
    char stringdata0[38];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UkmediaTrayIcon_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UkmediaTrayIcon_t qt_meta_stringdata_UkmediaTrayIcon = {
    {
QT_MOC_LITERAL(0, 0, 15), // "UkmediaTrayIcon"
QT_MOC_LITERAL(1, 16, 20), // "wheelRollEventSignal"
QT_MOC_LITERAL(2, 37, 0) // ""

    },
    "UkmediaTrayIcon\0wheelRollEventSignal\0"
    ""
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UkmediaTrayIcon[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void UkmediaTrayIcon::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UkmediaTrayIcon *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->wheelRollEventSignal((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (UkmediaTrayIcon::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UkmediaTrayIcon::wheelRollEventSignal)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject UkmediaTrayIcon::staticMetaObject = { {
    &QSystemTrayIcon::staticMetaObject,
    qt_meta_stringdata_UkmediaTrayIcon.data,
    qt_meta_data_UkmediaTrayIcon,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UkmediaTrayIcon::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UkmediaTrayIcon::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UkmediaTrayIcon.stringdata0))
        return static_cast<void*>(this);
    return QSystemTrayIcon::qt_metacast(_clname);
}

int UkmediaTrayIcon::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSystemTrayIcon::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void UkmediaTrayIcon::wheelRollEventSignal(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_MyTimer_t {
    QByteArrayData data[3];
    char stringdata0[17];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MyTimer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MyTimer_t qt_meta_stringdata_MyTimer = {
    {
QT_MOC_LITERAL(0, 0, 7), // "MyTimer"
QT_MOC_LITERAL(1, 8, 7), // "timeOut"
QT_MOC_LITERAL(2, 16, 0) // ""

    },
    "MyTimer\0timeOut\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MyTimer[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,

       0        // eod
};

void MyTimer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MyTimer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->timeOut(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MyTimer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MyTimer::timeOut)) {
                *result = 0;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject MyTimer::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_MyTimer.data,
    qt_meta_data_MyTimer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MyTimer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MyTimer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MyTimer.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MyTimer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void MyTimer::timeOut()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
struct qt_meta_stringdata_DeviceSwitchWidget_t {
    QByteArrayData data[50];
    char stringdata0[976];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DeviceSwitchWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DeviceSwitchWidget_t qt_meta_stringdata_DeviceSwitchWidget = {
    {
QT_MOC_LITERAL(0, 0, 18), // "DeviceSwitchWidget"
QT_MOC_LITERAL(1, 19, 18), // "app_volume_changed"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 6), // "isMute"
QT_MOC_LITERAL(4, 46, 6), // "volume"
QT_MOC_LITERAL(5, 53, 8), // "app_name"
QT_MOC_LITERAL(6, 62, 10), // "appBtnName"
QT_MOC_LITERAL(7, 73, 27), // "mouse_middle_clicked_signal"
QT_MOC_LITERAL(8, 101, 18), // "mouse_wheel_signal"
QT_MOC_LITERAL(9, 120, 4), // "step"
QT_MOC_LITERAL(10, 125, 15), // "app_name_signal"
QT_MOC_LITERAL(11, 141, 19), // "system_muted_signal"
QT_MOC_LITERAL(12, 161, 6), // "status"
QT_MOC_LITERAL(13, 168, 12), // "theme_change"
QT_MOC_LITERAL(14, 181, 11), // "font_change"
QT_MOC_LITERAL(15, 193, 23), // "deviceButtonClickedSlot"
QT_MOC_LITERAL(16, 217, 26), // "appVolumeButtonClickedSlot"
QT_MOC_LITERAL(17, 244, 27), // "activatedSystemTrayIconSlot"
QT_MOC_LITERAL(18, 272, 33), // "QSystemTrayIcon::ActivationRe..."
QT_MOC_LITERAL(19, 306, 6), // "reason"
QT_MOC_LITERAL(20, 313, 20), // "jumpControlPanelSlot"
QT_MOC_LITERAL(21, 334, 20), // "miniToAdvancedWidget"
QT_MOC_LITERAL(22, 355, 20), // "advancedToMiniWidget"
QT_MOC_LITERAL(23, 376, 24), // "deviceComboxIndexChanged"
QT_MOC_LITERAL(24, 401, 3), // "str"
QT_MOC_LITERAL(25, 405, 24), // "moveAdvanceSwitchBtnSlot"
QT_MOC_LITERAL(26, 430, 28), // "miniMastrerSliderChangedSlot"
QT_MOC_LITERAL(27, 459, 5), // "value"
QT_MOC_LITERAL(28, 465, 31), // "advancedSystemSliderChangedSlot"
QT_MOC_LITERAL(29, 497, 29), // "outputDeviceSliderChangedSlot"
QT_MOC_LITERAL(30, 527, 30), // "devWidgetMuteButtonClickedSlot"
QT_MOC_LITERAL(31, 558, 31), // "miniWidgetMuteButtonClickedSlot"
QT_MOC_LITERAL(32, 590, 30), // "appWidgetMuteButtonCLickedSlot"
QT_MOC_LITERAL(33, 621, 24), // "muteCheckBoxReleasedSlot"
QT_MOC_LITERAL(34, 646, 23), // "actionMuteTriggeredSLot"
QT_MOC_LITERAL(35, 670, 26), // "mouseMeddleClickedTraySlot"
QT_MOC_LITERAL(36, 697, 22), // "trayWheelRollEventSlot"
QT_MOC_LITERAL(37, 720, 19), // "miniWidgetWheelSlot"
QT_MOC_LITERAL(38, 740, 29), // "miniWidgetKeyboardPressedSlot"
QT_MOC_LITERAL(39, 770, 10), // "volumeGain"
QT_MOC_LITERAL(40, 781, 24), // "primaryScreenChangedSlot"
QT_MOC_LITERAL(41, 806, 8), // "QScreen*"
QT_MOC_LITERAL(42, 815, 6), // "screen"
QT_MOC_LITERAL(43, 822, 28), // "inputWidgetMuteButtonClicked"
QT_MOC_LITERAL(44, 851, 28), // "inputWidgetSliderChangedSlot"
QT_MOC_LITERAL(45, 880, 20), // "ukuiThemeChangedSlot"
QT_MOC_LITERAL(46, 901, 8), // "themeStr"
QT_MOC_LITERAL(47, 910, 19), // "fontSizeChangedSlot"
QT_MOC_LITERAL(48, 930, 20), // "osdDisplayWidgetHide"
QT_MOC_LITERAL(49, 951, 24) // "volumeSettingChangedSlot"

    },
    "DeviceSwitchWidget\0app_volume_changed\0"
    "\0isMute\0volume\0app_name\0appBtnName\0"
    "mouse_middle_clicked_signal\0"
    "mouse_wheel_signal\0step\0app_name_signal\0"
    "system_muted_signal\0status\0theme_change\0"
    "font_change\0deviceButtonClickedSlot\0"
    "appVolumeButtonClickedSlot\0"
    "activatedSystemTrayIconSlot\0"
    "QSystemTrayIcon::ActivationReason\0"
    "reason\0jumpControlPanelSlot\0"
    "miniToAdvancedWidget\0advancedToMiniWidget\0"
    "deviceComboxIndexChanged\0str\0"
    "moveAdvanceSwitchBtnSlot\0"
    "miniMastrerSliderChangedSlot\0value\0"
    "advancedSystemSliderChangedSlot\0"
    "outputDeviceSliderChangedSlot\0"
    "devWidgetMuteButtonClickedSlot\0"
    "miniWidgetMuteButtonClickedSlot\0"
    "appWidgetMuteButtonCLickedSlot\0"
    "muteCheckBoxReleasedSlot\0"
    "actionMuteTriggeredSLot\0"
    "mouseMeddleClickedTraySlot\0"
    "trayWheelRollEventSlot\0miniWidgetWheelSlot\0"
    "miniWidgetKeyboardPressedSlot\0volumeGain\0"
    "primaryScreenChangedSlot\0QScreen*\0"
    "screen\0inputWidgetMuteButtonClicked\0"
    "inputWidgetSliderChangedSlot\0"
    "ukuiThemeChangedSlot\0themeStr\0"
    "fontSizeChangedSlot\0osdDisplayWidgetHide\0"
    "volumeSettingChangedSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DeviceSwitchWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      34,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    4,  184,    2, 0x06 /* Public */,
       7,    0,  193,    2, 0x06 /* Public */,
       8,    1,  194,    2, 0x06 /* Public */,
      10,    1,  197,    2, 0x06 /* Public */,
      11,    1,  200,    2, 0x06 /* Public */,
      13,    0,  203,    2, 0x06 /* Public */,
      14,    0,  204,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      15,    0,  205,    2, 0x08 /* Private */,
      16,    0,  206,    2, 0x08 /* Private */,
      17,    1,  207,    2, 0x08 /* Private */,
      20,    0,  210,    2, 0x08 /* Private */,
      21,    0,  211,    2, 0x08 /* Private */,
      22,    0,  212,    2, 0x08 /* Private */,
      23,    1,  213,    2, 0x08 /* Private */,
      25,    0,  216,    2, 0x08 /* Private */,
      26,    1,  217,    2, 0x08 /* Private */,
      28,    1,  220,    2, 0x08 /* Private */,
      29,    1,  223,    2, 0x08 /* Private */,
      30,    0,  226,    2, 0x08 /* Private */,
      31,    0,  227,    2, 0x08 /* Private */,
      32,    0,  228,    2, 0x08 /* Private */,
      33,    0,  229,    2, 0x08 /* Private */,
      34,    0,  230,    2, 0x08 /* Private */,
      35,    0,  231,    2, 0x08 /* Private */,
      36,    1,  232,    2, 0x08 /* Private */,
      37,    1,  235,    2, 0x08 /* Private */,
      38,    1,  238,    2, 0x08 /* Private */,
      40,    1,  241,    2, 0x08 /* Private */,
      43,    0,  244,    2, 0x08 /* Private */,
      44,    1,  245,    2, 0x08 /* Private */,
      45,    1,  248,    2, 0x08 /* Private */,
      47,    1,  251,    2, 0x08 /* Private */,
      48,    0,  254,    2, 0x08 /* Private */,
      49,    0,  255,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool, QMetaType::Int, QMetaType::QString, QMetaType::QString,    3,    4,    5,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    9,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::Bool,   12,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 18,   19,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   24,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   27,
    QMetaType::Void, QMetaType::Int,   27,
    QMetaType::Void, QMetaType::Int,   27,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    9,
    QMetaType::Void, QMetaType::Bool,    9,
    QMetaType::Void, QMetaType::Int,   39,
    QMetaType::Void, 0x80000000 | 41,   42,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   27,
    QMetaType::Void, QMetaType::QString,   46,
    QMetaType::Void, QMetaType::QString,   46,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DeviceSwitchWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DeviceSwitchWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->app_volume_changed((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4]))); break;
        case 1: _t->mouse_middle_clicked_signal(); break;
        case 2: _t->mouse_wheel_signal((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->app_name_signal((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->system_muted_signal((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->theme_change(); break;
        case 6: _t->font_change(); break;
        case 7: _t->deviceButtonClickedSlot(); break;
        case 8: _t->appVolumeButtonClickedSlot(); break;
        case 9: _t->activatedSystemTrayIconSlot((*reinterpret_cast< QSystemTrayIcon::ActivationReason(*)>(_a[1]))); break;
        case 10: _t->jumpControlPanelSlot(); break;
        case 11: _t->miniToAdvancedWidget(); break;
        case 12: _t->advancedToMiniWidget(); break;
        case 13: _t->deviceComboxIndexChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 14: _t->moveAdvanceSwitchBtnSlot(); break;
        case 15: _t->miniMastrerSliderChangedSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->advancedSystemSliderChangedSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->outputDeviceSliderChangedSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: _t->devWidgetMuteButtonClickedSlot(); break;
        case 19: _t->miniWidgetMuteButtonClickedSlot(); break;
        case 20: _t->appWidgetMuteButtonCLickedSlot(); break;
        case 21: _t->muteCheckBoxReleasedSlot(); break;
        case 22: _t->actionMuteTriggeredSLot(); break;
        case 23: _t->mouseMeddleClickedTraySlot(); break;
        case 24: _t->trayWheelRollEventSlot((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 25: _t->miniWidgetWheelSlot((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 26: _t->miniWidgetKeyboardPressedSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 27: _t->primaryScreenChangedSlot((*reinterpret_cast< QScreen*(*)>(_a[1]))); break;
        case 28: _t->inputWidgetMuteButtonClicked(); break;
        case 29: _t->inputWidgetSliderChangedSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 30: _t->ukuiThemeChangedSlot((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 31: _t->fontSizeChangedSlot((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 32: _t->osdDisplayWidgetHide(); break;
        case 33: _t->volumeSettingChangedSlot(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DeviceSwitchWidget::*)(bool , int , QString , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DeviceSwitchWidget::app_volume_changed)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DeviceSwitchWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DeviceSwitchWidget::mouse_middle_clicked_signal)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (DeviceSwitchWidget::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DeviceSwitchWidget::mouse_wheel_signal)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (DeviceSwitchWidget::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DeviceSwitchWidget::app_name_signal)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (DeviceSwitchWidget::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DeviceSwitchWidget::system_muted_signal)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (DeviceSwitchWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DeviceSwitchWidget::theme_change)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (DeviceSwitchWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DeviceSwitchWidget::font_change)) {
                *result = 6;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DeviceSwitchWidget::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_DeviceSwitchWidget.data,
    qt_meta_data_DeviceSwitchWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DeviceSwitchWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DeviceSwitchWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceSwitchWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int DeviceSwitchWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 34)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 34;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 34)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 34;
    }
    return _id;
}

// SIGNAL 0
void DeviceSwitchWidget::app_volume_changed(bool _t1, int _t2, QString _t3, QString _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DeviceSwitchWidget::mouse_middle_clicked_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void DeviceSwitchWidget::mouse_wheel_signal(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void DeviceSwitchWidget::app_name_signal(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void DeviceSwitchWidget::system_muted_signal(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void DeviceSwitchWidget::theme_change()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void DeviceSwitchWidget::font_change()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
