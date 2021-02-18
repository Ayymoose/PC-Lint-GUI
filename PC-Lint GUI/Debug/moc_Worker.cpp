/****************************************************************************
** Meta object code from reading C++ file 'Worker.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../Worker.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QSet>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Worker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Worker_t {
    QByteArrayData data[20];
    char stringdata0[257];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Worker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Worker_t qt_meta_stringdata_Worker = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Worker"
QT_MOC_LITERAL(1, 7, 18), // "signalLintFinished"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 13), // "LINTER_STATUS"
QT_MOC_LITERAL(4, 41, 6), // "status"
QT_MOC_LITERAL(5, 48, 14), // "signalFinished"
QT_MOC_LITERAL(6, 63, 15), // "signalStartLint"
QT_MOC_LITERAL(7, 79, 19), // "signalSetLinterData"
QT_MOC_LITERAL(8, 99, 8), // "LintData"
QT_MOC_LITERAL(9, 108, 8), // "lintData"
QT_MOC_LITERAL(10, 117, 13), // "slotParseData"
QT_MOC_LITERAL(11, 131, 16), // "slotLintMessages"
QT_MOC_LITERAL(12, 148, 17), // "QSet<LintMessage>"
QT_MOC_LITERAL(13, 166, 12), // "lintMessages"
QT_MOC_LITERAL(14, 179, 18), // "slotLinterProgress"
QT_MOC_LITERAL(15, 198, 5), // "value"
QT_MOC_LITERAL(16, 204, 13), // "slotStartLint"
QT_MOC_LITERAL(17, 218, 5), // "start"
QT_MOC_LITERAL(18, 224, 15), // "slotGetLintData"
QT_MOC_LITERAL(19, 240, 16) // "slotLintFinished"

    },
    "Worker\0signalLintFinished\0\0LINTER_STATUS\0"
    "status\0signalFinished\0signalStartLint\0"
    "signalSetLinterData\0LintData\0lintData\0"
    "slotParseData\0slotLintMessages\0"
    "QSet<LintMessage>\0lintMessages\0"
    "slotLinterProgress\0value\0slotStartLint\0"
    "start\0slotGetLintData\0slotLintFinished"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Worker[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x06 /* Public */,
       5,    0,   67,    2, 0x06 /* Public */,
       6,    0,   68,    2, 0x06 /* Public */,
       7,    1,   69,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,   72,    2, 0x0a /* Public */,
      11,    1,   73,    2, 0x0a /* Public */,
      14,    1,   76,    2, 0x0a /* Public */,
      16,    1,   79,    2, 0x0a /* Public */,
      18,    1,   82,    2, 0x0a /* Public */,
      19,    1,   85,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void, QMetaType::Bool,   17,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 3,    4,

       0        // eod
};

void Worker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Worker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->signalLintFinished((*reinterpret_cast< LINTER_STATUS(*)>(_a[1]))); break;
        case 1: _t->signalFinished(); break;
        case 2: _t->signalStartLint(); break;
        case 3: _t->signalSetLinterData((*reinterpret_cast< const LintData(*)>(_a[1]))); break;
        case 4: _t->slotParseData(); break;
        case 5: _t->slotLintMessages((*reinterpret_cast< QSet<LintMessage>(*)>(_a[1]))); break;
        case 6: _t->slotLinterProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->slotStartLint((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->slotGetLintData((*reinterpret_cast< const LintData(*)>(_a[1]))); break;
        case 9: _t->slotLintFinished((*reinterpret_cast< LINTER_STATUS(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Worker::*)(LINTER_STATUS );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Worker::signalLintFinished)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Worker::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Worker::signalFinished)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Worker::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Worker::signalStartLint)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Worker::*)(const LintData & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Worker::signalSetLinterData)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Worker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Worker.data,
    qt_meta_data_Worker,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Worker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Worker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Worker.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Worker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void Worker::signalLintFinished(LINTER_STATUS _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Worker::signalFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Worker::signalStartLint()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Worker::signalSetLinterData(const LintData & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
