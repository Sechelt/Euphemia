/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_WColorsPickerWidget
#define H_WColorsPickerWidget

#include "W.h"

class WColorsWidget;

class WColorsSpectrumWidget : public QWidget
{
    Q_OBJECT
public:
    WColorsSpectrumWidget( const QColor &color, QWidget *pParent );

    void setColor( const QColor & ); 

    QColor getColor() { return color; }

signals:
    void signalValueChanged( const QColor & );

protected:
    QPoint point;
    QColor color;

    void mousePressEvent( QMouseEvent *pEvent ) override;
    void paintEvent( QPaintEvent *pEvent ) override;

private:
    struct Polar
    {
        qreal radius;
        qreal angle;    // radians
    };

    Polar cartesianToPolar(qreal x, qreal y);
};

class WColorsPickerSwatchWidget : public QWidget
{
    Q_OBJECT
public:
    WColorsPickerSwatchWidget( const QColor &color, QWidget *pParent );

public slots:
    void slotColor( const QColor & ); 

protected:
    QColor color;

    void paintEvent( QPaintEvent *pEvent ) override;
};

/*!
 * \brief Color picker widget ie for a color picker dialog.
 *  
 * This combines; 
 *  
 *      - color spectrum widget for picking a color
 *      - color palette widget for picking a color
 *      - controls for selecting HSVA for picking a color
 *      - color swatch to see example of current selection
 *  
 * \author pharvey (2/21/23)
 */
class WColorsPickerWidget : public QWidget
{
    Q_OBJECT
public:
    WColorsPickerWidget( const QColor &color, QWidget *pParent, bool bShowPaletteColor = false );

    void setColor( const QColor & );

    QColor getColor() { return color; }

protected:
    QColor color;
    WColorsSpectrumWidget *pSpectrum;
    WColorsPickerSwatchWidget *pSwatch;
    WColorsWidget *pPalette = nullptr;

    QSlider *pSliderHue;
    QSlider *pSliderSat;
    QSlider *pSliderVal;
    QSlider *pSliderAlp;

    QSpinBox *pSpinHue;
    QSpinBox *pSpinSat;
    QSpinBox *pSpinVal;
    QSpinBox *pSpinAlp;

protected slots:
    void slotSpectrum( const QColor & );
    void slotPalette( const QColor & );
    void slotHue( int );
    void slotSat( int );
    void slotVal( int );
    void slotAlp( int );
};

#endif 

