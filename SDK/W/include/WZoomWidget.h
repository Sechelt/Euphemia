/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#ifndef H_WZoomWidget
#define H_WZoomWidget

#include "W.h"

/*!
 * \class WZoomWidget 
 * \brief Allows zoom to be specified using a variety of controls. 
 *  
 * The controls that can be used are any combination of; 
 *  
 * - slider 
 * - +/- 
 * - to fit (width/height/all) 
 * 
 * \author pharvey (2/6/20)
 */
class WZoomWidget : public QWidget
{
    Q_OBJECT
public:
    enum FitTypes
    {
        FitWidth = 0,
        FitHeight,
        FitAll,
        FitIgnore
    };

    WZoomWidget( QWidget *pParent = nullptr, int nZoom = 100 );

    void setMin( int n );
    void setMax( int n );
    void setPage( int n );
    void setInc( int n );
    void setZoom( int n );
    void setFit( FitTypes n );

    void setVisibleFit( bool b = true );

    int getZoom();
    FitTypes getFit();


signals:
    void signalZoom( WZoomWidget::FitTypes nFit, int nZoom );

public slots:
    void slotRefresh( WZoomWidget::FitTypes nFit, int nZoom );

protected:
    int         nMin;
    int         nMax;
    int         nInc;
    int         nZoom;
    FitTypes    nFit;

    QSlider *       pSlider;
    QLineEdit *     pLineEdit;
    QIntValidator * pValidator;
    QToolButton *   pPlus;
    QToolButton *   pMinus;
    QComboBox *     pFit;

    QSize sizeHint() const override;

protected slots:
    void slotSlider( int nValue );
    void slotPlus();
    void slotMinus();
    void slotLineEdit();
    void slotFit( int nIndex );
};

#endif 

