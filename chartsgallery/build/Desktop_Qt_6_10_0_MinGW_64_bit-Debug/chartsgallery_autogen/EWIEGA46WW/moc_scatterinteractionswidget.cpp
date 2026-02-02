/****************************************************************************
** Meta object code from reading C++ file 'scatterinteractionswidget.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../scatterinteractionswidget.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'scatterinteractionswidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN25ScatterInteractionsWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto ScatterInteractionsWidget::qt_create_metaobjectdata<qt_meta_tag_ZN25ScatterInteractionsWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ScatterInteractionsWidget",
        "handleClickedPoint",
        "",
        "QPointF",
        "point"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'handleClickedPoint'
        QtMocHelpers::SlotData<void(const QPointF &)>(1, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ScatterInteractionsWidget, qt_meta_tag_ZN25ScatterInteractionsWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ScatterInteractionsWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<ContentWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25ScatterInteractionsWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25ScatterInteractionsWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN25ScatterInteractionsWidgetE_t>.metaTypes,
    nullptr
} };

void ScatterInteractionsWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ScatterInteractionsWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->handleClickedPoint((*reinterpret_cast<std::add_pointer_t<QPointF>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *ScatterInteractionsWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScatterInteractionsWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25ScatterInteractionsWidgetE_t>.strings))
        return static_cast<void*>(this);
    return ContentWidget::qt_metacast(_clname);
}

int ScatterInteractionsWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ContentWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}
QT_WARNING_POP
