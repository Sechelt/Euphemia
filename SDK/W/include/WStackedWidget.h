/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#ifndef H_WStackedWidget
#define H_WStackedWidget

#include "W.h"

/*!
 * \class WStackedWidget 
 * \brief A stacked widget with a combobox for selecting which widget to show. 
 * 
 * \author pharvey (2/6/20)
 */
class WStackedWidget : public QWidget
{
    Q_OBJECT
public:
    WStackedWidget( QWidget *pParent );

    void setName( int nIndex, const QString &stringName );
    void addWidget( const QString &stringName, QWidget *pWidget );
    void removeWidget( const QString &stringName );
    void removeWidget( QWidget *pWidget );
    void setCurrent( const QString &stringName );
    void setCurrent( QWidget *pWidget );
    int count();
    QWidget *widget( int nIndex );

signals:
    void signalCurrentChanged( QWidget *pWidget );
    void signalCurrentChanged( const QString &stringName );

protected:
    QComboBox *     pComboBox;
    QStackedWidget *pStackedWidget;

    QVector<QWidget*> vectorWidgets;

protected slots:
    void slotIndex( int nIndex );
};

#endif 

