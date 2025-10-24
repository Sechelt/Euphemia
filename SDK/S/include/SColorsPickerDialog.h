/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_SColorsPickerDialog
#define H_SColorsPickerDialog

#include "W.h"

class SColorsPickerWidget;

class SColorsPickerDialog : public QDialog
{
    Q_OBJECT
public:
    SColorsPickerDialog( const QColor &color, QWidget *pParent = nullptr, bool bShowPaletteColor = false );
    ~SColorsPickerDialog();

    QColor getColor();
    static QColor getColor( bool *pOk, const QColor &color, QWidget *pParent, bool bShowPaletteColor = false );

public slots:
    void slotColorPicker();                     /*!< enter 'color picker' mode                                  */

protected:
    bool                    bColorPicker = false;           /*!< true if we are in 'color picker' mode                      */
    bool                    bShowPaletteColor;
    SColorsPickerWidget *  pColorPickerWidget;

    void mousePressEvent( QMouseEvent *pEvent ) override;

    void doLoadState();
    void doSaveState();
};

#endif 

