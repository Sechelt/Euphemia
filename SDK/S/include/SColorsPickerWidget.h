/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_SColorsPickerWidget
#define H_SColorsPickerWidget

#include "W.h"

class SColorsWidget;

class SColorsSpectrumWidget : public QWidget
{
    Q_OBJECT
public:
    SColorsSpectrumWidget( const QColor &color, QWidget *pParent );

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

class SColorsPickerSwatchWidget : public QWidget
{
    Q_OBJECT
public:
    SColorsPickerSwatchWidget( const QColor &color, QWidget *pParent );

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
class SColorsPickerWidget : public QWidget
{
    Q_OBJECT
public:
    SColorsPickerWidget( const QColor &color, QWidget *pParent, bool bShowPaletteColor = false );

    void setColor( const QColor & );

    QColor getColor() { return color; }

protected:
    QColor color;
    SColorsSpectrumWidget *pSpectrum;
    SColorsPickerSwatchWidget *pSwatch;
    SColorsWidget *pPalette = nullptr;

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

