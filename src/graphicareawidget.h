#ifndef GRAPHICAREAWIDGET_H
#define GRAPHICAREAWIDGET_H

#include <QWidget>
#include "EDFlib/edflib.h"

struct ChannelParams {
    // индекс канала
    quint32 index;
    // отсчеты
    QByteArray samples;
    // масштабирующий коэффициент
    qreal scalingFactor;
    // минимальное значение
    qreal minValue;
    // максимальное значение
    qreal maxValue;

    ChannelParams() {
        index = 0;
        scalingFactor = 0.0;
        minValue = 0;
        maxValue = 0;
    }
};

class GraphicAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GraphicAreaWidget(QWidget *parent = nullptr);

    // передача заголовка файла с параметрами записей
    void setEDFHeader(edf_hdr_struct * pEDFHeader);
    // передача массива отсчетов для каждого из каналов
    void setData(qint32 channelIndex, QByteArray doubleSamples);
    //
    void setScalingFactor(qreal scalingFactor);
    //
    void setSweepFactor(qreal sweepFactor);
    //
    void setScroll(qreal part);

protected:
    // метод для отрисовки содержимого виджета
    void paintEvent(QPaintEvent *event);
    // реакция на движение мыши и нажатия клавиш мыши
    void mouseMoveEvent(QMouseEvent * pEvent);
    //
    void timerEvent(QTimerEvent *event) override;

signals:


public slots:

private:
    // заголовок файла EDF (если неопределен или ошибка открытия файла, то nullptr)
    edf_hdr_struct * mpEDFHeader;
    // параметры и данные каналов
    QVector<ChannelParams> mChannels;
    // масштабирующий коэффициент по умолчанию
    qreal mScalingFactor;
    // разрешение по времени
    qreal mSweepFactor;
    // прокрутка по времени (0..1)
    qreal mScroll;
    //
    bool mRepaint;

};

#endif // GRAPHICAREAWIDGET_H
