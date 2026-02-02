/****************************************************************************
** Meta object code from reading C++ file 'pccwidget.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../pccwidget.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pccwidget.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN9PccWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto PccWidget::qt_create_metaobjectdata<qt_meta_tag_ZN9PccWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "PccWidget",
        "updateChartSettings",
        "",
        "updateSerieSettings",
        "updateSliceSettings",
        "handleSliceClicked",
        "QPieSlice*",
        "slice",
        "showFontDialog",
        "appendSlice",
        "insertSlice",
        "removeSlice"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'updateChartSettings'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'updateSerieSettings'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'updateSliceSettings'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'handleSliceClicked'
        QtMocHelpers::SlotData<void(QPieSlice *)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 },
        }}),
        // Slot 'showFontDialog'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'appendSlice'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'insertSlice'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'removeSlice'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<PccWidget, qt_meta_tag_ZN9PccWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject PccWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<ContentWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9PccWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9PccWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9PccWidgetE_t>.metaTypes,
    nullptr
} };

void PccWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<PccWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->updateChartSettings(); break;
        case 1: _t->updateSerieSettings(); break;
        case 2: _t->updateSliceSettings(); break;
        case 3: _t->handleSliceClicked((*reinterpret_cast<std::add_pointer_t<QPieSlice*>>(_a[1]))); break;
        case 4: _t->showFontDialog(); break;
        case 5: _t->appendSlice(); break;
        case 6: _t->insertSlice(); break;
        case 7: _t->removeSlice(); break;
        default: ;
        }
    }
}

const QMetaObject *PccWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PccWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9PccWidgetE_t>.strings))
        return static_cast<void*>(this);
    return ContentWidget::qt_metacast(_clname);
}

int PccWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ContentWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
