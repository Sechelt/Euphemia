/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
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
