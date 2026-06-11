/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_LGraphicsScene
#define H_LGraphicsScene

#include "L.h"

class LFile;

/*!
 * \brief An editor for a \sa LFile that is based off of QGraphicsScene.
 *  
 * The main reason we extend QGraphicsScene is that we want to redirect 
 * events to the LFile. 
 *  
 * LFile will, in turn, redirect to the current \sa LLayer - if any. 
 * How LLayer handles these events is up to the LLayer. In some cases 
 * they are passed on to a child object but mostly they are processed 
 * in LLayer itself. 
 *  
 * \note The app will be more interested in interacting with the LFile. 
 *  
 * \author pharvey (4/5/23)
 */
class LGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    LGraphicsScene( LFile *p, const QRectF &r, QObject *pObject = nullptr );
    ~LGraphicsScene();

    LFile *getFile() { return pFile; }

signals:
    void signalPos( const QPointF & );

protected:
    LFile *pFile = nullptr;

    // events
    void mousePressEvent( QGraphicsSceneMouseEvent *pEvent ) override;
    void mouseMoveEvent( QGraphicsSceneMouseEvent *pEvent ) override;
    void mouseReleaseEvent( QGraphicsSceneMouseEvent *pEvent ) override;
    void dragEnterEvent( QGraphicsSceneDragDropEvent *pEvent ) override;
    void dragMoveEvent( QGraphicsSceneDragDropEvent *pEvent ) override;
    void dropEvent( QGraphicsSceneDragDropEvent *pEvent ) override;
    void contextMenuEvent( QGraphicsSceneContextMenuEvent *pEvent ) override;
    void mouseDoubleClickEvent( QGraphicsSceneMouseEvent *pEvent ) override;
    void drawBackground( QPainter *pPainter, const QRectF &rect ) override;
    void drawForeground( QPainter *pPainter, const QRectF &rect ) override;
};

#endif
