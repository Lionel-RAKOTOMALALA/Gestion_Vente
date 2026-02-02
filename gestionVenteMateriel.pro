QT       += core gui sql charts network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cashpage.cpp \
    chartsmanager.cpp \
    chartswidget.cpp \
    clientdialog.cpp \
    clientspage.cpp \
    connexion.cpp \
    dashboardpage.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    orderdialog.cpp \
    orderdetailsdialog.cpp \
    orderspage.cpp \
    paymentdialog.cpp \
    paymentspage.cpp \
    productdialog.cpp \
    productspage.cpp \
    receiptgenerator.cpp \
    sidebar.cpp \
    smtpclient.cpp \
    stockmovementpage.cpp \
    stylesheet.cpp \
    thememanager.cpp \
    userdialog.cpp \
    userspage.cpp

HEADERS += \
    cashpage.h \
    chartsmanager.h \
    chartswidget.h \
    clientdialog.h \
    clientspage.h \
    connexion.h \
    dashboardpage.h \
    logindialog.h \
    mainwindow.h \
    orderdialog.h \
    orderdetailsdialog.h \
    orderspage.h \
    paymentdialog.h \
    paymentspage.h \
    productdialog.h \
    productspage.h \
    receiptgenerator.h \
    sidebar.h \
    smtpclient.h \
    smtpconfig.h \
    stockmovementpage.h \
    stylesheet.h \
    thememanager.h \
    userdialog.h \
    userspage.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
