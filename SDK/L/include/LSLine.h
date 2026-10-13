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
 * Settings for line-ends.
 * 
 * \author pharvey (4/29/23)
 */
class LSLine : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(LSLine::LineEndStyles nBegin READ getBegin WRITE setBegin NOTIFY signalChanged)
    Q_PROPERTY(LSLine::LineEndStyles nEnd READ getEnd WRITE setEnd NOTIFY signalChanged)
    Q_PROPERTY(qreal nBeginSize READ getBeginSize WRITE setBeginSize NOTIFY signalChanged)
    Q_PROPERTY(qreal nEndSize READ getEndSize WRITE setEndSize NOTIFY signalChanged)
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
    void setBeginSize( qreal );
    void setEndSize( qreal );
    void setSelected( LineEndStyles, LineEndStyles, qreal, qreal );

    LineEndStyles   getBegin()      { return nBegin;        }
    LineEndStyles   getEnd()        { return nEnd;          }
    qreal           getBeginSize()  { return nBeginSize;    }
    qreal           getEndSize()    { return nEndSize;      }

    QWidget *   getButton( QWidget *pParent );
    QWidget *   getToolBar( QWidget *pParent );
    QWidget *   getPanel( QWidget *pParent );
    QWidget *   getSwatch( QWidget *pParent );

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem );
    void doLoad( QDomElement *pdomElem );

protected:
    LineEndStyles   nBegin      = LineEndNone;
    LineEndStyles   nEnd        = LineEndNone;
    qreal           nBeginSize  = 5;
    qreal           nEndSize    = 5;
};

/*!
 * \brief A button to invoke a dialog/panel to edit.
 * 
 * \author pharvey (2026-10-12)
 */
class LSLineButton : public QToolButton
{
    Q_OBJECT
public:
    LSLineButton( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    LSLine *pSetting;

    QPixmap getIcon();

protected slots:
    void slotClicked();
};

/*!
 * \brief Edit LSLine using a widget which can be used in a tool bar. 
 *  
 * Horizontal layout with subset of options. 
 * Includes 'More' button to invoke dialog to edit more details. 
 * 
 * \author pharvey (2026-10-12)
 */
class LSLineToolBar : public QWidget
{
    Q_OBJECT
public:
    LSLineToolBar( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    LSLine *            pSetting    = nullptr;
    QComboBox *         pBegin      = nullptr;
    QComboBox *         pEnd        = nullptr;
    QDoubleSpinBox *    pBeginSize  = nullptr;
    QDoubleSpinBox *    pEndSize    = nullptr;
    QToolButton *       pMore       = nullptr;

protected slots:
    void slotBegin( int );
    void slotEnd( int );
    void slotBeginSize( qreal n );
    void slotEndSize( qreal n );
    void slotMore();
};

/*!
 * \brief Panel for editing a LSLine.
 *  
 * Vertical layout. 
 * Includes all details. 
 *  
 * \author pharvey (2026-10-12)
 */
class LSLinePanel : public QWidget
{
    Q_OBJECT
public:
    LSLinePanel( QWidget *pParent = nullptr );

public slots:
    void slotRefresh();

protected:
    LSLine *            pSetting    = nullptr;
    QComboBox *         pBegin      = nullptr;
    QComboBox *         pEnd        = nullptr;
    QDoubleSpinBox *    pBeginSize  = nullptr;
    QDoubleSpinBox *    pEndSize    = nullptr;

protected slots:
    void slotBegin( int );
    void slotEnd( int );
    void slotBeginSize( qreal n );
    void slotEndSize( qreal n );
};

/*!
 * \brief Swatch (example) for a LSLine.
 * 
 * \author pharvey (2026-10-12)
 */
class LSLineSwatch : public QWidget
{
    Q_OBJECT
public:
    LSLineSwatch( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    LSLine *pSetting;

    void paintEvent( QPaintEvent *pEvent );
    QSize sizeHint() const;
};

#endif

