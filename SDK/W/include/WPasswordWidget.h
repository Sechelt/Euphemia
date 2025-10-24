/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_WPasswordWidget
#define H_WPasswordWidget

#include "W.h"

/*!
 * \brief Widget for editing a password.
 * 
 * Includes a button to toggle echo mode.
 * 
 * \author pharvey (1/14/25)
 */
class WPasswordWidget : public QWidget
{
    Q_OBJECT
public:
    WPasswordWidget( const QString &stringPassword, QWidget *pWidgetParent );

    void setPassword( const QString &s ) { pPassword->setText( s ); }

    QString getPassword() { return pPassword->text(); }

protected:
    QLineEdit *     pPassword;
    QToolButton *   pEchoMode;

protected slots:
    void slotEchoMode( bool );
};

#endif 

