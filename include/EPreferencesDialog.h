/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 *
 * This file is part of Euphemia.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef H_EPreferencesDialog
#define H_EPreferencesDialog

#include <WColorButton.h>
#include <WBrushStyleComboBox.h>
#include <WImageButton.h>

#include <PContext.h>

class EPreferencesDialog : public QDialog
{
    Q_OBJECT
public:
    EPreferencesDialog( QWidget *pParent = nullptr );

    static void doPreferences( QWidget *pParent );
};

class PGeneralConfig : public QWidget
{
    Q_OBJECT
public:
    PGeneralConfig( QWidget *pParent );

public slots:
    void slotRefresh( const PContextGeneral & );

protected:
    // brushBackground
    WColorButton *          pColor;         /*!< a transparent color (alpha==0) and no texture will implicitly indicate block pattern  */
    WBrushStyleComboBox *   pStyle;
    WImageButton *          pImage;

    QCheckBox *             pRestoreState;      
    QCheckBox *             pAutoCommit;      

protected slots:
    void slotColor( const QColor &color );
    void slotStyle( Qt::BrushStyle n );
    void slotImage();
    void slotRestoreState( int );
    void slotAutoCommit( int );
};

#endif
