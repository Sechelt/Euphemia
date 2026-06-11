/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#ifndef H_WFileSelectWidget
#define H_WFileSelectWidget

#include "W.h"

class WFileSelectWidget : public QWidget
{
    Q_OBJECT
public:
    WFileSelectWidget( const QString &stringFileName, const QString &stringFileSpec, bool bNew, QWidget *pWidgetParent );

    void setFileName( const QString &s ) { pFileName->setText( s ); }

    QString getFileName() { return pFileName->text(); }

protected:
    QString         stringFileSpec;
    QLineEdit *     pFileName;
    QPushButton *   pNew = nullptr;
    QPushButton *   pOpen;

protected slots:
    void slotNew();
    void slotOpen();
};

#endif 

