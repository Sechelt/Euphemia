/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#ifndef H_WCanvasSizeWidget
#define H_WCanvasSizeWidget

#include "W.h"

/*!
 * \brief Manage canvas size. 
 *  
 * The canvas size can be; 
 *  
 * - based upon current printer page size
 * - set manually in pixels 
 * - fixed  
 *  
 * Size can not be changed when 'fixed' which makes this a view-only. Just disable this widget.
 *  
 * \author pharvey (8/1/20)
 */
class WCanvasSizeWidget : public QWidget
{
    Q_OBJECT
public:
    WCanvasSizeWidget( const QSizeF &size, bool bPrinter, QWidget *pParent );

signals:
    // to request change in object
    void signalSize( const QSizeF &size );
    void signalPrinter( bool );

public slots:
    // to register value in object (may be different than requested)
    void slotSize( const QSizeF &size );
    void slotPrinter( bool );

protected:
    QCheckBox *     pUsePrinter;
    QLineEdit *     pLineEditX;
    QLineEdit *     pLineEditY;

protected slots:
    void slotSize( const QString & );
};

#endif


