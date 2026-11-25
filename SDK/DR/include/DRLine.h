/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_DRLine
#define H_DRLine

#include "DRObject.h"

#include <LSLine.h>

class DRSLine;

class DRLine : public DRObject
{
    Q_OBJECT
public:
    DRLine( DRDraw *pCanvas, const QPointF &pointPos = QPointF() );
    virtual ~DRLine();

    virtual QPainterPath shape() const;

    virtual QPixmap     getIcon() override;
    virtual QRectF      getRect() const override { return QRectF( pointBegin, pointEnd ).normalized(); }
    virtual QImage      getImage() override;
    virtual LHandle *   getGrab( const QPointF &pointPos, bool bSizing = false ) override;
    QPointF             getBegin()  { return pointBegin;    }
    QPointF             getEnd()    { return pointEnd;      }

    virtual void doMoveBy( const QPointF &point ) override;
    virtual void doMove( LHandle *, const QPointF & ) override;
    virtual void doDrop( LHandle *, const QPointF &point = QPointF() ) override;

    // save/load
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent ) override;
    virtual bool        doLoad( QDomElement *pdomElem ) override;

protected:
    DRSLine *               pSettingLine;
    QPointF                 pointBegin;
    QPointF                 pointEnd;
    qreal                   nBeginSize  = 5;
    qreal                   nEndSize    = 5;
    LSLine::LineEndStyles   nBeginStyle = LSLine::LineEndNone;
    LSLine::LineEndStyles   nEndStyle   = LSLine::LineEndNone;

    // index of handles
    int nIndexHandleMove    = 0;
    int nIndexHandleBegin   = 1;
    int nIndexHandleEnd     = 2;

    virtual void doDraw( QPainter * ) override;
    virtual void doDrawLineEnds( QPainter *pPainter );
    virtual void doDrawLineEnd( QPainter *pPainter, LSLine::LineEndStyles nStyle, const QLineF &line );
    virtual void doDrawLineEndArrow( QPainter *pPainter, const QLineF &line );
    virtual void doDrawLineEndBall( QPainter *pPainter, const QLineF &line );

    virtual void doConnectSettings() override;
    virtual void doDisconnectSettings() override;

    virtual void doCreateHandlesStandard() override;
    virtual void doSyncHandlesStandard() override;

protected slots:
    virtual void slotRefreshLine();
};

/*!
 * \brief Settings for DRLine.
 * 
 * This does not contain its own settings - just brings in some useful settings from elsewhere.
 * 
 * \author pharvey (2026-11-24)
 */
class DRSLine : public DRSObject 
{
    Q_OBJECT
public:
    DRSLine();

    LSLine *pLine;

    QWidget *   getButton( QWidget *pParent ) override;
    QWidget *   getToolBar( QWidget *pParent ) override;
    QWidget *   getPanel( QWidget *pParent ) override;
    QWidget *   getSwatch( QWidget *pParent ) override;
    QToolBox *  getToolBox( QWidget *pParent ) override;    // return valid QToolBox

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) override;   // does nothing
    void doLoad( QDomElement *pdomElem ) override;                          // does nothing

};

#endif
