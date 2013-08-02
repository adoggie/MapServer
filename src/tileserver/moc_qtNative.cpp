/****************************************************************************
** Meta object code from reading C++ file 'qtNative.h'
**
** Created: Mon Jun 24 12:51:23 2013
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qtNative.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtNative.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TileServerHelper[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
      31,   17,   17,   17, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_TileServerHelper[] = {
    "TileServerHelper\0\0tileRender()\0"
    "getBitmapTile_Direct()\0"
};

const QMetaObject TileServerHelper::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_TileServerHelper,
      qt_meta_data_TileServerHelper, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TileServerHelper::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TileServerHelper::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TileServerHelper::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TileServerHelper))
        return static_cast<void*>(const_cast< TileServerHelper*>(this));
    return QThread::qt_metacast(_clname);
}

int TileServerHelper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: tileRender(); break;
        case 1: getBitmapTile_Direct(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void TileServerHelper::tileRender()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
