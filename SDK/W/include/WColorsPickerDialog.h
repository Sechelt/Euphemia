/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_WColorsPickerDialog
#define H_WColorsPickerDialog

#include "W.h"

class WColorsPickerWidget;

class WColorsPickerDialog : public QDialog
{
    Q_OBJECT
public:
    WColorsPickerDialog( const QColor &color, QWidget *pParent = nullptr, bool bShowPaletteColor = false );
    ~WColorsPickerDialog();

    QColor getColor();
    static QColor getColor( bool *pOk, const QColor &color, QWidget *pParent, bool bShowPaletteColor = false );

public slots:
    void slotColorPicker();                     /*!< enter 'color picker' mode                                  */

protected:
    bool                    bColorPicker = false;           /*!< true if we are in 'color picker' mode                      */
    bool                    bShowPaletteColor;
    WColorsPickerWidget *  pColorPickerWidget;

    void mousePressEvent( QMouseEvent *pEvent ) override;

    void doLoadState();
    void doSaveState();
};

#endif 

