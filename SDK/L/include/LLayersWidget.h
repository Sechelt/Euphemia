/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_LLayersWidget
#define H_LLayersWidget

#include <AWTreeWidget.h>

#include "LFile.h"

class LLayersListWidget : public QListWidget
{
    Q_OBJECT
public:
    LLayersListWidget( QWidget *pParent );

    void setLayers( LFile * );

protected:
    LFile *pLayers = nullptr;

    void doConnectLayers( LFile *p );
    void doDisconnectLayers();

protected slots:
    void slotRefresh();

    void slotCurrentItemChanged( QListWidgetItem *, QListWidgetItem * );
    void slotContextMenu( const QPoint & );
};

/*!
 * \brief Controls to make requests to LFile.
 * 
 * - new layer (types provided by LFile)
 * - delete layer
 * - z-order of layer (move forward/backward)
 * - visibility of layer (on/off)
 * 
 * \author pharvey (6/6/24)
 */
class LLayersControlWidget : public QWidget
{
    Q_OBJECT
public:
    LLayersControlWidget( QWidget *pParent );
 
    void setLayers( LFile * );

protected:
    LFile *         pLayers = nullptr;
    QToolButton *   pVisible;
    QToolButton *   pBack;
    QToolButton *   pForward;
    QToolButton *   pAdd;
    QToolButton *   pRemove;

    void doConnectLayers( LFile *p );
    void doDisconnectLayers();

protected slots:
    void slotRefresh();

    void slotVisible();
    void slotBack();
    void slotForward();
    void slotAdd();
    void slotRemove();
};

/*!
 * \brief Shows file layers and allows them to be manipulated.
 * 
 * - dislays layers in a list
 * - new layer (types provided by LFile)
 * - delete layer
 * - z-order of layer (move forward/backward)
 * - visibility of layer (on/off)
 * 
 * \sa LLayersControlWidget
 * 
 * \author pharvey (6/6/24)
 */
class LLayersWidget : public QWidget
{
    Q_OBJECT
public:
    LLayersWidget( QWidget *pParent );

    void setLayers( LFile * ); // called by app mainwindow

protected:
    LLayersListWidget *     pListWidget;                /*! root is the LFile */
    LLayersControlWidget *  pControlWidget;
};

#endif
