/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_EStartLayerDialog
#define H_EStartLayerDialog

#include <CBD.h>

class EStartLayerWidget;

class EStartLayerDialog : public QDialog
{
    Q_OBJECT
public:
    EStartLayerDialog( QWidget *pParent = nullptr );

    bool isNone();
    bool isPaint();
    bool isDraw();

protected:
    EStartLayerWidget *pStartLayerWidget;
};

class EStartLayerWidget : public QWidget
{
    Q_OBJECT
public:
    EStartLayerWidget( QWidget *pParent );

    QRadioButton *pNone;
    QRadioButton *pPaint;
    QRadioButton *pDraw;      
};

#endif
