#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSignalMapper>

class Sidebar : public QWidget
{
    Q_OBJECT

public:
    explicit Sidebar(const QString &userRole, QWidget *parent = nullptr);

signals:
    void pageChanged(int pageIndex);
    void logoutRequested();

public slots:
    void updateTheme();

private:
    QVBoxLayout *layout;
    QSignalMapper *mapper;
};

#endif // SIDEBAR_H