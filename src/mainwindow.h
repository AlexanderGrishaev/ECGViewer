#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "graphicareawidget.h"
#include "EDFlib/edflib.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionExit_triggered();

    void on_actionLoad_triggered();

private:
    Ui::MainWindow *ui;
    QString mFileName;
    GraphicAreaWidget * mpGraphicAreaWidget;

    edf_hdr_struct mEDFHeader;

};

#endif // MAINWINDOW_H
