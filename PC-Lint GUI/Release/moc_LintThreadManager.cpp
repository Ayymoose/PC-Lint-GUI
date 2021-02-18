/****************************************************************************
** Meta object code from reading C++ file 'LintThreadManager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../LintThreadManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LintThreadManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_LintThreadManager_t {
    QByteArrayData data[16];
    char stringdata0[233];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LintThreadManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LintThreadManager_t qt_meta_stringdata_LintThreadManager = {
    {
QT_MOC_LITERAL(0, 0, 17), // "LintThreadManager"
QT_MOC_LITERAL(1, 18, 19), // "signalGetLinterData"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 19), // "signalSetLinterData"
QT_MOC_LITERAL(4, 59, 8), // "LintData"
QT_MOC_LITERAL(5, 68, 8), // "lintData"
QT_MOC_LITERAL(6, 77, 15), // "signalStartLint"
QT_MOC_LITERAL(7, 93, 5), // "start"
QT_MOC_LITERAL(8, 99, 18), // "signalLintFinished"
QT_MOC_LITERAL(9, 118, 12), // "LintResponse"
QT_MOC_LITERAL(10, 131, 12), // "lintResponse"
QT_MOC_LITERAL(11, 144, 18), // "signalLintComplete"
QT_MOC_LITERAL(12, 163, 17), // "slotSetLinterData"
QT_MOC_LITERAL(13, 181, 20), // "slotStartLintManager"
QT_MOC_LITERAL(14, 202, 13), // "slotAbortLint"
QT_MOC_LITERAL(15, 216, 16) // "slotLintFinished"

    },
    "LintThreadManager\0signalGetLinterData\0"
    "\0signalSetLinterData\0LintData\0lintData\0"
    "signalStartLint\0start\0signalLintFinished\0"
    "LintResponse\0lintResponse\0signalLintComplete\0"
    "slotSetLinterData\0slotStartLintManager\0"
    "slotAbortLint\0slotLintFinished"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LintThreadManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x06 /* Public */,
       3,    1,   60,    2, 0x06 /* Public */,
       6,    1,   63,    2, 0x06 /* Public */,
       8,    1,   66,    2, 0x06 /* Public */,
      11,    0,   69,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    1,   70,    2, 0x0a /* Public */,
      13,    0,   73,    2, 0x0a /* Public */,
      14,    0,   74,    2, 0x0a /* Public */,
      15,    1,   75,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,   10,

       0        // eod
};

void LintThreadManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LintThreadManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->signalGetLinterData(); break;
        case 1: _t->signalSetLinterData((*reinterpret_cast< const LintData(*)>(_a[1]))); break;
        case 2: _t->signalStartLint((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->signalLintFinished((*reinterpret_cast< const LintResponse(*)>(_a[1]))); break;
        case 4: _t->signalLintComplete(); break;
        case 5: _t->slotSetLinterData((*reinterpret_cast< const LintData(*)>(_a[1]))); break;
        case 6: _t->slotStartLintManager(); break;
        case 7: _t->slotAbortLint(); break;
        case 8: _t->slotLintFinished((*reinterpret_cast< const LintResponse(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (LintThreadManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LintThreadManager::signalGetLinterData)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (LintThreadManager::*)(const LintData & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LintThreadManager::signalSetLinterData)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (LintThreadManager::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LintThreadManager::signalStartLint)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (LintThreadManager::*)(const LintResponse & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LintThreadManager::signalLintFinished)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (LintThreadManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LintThreadManager::signalLintComplete)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject LintThreadManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_LintThreadManager.data,
    qt_meta_data_LintThreadManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *LintThreadManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LintThreadManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LintThreadManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int LintThreadManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void LintThreadManager::signalGetLinterData()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void LintThreadManager::signalSetLinterData(const LintData & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void LintThreadManager::signalStartLint(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void LintThreadManager::signalLintFinished(const LintResponse & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void LintThreadManager::signalLintComplete()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
