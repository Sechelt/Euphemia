/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_LSLine
#define H_LSLine

#include <SSetting.h>

/*!
 * \brief Setting for \sa PLine
 * 
 * \author pharvey (4/29/23)
 */
class LSLine : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(LSLine::LineEndStyles nBegin READ getBegin WRITE setBegin NOTIFY signalChanged)
    Q_PROPERTY(LSLine::LineEndStyles nEnd READ getEnd WRITE setEnd NOTIFY signalChanged)
public:
    enum LineEndStyles
    {
        LineEndNone,
        LineEndArrow,
        LineEndBall
    };
    Q_ENUM(LineEndStyles)

    LSLine();

    void setBegin( LineEndStyles );
    void setEnd( LineEndStyles );
    void setSelected( LineEndStyles, LineEndStyles );

    LineEndStyles    getBegin()   { return nBegin;   }
    LineEndStyles    getEnd()     { return nEnd;     }

    QWidget *   getButton( QWidget * /* pParent */ ) { return nullptr; }
    QWidget *   getToolBar( QWidget *pParent );
    QWidget *   getPanel( QWidget *pParent );

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem );
    void doLoad( QDomElement *pdomElem );

protected:
    LineEndStyles nBegin    = LineEndNone;
    LineEndStyles nEnd      = LineEndNone;
};

/*!
 * \brief Widget to edit \sa LSLine.
 * 
 * \author pharvey (4/29/23)
 */
class LSLineWidget : public QWidget
{
    Q_OBJECT
public:
    LSLineWidget( QWidget *p, Qt::Orientation n );

public slots:
    void slotRefresh();

protected slots:
    void slotBegin( int );
    void slotEnd( int );

protected:
    LSLine *       pSetting;

    QComboBox *    pBegin;
    QComboBox *    pEnd;
};

#endif

