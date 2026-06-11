/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_WColorsWidget
#define H_WColorsWidget

#include "S.h"

/*!
 * \brief Presents the global color palette for color selection/modification.
 *  
 * This a UI for SColors - specifically - its single global instance \sa g_SColors. 
 *  
 * - view state of palette 
 * - add/remove/edit color (triggeres SColors::signalModified())
 * - select a color (triggers WColorsMatrix::signalSelected())
 * 
 * \author pharvey (11/30/22)
 */
class WColorsMatrix : public QWidget
{
    Q_OBJECT
public:
    WColorsMatrix( QWidget *pParent );

signals:
    void signalSelected( const QColor & );

public slots:
    void slotRefresh(); // connect to g_Palette->colors.signalModified()

protected:
    QVector<QColor> vectorColors;

    void mousePressEvent( QMouseEvent *pEvent ) override;
    void mouseDoubleClickEvent( QMouseEvent *pEvent ) override;
    void paintEvent( QPaintEvent *pEvent ) override;

private:
    int getIndex( const QPoint & );
};

/*!
 * \brief Combines a WColorsMatrix with a tool bar to allow save/load/reset. 
 * 
 * \author pharvey (12/3/22)
 */
class WColorsWidget : public QWidget
{
    Q_OBJECT
public:
    WColorsWidget( QWidget *pParent );

signals:
    void signalSelected( const QColor & );

public slots:
    void slotRefresh( const QColor & );

protected:
    QToolButton *   pLoad;  
    QToolButton *   pSave;  
    QToolButton *   pSaveAs;
    QToolButton *   pReset; 
    QLabel *        pModified;

protected slots:
    void slotRefresh();

};

#endif 

