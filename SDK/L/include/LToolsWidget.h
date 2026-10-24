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

/*!
 * \class LToolsWidget
 * \brief Displays the current tool actions. 
 * 
 * \author pharvey (2026-10-20)
 */
class LToolsWidget : public QSplitter
{
    Q_OBJECT
public:
    LToolsWidget( QWidget *pParent );

    void setLayers( LFile *p );

protected:
    LFile * pLayers = nullptr;
    LLayer *pLayer  = nullptr;

    QSplitter *         pSplitter               = nullptr;

    QScrollArea *       pScrollAreaTools        = nullptr;
    QWidget *           pScrollableWidgetTools  = nullptr;
    QActionGroup *      pActionGroup            = nullptr;
    QVector<QAction*>   vectorActions;
    int                 nTool = -1;

    QScrollArea *       pScrollAreaPanel        = nullptr;
    QWidget *           pScrollableWidgetPanel  = nullptr;

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

    void doInitPanel();
    void doFiniPanel();

protected slots:
    void slotTriggered( QAction * );
    void slotChangedCurrent();
    void slotChangedTool();
};

#endif
