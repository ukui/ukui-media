/****************************************************************************
** Meta object code from reading C++ file 'ukmedia_mini_master_volume_widget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../ukui-volume-control-applet-qt/ukmedia_mini_master_volume_widget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ukmedia_mini_master_volume_widget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UkmediaMiniMasterVolumeWidget_t {
    QByteArrayData data[7];
    char stringdata0[112];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UkmediaMiniMasterVolumeWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UkmediaMiniMasterVolumeWidget_t qt_meta_stringdata_UkmediaMiniMasterVolumeWidget = {
    {
QT_MOC_LITERAL(0, 0, 29), // "UkmediaMiniMasterVolumeWidget"
QT_MOC_LITERAL(1, 30, 18), // "mouse_wheel_signal"
QT_MOC_LITERAL(2, 49, 0), // ""
QT_MOC_LITERAL(3, 50, 4), // "step"
QT_MOC_LITERAL(4, 55, 23), // "keyboard_pressed_signal"
QT_MOC_LITERAL(5, 79, 10), // "volumeGain"
QT_MOC_LITERAL(6, 90, 21) // "moveMiniSwitchBtnSlot"

    },
    "UkmediaMiniMasterVolumeWidget\0"
    "mouse_wheel_signal\0\0step\0"
    "keyboard_pressed_signal\0volumeGain\0"
    "moveMiniSwitchBtnSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UkmediaMiniMasterVolumeWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       4,    1,   32,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   35,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Int,    5,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void UkmediaMiniMasterVolumeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UkmediaMiniMasterVolumeWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->mouse_wheel_signal((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->keyboard_pressed_signal((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->moveMiniSwitchBtnSlot(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (UkmediaMiniMasterVolumeWidget::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UkmediaMiniMasterVolumeWidget::mouse_wheel_signal)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (UkmediaMiniMasterVolumeWidget::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UkmediaMiniMasterVolumeWidget::keyboard_pressed_signal)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject UkmediaMiniMasterVolumeWidget::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_UkmediaMiniMasterVolumeWidget.data,
    qt_meta_data_UkmediaMiniMasterVolumeWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UkmediaMiniMasterVolumeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UkmediaMiniMasterVolumeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UkmediaMiniMasterVolumeWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int UkmediaMiniMasterVolumeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void UkmediaMiniMasterVolumeWidget::mouse_wheel_signal(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void UkmediaMiniMasterVolumeWidget::keyboard_pressed_signal(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
