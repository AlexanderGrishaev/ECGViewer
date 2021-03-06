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

    void on_comboBox_currentIndexChanged(const QString &scaleFactorText);

    void on_comboBox_2_currentIndexChanged(const QString &sweepFactorText);

    void on_horizontalScrollBar_valueChanged(int value);

    void on_pushButton_clicked();

    void on_horizontalSlider_sliderMoved(int position);

    void on_horizontalSlider_2_sliderMoved(int position);

private:
    Ui::MainWindow *ui;
    QString mFileName;
    GraphicAreaWidget * mpGraphicAreaWidget;

    edf_hdr_struct mEDFHeader;

    int percent0;
    int percent1;

};

#endif // MAINWINDOW_H
