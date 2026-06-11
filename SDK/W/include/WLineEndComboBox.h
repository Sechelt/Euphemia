/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#ifndef WLINEENDCOMBOBOX_H
#define WLINEENDCOMBOBOX_H

#include "W.h"

#include "W.h"

class WLineEndComboBox : public QComboBox
{
    Q_OBJECT
public:
    WLineEndComboBox( W::WLineEndStyles n, QWidget *pParent );

    void setValue( W::WLineEndStyles n );
    W::WLineEndStyles getValue();

protected:
    QPixmap getIcon( W::WLineEndStyles n, int width = 32, int height = 32 );

signals:
    void signalChanged( W::WLineEndStyles n );

private slots:
    void slotChanged( int nIndex );
};

#endif 

