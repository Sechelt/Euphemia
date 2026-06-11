/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_LToolsWidget
#define H_LToolsWidget

#include "LFile.h"

class WIconLayout;

class LToolsWidget : public QWidget
{
    Q_OBJECT
public:
    LToolsWidget( QWidget *pParent );

    void setLayers( LFile *p );

protected:
    LFile * pLayers = nullptr;
    LLayer *pLayer  = nullptr;

    QScrollArea *       pScrollArea         = nullptr;
    QWidget *           pScrollableWidget   = nullptr;
    QActionGroup *      pActionGroup        = nullptr;
    QVector<QAction*>   vectorActions;
    int                 nTool = -1;

    void doConnectLayers( LFile *p );
    void doDisconnectLayers();
    void doConnectLayer( LLayer *p );
    void doDisconnectLayer();
    void doConnectTool( int n );
    void doDisconnectTool();

    void doInitActions( QVector<LTool> vectorTools );
    void doFiniActions();
    void doInitButtons();
    void doFiniButtons();

protected slots:
    void slotTriggered( QAction * );
    void slotChangedCurrent();
    void slotChangedTool();
};

#endif
