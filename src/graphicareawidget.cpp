#include "graphicareawidget.h"
#include <QPainter>

GraphicAreaWidget::GraphicAreaWidget(QWidget *parent) : QWidget(parent) {
    mScalingFactor = 100.0;
    mpEDFHeader = nullptr;
    setMouseTracking(true);
}

void GraphicAreaWidget::setEDFHeader(edf_hdr_struct *pEDFHeader) {
    mpEDFHeader = pEDFHeader;
    mChannels.resize(mpEDFHeader->edfsignals);
    for (int i = 0; i < mChannels.size(); i++) {
        if (mChannels[i].scalingFactor == 0) {
            mChannels[i].scalingFactor = mScalingFactor;
        }
    }
    repaint();
}

void GraphicAreaWidget::setData(quint32 channelIndex, QByteArray doubleSamples)
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
    repaint();
}

void GraphicAreaWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
    painter.drawRect(0, 0, width(), height());

    if (mpEDFHeader == nullptr || mpEDFHeader->edfsignals <= 0) {
        // ничего не делаем
        return;
    }

    qreal magicScaler = 5.0;

    painter.setPen(Qt::black);

    quint32 screenHeight = height();
    quint32 screenWidth = width();
    quint32 channelHeight = screenHeight / mpEDFHeader->edfsignals;

    for (quint32 channel = 0; channel < quint32(mpEDFHeader->edfsignals); channel++)
    {
        quint32 startY = channelHeight / 2 + channelHeight * channel;
        if (channel < mChannels.size() && mChannels[channel].samples.size() > 0)
        {
            qreal scale = magicScaler * qreal(mpEDFHeader->signalparam[channel].dig_max - mpEDFHeader->signalparam[channel].dig_min) /
                    ((mpEDFHeader->signalparam[channel].phys_max - mpEDFHeader->signalparam[channel].phys_min) * mChannels[channel].scalingFactor);

            double * pData = (double *) mChannels[channel].samples.data();
            quint32 samplesCount = mChannels[channel].samples.size() / sizeof(double);
            QPoint * points = nullptr;

            if (samplesCount > screenWidth) {
                QPoint * points = new QPoint[screenWidth*2];
                quint32 pointCount = 0;
                quint32 xPrev = 0;
                qint32 yMax = 0;
                qint32 yMin = screenHeight;
                for (quint32 sampleIndex = 0; sampleIndex < samplesCount; sampleIndex++)
                {
                    qint32 x = screenWidth * sampleIndex / samplesCount;
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
                QPoint * points = new QPoint[samplesCount];

                for (quint32 sampleIndex = 0; sampleIndex < samplesCount; sampleIndex++)
                {
                    qint32 x = screenWidth * sampleIndex / samplesCount;
                    qint32 y = startY + pData[sampleIndex] * scale;
                    if (y < 0) y = 0;
                    if (y > screenHeight - 1) y = screenHeight - 1;
                    points[sampleIndex].setX(x);
                    points[sampleIndex].setY(y);
                }
                painter.drawPolyline(points, samplesCount);
            }
            delete points;
        }
    }
}

void GraphicAreaWidget::mouseMoveEvent(QMouseEvent *pEvent) {
    repaint();
}
