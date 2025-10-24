/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef WICONWIDGET_H
#define WICONWIDGET_H

#include "W.h"

/*!
 * \brief Base class for shape icon to be used in tool box.
 *  
 * Derived classes will provide an icon, be able to draw a shape for dragging, etc 
 * The idea is that instances of derived classes can come from a plugin. 
 *  
 * \author pharvey (4/3/19)
 */
class WIconWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WIconWidget( const QString &stringText, const QPixmap &pixmapIcon, QWidget *pWidget, const QString &stringTip = QString() );
    virtual ~WIconWidget();

    virtual void setSelected( bool b = true );

    virtual QString getText() { return pLabelText->text(); }
    virtual bool isSelected() { return bSelected; }
      
signals:
    void signalClicked( WIconWidget * );

protected:
    bool bSelected;
    QLabel *pLabelIcon; 
    QLabel *pLabelText; 

    void mousePressEvent( QMouseEvent *pMouseEvent ) override;
};


#endif


