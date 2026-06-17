/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_LSRender
#define H_LSRender

#include "SSetting.h"

/*!
 * \brief Preferences/Context settings for PPaint.
 * 
 * \author pharvey (4/21/23)
 */
class LSRender : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(int nAntialiasing READ getRenderAntialiasing WRITE setRenderAntialiasing NOTIFY signalChanged)
    Q_PROPERTY(int nTextAntialiasing READ getRenderTextAntialiasing WRITE setRenderTextAntialiasing NOTIFY signalChanged)
    Q_PROPERTY(int nSmoothPixmapTransform READ getRenderSmoothPixmapTransform WRITE setRenderSmoothPixmapTransform NOTIFY signalChanged)
    Q_PROPERTY(int nLosslessImageRendering READ getRenderLosslessImageRendering WRITE setRenderLosslessImageRendering NOTIFY signalChanged)
public:
    LSRender();

    void setRenderAntialiasing( int );
    void setRenderTextAntialiasing( int );
    void setRenderSmoothPixmapTransform( int );
    void setRenderLosslessImageRendering( int );
    void setSelected( int, int, int, int );

    int getRenderAntialiasing()             { return nAntialiasing;             }
    int getRenderTextAntialiasing()         { return nTextAntialiasing;         }
    int getRenderSmoothPixmapTransform()    { return nSmoothPixmapTransform;    }
    int getRenderLosslessImageRendering()   { return nLosslessImageRendering;   }

    QWidget *getButton( QWidget * /* pParent */ ) { return nullptr; }
    QWidget *getToolBar( QWidget *pParent );
    QWidget *getPanel( QWidget *pParent );

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem );
    void doLoad( QDomElement *pdomElem );

protected:
    // RenderHints
    //      QPainter::setRenderHints()
    //      QGraphicsView::setRenderHints() 
    int nAntialiasing           = 1;
    int nTextAntialiasing       = 1;
    int nSmoothPixmapTransform  = 1;
    int nLosslessImageRendering = 1;
};

/*!
 * \brief Widget to edit LSRender.
 * 
 * \author pharvey (4/21/23)
 */
class LSRenderWidget : public QWidget
{
    Q_OBJECT
public:
    LSRenderWidget( QWidget *p, Qt::Orientation n );

public slots:
    void slotRefresh();

protected:
    LSRender *       pSetting;
    // nRenderHint
    QCheckBox *     pAntialiasing;
    QCheckBox *     pTextAntialiasing;
    QCheckBox *     pSmoothPixmapTransform;
    QCheckBox *     pLosslessImageRendering;

protected slots:
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    void slotRenderAntialiasing( Qt::CheckState );
    void slotRenderTextAntialiasing( Qt::CheckState );
    void slotRenderSmoothPixmapTransform( Qt::CheckState );
    void slotRenderLosslessImageRendering( Qt::CheckState );
#else
    void slotRenderAntialiasing( int );
    void slotRenderTextAntialiasing( int );
    void slotRenderSmoothPixmapTransform( int );
    void slotRenderLosslessImageRendering( int );
#endif
};

#endif


