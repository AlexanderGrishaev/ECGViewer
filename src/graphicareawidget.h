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
    //
    QByteArray maximums;
    //
    QByteArray minimums;
    //
    QByteArray maximumsCalculated;
    //
    QByteArray minimumsCalculated;

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

// массив давлений и задержек для каждого максимума ЭКГ
struct DelayAndPressure {
    // задержка в секундах
    double delayS;
    // нижнее давление, ммрс
    double minPressureMm;
    // верхнее давление, ммрс
    double maxPressureMm;
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
    void calc(int channelECG, int channelP, int channelABP);
    //
    void setPressureCalcPercent(int beginPercent, int endPercent);

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
    //
    double mAHi;
    double mALo;
    double mBHi;
    double mBLo;
    int mN;

    //
    int mBeginPercent;
    //
    int mEndPercent;

    //
    int mMouseX, mMouseY;

    //
    QList<DelayAndPressure> mDelayAndPressureList;
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
    // 0 автоматический подбор (хорошо работает для кардиограммы с ярковыраженными пиками))
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
