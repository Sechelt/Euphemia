/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#ifndef H_WDirSelectWidget
#define H_WDirSelectWidget

#include "W.h"

class WDirSelectWidget : public QWidget
{
    Q_OBJECT
public:
    WDirSelectWidget( const QString &stringDirectory, QWidget *pWidgetParent );

    void setDirectory( const QString &s ) { pDirectory->setText( s ); }

    QString getDirectory() { return pDirectory->text(); }

protected:
    QLineEdit *     pDirectory;
    QPushButton *   pSelect;

protected slots:
    void slotSelect();
};

#endif 

