/****************************************************************************
** Meta object code from reading C++ file 'pccpentool.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../pccpentool.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pccpentool.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10PccPenToolE_t {};
} // unnamed namespace

template <> constexpr inline auto PccPenTool::qt_create_metaobjectdata<qt_meta_tag_ZN10PccPenToolE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "PccPenTool",
        "changed",
        "",
        "showColorDialog",
        "updateWidth",
        "width",
        "updateStyle",
        "style",
        "updateCapStyle",
        "index",
        "updateJoinStyle"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'changed'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'showColorDialog'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'updateWidth'
        QtMocHelpers::SlotData<void(double)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 5 },
        }}),
        // Slot 'updateStyle'
        QtMocHelpers::SlotData<void(int)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 7 },
        }}),
        // Slot 'updateCapStyle'
        QtMocHelpers::SlotData<void(int)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 9 },
        }}),
        // Slot 'updateJoinStyle'
        QtMocHelpers::SlotData<void(int)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 9 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<PccPenTool, qt_meta_tag_ZN10PccPenToolE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject PccPenTool::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10PccPenToolE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10PccPenToolE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10PccPenToolE_t>.metaTypes,
    nullptr
} };

void PccPenTool::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<PccPenTool *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->changed(); break;
        case 1: _t->showColorDialog(); break;
        case 2: _t->updateWidth((*reinterpret_cast<std::add_pointer_t<double>>(_a[1]))); break;
        case 3: _t->updateStyle((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->updateCapStyle((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->updateJoinStyle((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (PccPenTool::*)()>(_a, &PccPenTool::changed, 0))
            return;
    }
}

const QMetaObject *PccPenTool::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PccPenTool::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10PccPenToolE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int PccPenTool::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void PccPenTool::changed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
