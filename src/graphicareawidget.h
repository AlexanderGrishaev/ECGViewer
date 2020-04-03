#ifndef GRAPHICAREAWIDGET_H
#define GRAPHICAREAWIDGET_H

#include <QWidget>
#include "EDFlib/edflib.h"

#define MIN_HEART_RATE 30.0
#define MAX_HEART_RATE 200.0

struct ChannelParams {
    // индекс канала
    quint32 index;
    // отсчеты (double)
    QByteArray samples;
    // ЧСС (int)
    QByteArray heartRate;
    // Отставание по времени (только в канале плетизмограммы), с (double)
    QByteArray timeLag;

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
    //
    void calc(int channelECG, int channelP);

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
    // метод поиска пиков
    // pInSamples входной массив отсчетов
    // pHeartRate выходной массив пиков, ненулевое значение это измеренная ЧСС
    // maxInterval максимальная дистанция между пиками
    // inversion Инвертирование входных данных:
    // -1 инвертирование
    // 0 автоматический подбор (хорошо работает для картиограммы с ярковыраженными пиками))
    // 1 без инвертирования
    void findHeartRate(double * pInSamples, int * pHeartRate, int samplesCount, double sampleRate, int inversion);
    // индекс канала кардиограммы
    int mChannelECG;
    // индекс канала плетизмограммы
    int mChannelPlethism;
    //
    double getSampleRate(int channel);
    //
    void findTimeLag(int * pHeartRateECG, int samplesCountECG, double SampleRateECG, int * pHeartRateP, double * pTimeLag, int samplesCountP, double SampleRateP);


};

#endif // GRAPHICAREAWIDGET_H
