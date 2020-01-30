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

    ChannelParams() {
        index = 0;
        scalingFactor = 20.0;
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
    void setData(quint32 channelIndex, QByteArray doubleSamples);

protected:
    // метод для отрисовки содержимого виджета
    void paintEvent(QPaintEvent *event);
    // реакция на движение мыши и нажатия клавиш мыши
    void mouseMoveEvent(QMouseEvent * pEvent);

signals:


public slots:

private:
    // заголовок файла EDF (если неопределен или ошибка открытия файла, то nullptr)
    edf_hdr_struct * mpEDFHeader;
    // параметры и данные каналов
    QVector<ChannelParams> mChannels;
};

#endif // GRAPHICAREAWIDGET_H
