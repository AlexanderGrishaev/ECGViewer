#include "graphicareawidget.h"
#include <QPainter>

GraphicAreaWidget::GraphicAreaWidget(QWidget *parent) : QWidget(parent) {
    mScalingFactor = 100.0;
    mSweepFactor = 30.0;
    mScroll = 0.0;
    mpEDFHeader = nullptr;
    setMouseTracking(true);
    mRepaint = false;
    startTimer(50);
}

void GraphicAreaWidget::setEDFHeader(edf_hdr_struct *pEDFHeader) {
    mpEDFHeader = pEDFHeader;
    mChannels.resize(mpEDFHeader->edfsignals);
    for (int i = 0; i < mChannels.size(); i++) {
        if (mChannels[i].scalingFactor == 0) {
            mChannels[i].scalingFactor = mScalingFactor;
        }
    }
    mRepaint = true;
}

void GraphicAreaWidget::setData(qint32 channelIndex, QByteArray doubleSamples)
{
    if (channelIndex < mChannels.size()) {
        mChannels[channelIndex].index = channelIndex;
        mChannels[channelIndex].samples = doubleSamples;
    }
}

void GraphicAreaWidget::setScalingFactor(qreal scalingFactor)
{
    mScalingFactor = scalingFactor;
    for (int i = 0; i < mChannels.size(); i++) {
        mChannels[i].scalingFactor = mScalingFactor;
    }
    mRepaint = true;
}

void GraphicAreaWidget::setSweepFactor(qreal sweepFactor)
{
    mSweepFactor = sweepFactor;
    mRepaint = true;
}

void GraphicAreaWidget::setScroll(qreal part)
{
    mScroll = part;
    mRepaint = true;
}

void GraphicAreaWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
    painter.drawRect(0, 0, width(), height());

    if (mpEDFHeader == nullptr || mpEDFHeader->edfsignals <= 0) {
        // ничего не делаем
        return;
    }

    qreal magicPowerScaler = 5.0;
    qreal magicTimeScaler = 60.0;

    painter.setPen(Qt::black);

    qint32 screenHeight = height();
    qint32 screenWidth = width();
    qint32 channelHeight = screenHeight / mpEDFHeader->edfsignals;

    for (qint32 channel = 0; channel < qint32(mpEDFHeader->edfsignals); channel++)
    {
        qint32 startY = channelHeight / 2 + channelHeight * channel;
        if (channel < mChannels.size() && mChannels[channel].samples.size() > 0)
        {
            qreal scale = magicPowerScaler * qreal(mpEDFHeader->signalparam[channel].dig_max - mpEDFHeader->signalparam[channel].dig_min) /
                    ((mpEDFHeader->signalparam[channel].phys_max - mpEDFHeader->signalparam[channel].phys_min) * mChannels[channel].scalingFactor);

            double * pData = (double *) mChannels[channel].samples.data();
            qint32 samplesCountAll = mChannels[channel].samples.size() / sizeof(double);
            qint32 samplesViewPort = qreal(screenWidth) * magicTimeScaler / mSweepFactor;

            if (samplesViewPort < 1) {
                samplesViewPort = 1;
            }

            qint32 startSampleIndex = qint32(mScroll * qreal(samplesCountAll));
            if (startSampleIndex >= samplesCountAll - 1) {
                startSampleIndex = samplesCountAll - 2;
            }

            qint32 endSampleIndex = startSampleIndex + samplesViewPort;

            if (endSampleIndex >= samplesCountAll) {
                endSampleIndex = samplesCountAll - 1;
            }

            QPoint * points = nullptr;

            if (samplesViewPort > screenWidth) {
                QPoint * points = new QPoint[screenWidth*2];
                qint32 pointCount = 0;
                qint32 xPrev = 0;
                qint32 yMax = 0;
                qint32 yMin = screenHeight;
                for (qint32 sampleIndex = startSampleIndex; sampleIndex < endSampleIndex; sampleIndex++)
                {
                    qint32 x = screenWidth * (sampleIndex - startSampleIndex) / samplesViewPort;
                    if (x < 0) x = 0;
                    if (x > screenWidth - 1) x = screenWidth - 1;

                    qint32 y = startY + pData[sampleIndex] * scale;
                    if (y < 0) y = 0;
                    if (y > screenHeight - 1) y = screenHeight - 1;

                    if (yMin > y) yMin = y;
                    if (yMax < y) yMax = y;

                    if (x != xPrev) {
                        if (yMin == yMax) {
                            points[pointCount].setX(x);
                            points[pointCount].setY(yMin);
                            pointCount++;
                        } else {
                            points[pointCount].setX(x);
                            points[pointCount].setY(yMin);
                            pointCount++;

                            points[pointCount].setX(x);
                            points[pointCount].setY(yMax);
                            pointCount++;
                        }
                        xPrev = x;
                        yMax = 0;
                        yMin = screenHeight;
                    }
                }
                painter.drawPolyline(points, pointCount);
            } else {
                QPoint * points = new QPoint[endSampleIndex - startSampleIndex];

                for (qint32 sampleIndex = startSampleIndex; sampleIndex < endSampleIndex; sampleIndex++)
                {
                    qint32 x = (qint32)((qreal)screenWidth * (qreal)(sampleIndex - startSampleIndex) / (qreal)samplesViewPort);
                    if (x < 0) x = 0;
                    if (x > screenWidth - 1) x = screenWidth - 1;

                    qint32 y = startY + (qint32)(pData[sampleIndex] * scale);
                    if (y < 0) y = 0;
                    if (y > screenHeight - 1) y = screenHeight - 1;
                    points[sampleIndex - startSampleIndex].setX(x);
                    points[sampleIndex - startSampleIndex].setY(y);
                }
                painter.drawPolyline(points, endSampleIndex - startSampleIndex);
            }
            delete[] points;
        }
    }
}

void GraphicAreaWidget::mouseMoveEvent(QMouseEvent *pEvent) {
    mRepaint = true;
}

void GraphicAreaWidget::timerEvent(QTimerEvent *event)
{
    if (mRepaint) {
        repaint();
    }
}
