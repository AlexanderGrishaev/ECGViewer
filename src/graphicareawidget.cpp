#include "graphicareawidget.h"
#include <QPainter>

GraphicAreaWidget::GraphicAreaWidget(QWidget *parent) : QWidget(parent) {
    mScalingFactor = 20.0;
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

    painter.setPen(Qt::black);

    quint32 screenHeight = height();
    quint32 screenWidth = width();
    quint32 channelHeight = screenHeight / mpEDFHeader->edfsignals;

    for (quint32 channel = 0; channel < quint32(mpEDFHeader->edfsignals); channel++)
    {
        quint32 startY = channelHeight / 2 + channelHeight * channel;
        if (channel < mChannels.size() && mChannels[channel].samples.size() > 0)
        {
            quint32 samplesCount = mChannels[channel].samples.size() / sizeof(double);
            double * pData = (double *) mChannels[channel].samples.data();

            QPoint * points = new QPoint[samplesCount];

            for (quint32 sampleIndex = 0; sampleIndex < samplesCount; sampleIndex++)
            {
                quint32 x = screenWidth * sampleIndex / samplesCount;
                quint32 y = startY - pData[sampleIndex] * mChannels[channel].scalingFactor;
                points[sampleIndex].setX(x);
                points[sampleIndex].setY(y);
            }

            painter.drawPolyline(points, samplesCount);
            delete points;
        }
    }
}

void GraphicAreaWidget::mouseMoveEvent(QMouseEvent *pEvent) {
    repaint();
}
