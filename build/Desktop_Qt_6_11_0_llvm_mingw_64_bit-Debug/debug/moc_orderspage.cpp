/****************************************************************************
** Meta object code from reading C++ file 'orderspage.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../orderspage.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'orderspage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.0. It"
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
struct qt_meta_tag_ZN10OrdersPageE_t {};
} // unnamed namespace

template <> constexpr inline auto OrdersPage::qt_create_metaobjectdata<qt_meta_tag_ZN10OrdersPageE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "OrdersPage",
        "0$RAr/stO5CAi3GMhEC5YXyhhsgbI",
        "onSearchTextChanged",
        "",
        "text",
        "onStatusFilterChanged",
        "status",
        "onPaymentFilterChanged",
        "filter",
        "onRefreshClicked",
        "onViewOrderDetails",
        "row",
        "column",
        "onEditOrder",
        "commandeId",
        "onDeleteOrder",
        "onNotifyUnpaidOrder",
        "orderId",
        "onFirstPageClicked",
        "onPreviousPageClicked",
        "onNextPageClicked",
        "onLastPageClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onSearchTextChanged'
        QtMocHelpers::SlotData<void(const QString &)>(2, 3, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 4 },
        }}),
        // Slot 'onStatusFilterChanged'
        QtMocHelpers::SlotData<void(const QString &)>(5, 3, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Slot 'onPaymentFilterChanged'
        QtMocHelpers::SlotData<void(const QString &)>(7, 3, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
        // Slot 'onRefreshClicked'
        QtMocHelpers::SlotData<void()>(9, 3, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onViewOrderDetails'
        QtMocHelpers::SlotData<void(int, int)>(10, 3, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 11 }, { QMetaType::Int, 12 },
        }}),
        // Slot 'onEditOrder'
        QtMocHelpers::SlotData<void(const QString &)>(13, 3, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Slot 'onDeleteOrder'
        QtMocHelpers::SlotData<void(const QString &)>(15, 3, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Slot 'onNotifyUnpaidOrder'
        QtMocHelpers::SlotData<void(int)>(16, 3, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 17 },
        }}),
        // Slot 'onFirstPageClicked'
        QtMocHelpers::SlotData<void()>(18, 3, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPreviousPageClicked'
        QtMocHelpers::SlotData<void()>(19, 3, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onNextPageClicked'
        QtMocHelpers::SlotData<void()>(20, 3, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLastPageClicked'
        QtMocHelpers::SlotData<void()>(21, 3, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    uint qt_metaObjectHashIndex = 1;
    return QtMocHelpers::metaObjectData<OrdersPage, qt_meta_tag_ZN10OrdersPageE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums, qt_metaObjectHashIndex);
}
Q_CONSTINIT const QMetaObject OrdersPage::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10OrdersPageE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10OrdersPageE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10OrdersPageE_t>.metaTypes,
    nullptr
} };

void OrdersPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<OrdersPage *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onSearchTextChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->onStatusFilterChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->onPaymentFilterChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->onRefreshClicked(); break;
        case 4: _t->onViewOrderDetails((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 5: _t->onEditOrder((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->onDeleteOrder((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->onNotifyUnpaidOrder((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->onFirstPageClicked(); break;
        case 9: _t->onPreviousPageClicked(); break;
        case 10: _t->onNextPageClicked(); break;
        case 11: _t->onLastPageClicked(); break;
        default: ;
        }
    }
}

const QMetaObject *OrdersPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OrdersPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10OrdersPageE_t>.strings))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int OrdersPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
