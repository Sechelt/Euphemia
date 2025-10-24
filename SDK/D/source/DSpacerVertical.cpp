#include "LibInfo.h"
#include "DSpacerVertical.h"

DSpacerVertical::DSpacerVertical(ADObject *pObjectParent, const QString &stringName)
    : DSpacer(pObjectParent, stringName) {
}

DSpacerVertical::~DSpacerVertical() {
}

QPixmap DSpacerVertical::getIcon() {
    return QPixmap(":D/VSpacer");
}

void DSpacerVertical::paintAt(QPainter *pPainter, const QPointF &pointPos) {
    pPainter->setPen(pen);

    QRectF rect;
    rect.setTopLeft(pointPos);
    rect.setSize(this->rect.size());

    switch (nSizeType) {
    case DSpacer::SizeExpanding:
        {
            qreal nX1   = rect.width() / 2 - 4;
            qreal nX2   = rect.width() / 2 + 4;
            qreal nStep = 3;

            for (qreal nY = 0; nY < rect.height(); nY += nStep) {
                pPainter->drawLine(nX1, nY, nX2, nY + 1);
            }
        }
        break;
    case DSpacer::SizeFixed:
        {
            qreal nX = rect.width() / 2;
            QPen penThick = pen;
            penThick.setWidth(4);
            pPainter->setPen(penThick);
            pPainter->drawLine(nX, 0, nX, rect.height());
        }
        break;
    }
}


