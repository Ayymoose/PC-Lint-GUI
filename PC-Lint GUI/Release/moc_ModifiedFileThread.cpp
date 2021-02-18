/****************************************************************************
** Meta object code from reading C++ file 'ModifiedFileThread.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../ModifiedFileThread.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ModifiedFileThread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ModifiedFileThread_t {
    QByteArrayData data[15];
    char stringdata0[229];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ModifiedFileThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ModifiedFileThread_t qt_meta_stringdata_ModifiedFileThread = {
    {
QT_MOC_LITERAL(0, 0, 18), // "ModifiedFileThread"
QT_MOC_LITERAL(1, 19, 14), // "signalFinished"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 18), // "signalFileModified"
QT_MOC_LITERAL(4, 54, 12), // "modifiedFile"
QT_MOC_LITERAL(5, 67, 21), // "signalFileDoesntExist"
QT_MOC_LITERAL(6, 89, 11), // "deletedFile"
QT_MOC_LITERAL(7, 101, 20), // "slotSetModifiedFiles"
QT_MOC_LITERAL(8, 122, 26), // "QMap<QString,ModifiedFile>"
QT_MOC_LITERAL(9, 149, 13), // "modifiedFiles"
QT_MOC_LITERAL(10, 163, 19), // "slotSetModifiedFile"
QT_MOC_LITERAL(11, 183, 8), // "dateTime"
QT_MOC_LITERAL(12, 192, 14), // "slotRemoveFile"
QT_MOC_LITERAL(13, 207, 12), // "slotKeepFile"
QT_MOC_LITERAL(14, 220, 8) // "keepFile"

    },
    "ModifiedFileThread\0signalFinished\0\0"
    "signalFileModified\0modifiedFile\0"
    "signalFileDoesntExist\0deletedFile\0"
    "slotSetModifiedFiles\0QMap<QString,ModifiedFile>\0"
    "modifiedFiles\0slotSetModifiedFile\0"
    "dateTime\0slotRemoveFile\0slotKeepFile\0"
    "keepFile"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ModifiedFileThread[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x06 /* Public */,
       3,    1,   50,    2, 0x06 /* Public */,
       5,    1,   53,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    1,   56,    2, 0x0a /* Public */,
      10,    2,   59,    2, 0x0a /* Public */,
      12,    1,   64,    2, 0x0a /* Public */,
      13,    1,   67,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString,    6,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, QMetaType::QString, QMetaType::QDateTime,    4,   11,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::QString,   14,

       0        // eod
};

void ModifiedFileThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ModifiedFileThread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->signalFinished(); break;
        case 1: _t->signalFileModified((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->signalFileDoesntExist((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->slotSetModifiedFiles((*reinterpret_cast< QMap<QString,ModifiedFile>(*)>(_a[1]))); break;
        case 4: _t->slotSetModifiedFile((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QDateTime(*)>(_a[2]))); break;
        case 5: _t->slotRemoveFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->slotKeepFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ModifiedFileThread::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ModifiedFileThread::signalFinished)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ModifiedFileThread::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ModifiedFileThread::signalFileModified)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ModifiedFileThread::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ModifiedFileThread::signalFileDoesntExist)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ModifiedFileThread::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_ModifiedFileThread.data,
    qt_meta_data_ModifiedFileThread,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ModifiedFileThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ModifiedFileThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ModifiedFileThread.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int ModifiedFileThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void ModifiedFileThread::signalFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ModifiedFileThread::signalFileModified(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ModifiedFileThread::signalFileDoesntExist(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
