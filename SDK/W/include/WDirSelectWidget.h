/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
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

