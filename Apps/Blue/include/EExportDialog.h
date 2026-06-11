/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_EExportDialog
#define H_EExportDialog

#include <CBD.h>

class EExportWidget;

class EExportDialog : public QDialog
{
    Q_OBJECT
public:
    EExportDialog( QWidget *pParent = nullptr );

    bool isAll();
    bool isCurrent();
    bool isImage();
    bool isXML();

protected:
    EExportWidget *pExportWidget;
};

class EExportWidget : public QWidget
{
    Q_OBJECT
public:
    EExportWidget( QWidget *pParent );

    QRadioButton *pAll;
    QRadioButton *pCurrent;
    QRadioButton *pImage;      
    QRadioButton *pXML;
};

#endif
