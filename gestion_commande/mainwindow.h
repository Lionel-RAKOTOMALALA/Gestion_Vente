#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlQueryModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void reloadData();
    void affichDonnee();
    QSqlQueryModel *model = nullptr;
    QSqlQueryModel *modelClient = nullptr;
    ~MainWindow();

private slots:
    void on_AjoutBtn_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
