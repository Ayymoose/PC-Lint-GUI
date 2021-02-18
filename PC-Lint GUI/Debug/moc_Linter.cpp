/****************************************************************************
** Meta object code from reading C++ file 'Linter.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../Linter.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Linter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Linter_t {
    QByteArrayData data[17];
    char stringdata0[237];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Linter_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Linter_t qt_meta_stringdata_Linter = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Linter"
QT_MOC_LITERAL(1, 7, 20), // "signalUpdateProgress"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 5), // "value"
QT_MOC_LITERAL(4, 35, 23), // "signalUpdateProgressMax"
QT_MOC_LITERAL(5, 59, 15), // "signalUpdateETA"
QT_MOC_LITERAL(6, 75, 3), // "eta"
QT_MOC_LITERAL(7, 79, 26), // "signalUpdateProcessedFiles"
QT_MOC_LITERAL(8, 106, 14), // "processedFiles"
QT_MOC_LITERAL(9, 121, 20), // "signalLinterProgress"
QT_MOC_LITERAL(10, 142, 18), // "signalLintFinished"
QT_MOC_LITERAL(11, 161, 12), // "LintResponse"
QT_MOC_LITERAL(12, 174, 12), // "lintResponse"
QT_MOC_LITERAL(13, 187, 13), // "slotStartLint"
QT_MOC_LITERAL(14, 201, 17), // "slotGetLinterData"
QT_MOC_LITERAL(15, 219, 8), // "LintData"
QT_MOC_LITERAL(16, 228, 8) // "lintData"

    },
    "Linter\0signalUpdateProgress\0\0value\0"
    "signalUpdateProgressMax\0signalUpdateETA\0"
    "eta\0signalUpdateProcessedFiles\0"
    "processedFiles\0signalLinterProgress\0"
    "signalLintFinished\0LintResponse\0"
    "lintResponse\0slotStartLint\0slotGetLinterData\0"
    "LintData\0lintData"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Linter[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       4,    1,   57,    2, 0x06 /* Public */,
       5,    1,   60,    2, 0x06 /* Public */,
       7,    1,   63,    2, 0x06 /* Public */,
       9,    1,   66,    2, 0x06 /* Public */,
      10,    1,   69,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      13,    0,   72,    2, 0x0a /* Public */,
      14,    1,   73,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, 0x80000000 | 11,   12,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 15,   16,

       0        // eod
};

void Linter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Linter *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->signalUpdateProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->signalUpdateProgressMax((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->signalUpdateETA((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->signalUpdateProcessedFiles((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->signalLinterProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->signalLintFinished((*reinterpret_cast< const LintResponse(*)>(_a[1]))); break;
        case 6: _t->slotStartLint(); break;
        case 7: _t->slotGetLinterData((*reinterpret_cast< const LintData(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Linter::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Linter::signalUpdateProgress)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Linter::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Linter::signalUpdateProgressMax)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Linter::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Linter::signalUpdateETA)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Linter::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Linter::signalUpdateProcessedFiles)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Linter::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Linter::signalLinterProgress)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (Linter::*)(const LintResponse & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Linter::signalLintFinished)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Linter::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Linter.data,
    qt_meta_data_Linter,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Linter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Linter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Linter.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Linter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void Linter::signalUpdateProgress(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Linter::signalUpdateProgressMax(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Linter::signalUpdateETA(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Linter::signalUpdateProcessedFiles(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Linter::signalLinterProgress(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Linter::signalLintFinished(const LintResponse & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
