/****************************************************************************
** Meta object code from reading C++ file 'legendwidget.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../legendwidget.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'legendwidget.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN12LegendWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto LegendWidget::qt_create_metaobjectdata<qt_meta_tag_ZN12LegendWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "LegendWidget",
        "toggleAttached",
        "",
        "toggleInteractive",
        "toggleBold",
        "toggleItalic",
        "addBarset",
        "removeBarset",
        "setLegendAlignment",
        "fontSizeChanged",
        "updateLegendLayout"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'toggleAttached'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'toggleInteractive'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'toggleBold'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'toggleItalic'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'addBarset'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'removeBarset'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'setLegendAlignment'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fontSizeChanged'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'updateLegendLayout'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<LegendWidget, qt_meta_tag_ZN12LegendWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject LegendWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<ContentWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12LegendWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12LegendWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12LegendWidgetE_t>.metaTypes,
    nullptr
} };

void LegendWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<LegendWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->toggleAttached(); break;
        case 1: _t->toggleInteractive(); break;
        case 2: _t->toggleBold(); break;
        case 3: _t->toggleItalic(); break;
        case 4: _t->addBarset(); break;
        case 5: _t->removeBarset(); break;
        case 6: _t->setLegendAlignment(); break;
        case 7: _t->fontSizeChanged(); break;
        case 8: _t->updateLegendLayout(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *LegendWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LegendWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12LegendWidgetE_t>.strings))
        return static_cast<void*>(this);
    return ContentWidget::qt_metacast(_clname);
}

int LegendWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ContentWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
