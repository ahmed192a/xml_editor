#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QFileSystemModel>
#include <QApplication>
#include <QListView>
#include <QTreeView>

#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionopen_file_triggered();

    void on_actionsave_as_triggered();

    void on_actionexit_triggered();

    void on_actioncopy_triggered();

    void on_actionpaste_triggered();

    void on_actioncut_triggered();

    void on_actionredo_triggered();

    void on_actionundo_triggered();

    void on_actionsave_triggered();

    void on_treeView_activated(const QModelIndex &index);

    void on_tabWidget_tabCloseRequested(int index);

    void on_actionnew_file_triggered();

    void on_actionopen_folder_triggered();

    void on_actionopen_folder_2_triggered();

    void on_actionto_json_triggered();

    void on_actionCheck_Error_triggered();

    void on_actiontojson_triggered();

    void on_actionFormat_triggered();

    void on_actionminify_triggered();

    void on_actioncorrect_errors_triggered();

    void on_actioncompress_triggered();

    void on_actionunCompress_triggered();

private:
    QDir dir;
    QFileSystemModel model;
    Ui::MainWindow *ui;
    QString currentfile = "";
    QStringList files ;
};
#endif // MAINWINDOW_H
