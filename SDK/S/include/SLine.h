/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_SLine
#define H_SLine

#include <SSettings.h>

/*!
 * \class SLine
 * 
 * \author pharvey (2026-10-12)
 */
class SLine : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(qreal nSize READ getValue WRITE setValue NOTIFY signalChanged)
public:
    SLine();

    void setValue( qreal );
    void setSelected( qreal );

    qreal       getValue() { return nSize; }
    QWidget *   getButton( QWidget *pParent ) override;
    QWidget *   getToolBar( QWidget *pParent ) override;
    QWidget *   getPanel( QWidget *pParent ) override;
    QWidget *   getSwatch( QWidget *pParent ) override;

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) override;
    void doLoad( QDomElement *pdomElem ) override;

protected:
    qreal nSize;
};

/*!
 * \brief A button to invoke a dialog/panel to edit.
 * 
 * \author pharvey (2026-10-12)
 */
class SLineButton : public QToolButton
{
    Q_OBJECT
public:
    SLineButton( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    SLine *pLine;

    QPixmap getIcon();

protected slots:
    void slotClicked();
};

/*!
 * \brief Edit SLine using a widget which can be used in a tool bar. 
 *  
 * Horizontal layout with subset of options. 
 * Includes 'More' button to invoke dialog to edit more details. 
 * 
 * \author pharvey (2026-10-12)
 */
class SLineToolBar : public QWidget
{
    Q_OBJECT
public:
    SLineToolBar( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    SLine *            pLine;
    qreal               nSize;
    QDoubleSpinBox *    pSize   = nullptr;
    QToolButton *       pMore   = nullptr;

protected slots:
    void slotSize( qreal n );
    void slotMore();
};

/*!
 * \brief Panel for editing a SLine.
 *  
 * Vertical layout. 
 * Includes all details. 
 *  
 * \author pharvey (2026-10-12)
 */
class SLinePanel : public QWidget
{
    Q_OBJECT
public:
    SLinePanel( QWidget *pParent = nullptr );

public slots:
    void slotRefresh();

protected:
    SLine *pLine;
    qreal   nSize;
    QDoubleSpinBox *          pSize;

protected slots:
    void slotSize( qreal n );
};

/*!
 * \brief Swatch (example) for a SLine.
 * 
 * \author pharvey (2026-10-12)
 */
class SLineSwatch : public QWidget
{
    Q_OBJECT
public:
    SLineSwatch( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    SLine *pLine;
    qreal   nSize;

    void paintEvent( QPaintEvent *pEvent );
    QSize sizeHint() const;
};

#endif


