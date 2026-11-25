/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_DRText
#define H_DRText

#include <LSText.h>

#include "DRRectangleBase.h"

class DRSText;

/*!
 * \brief Text, which by default, lacks displaying rect.
 * 
 * The text does not allow for paragraphs - just a single line.
 * 
 * \author pharvey (2026-11-23)
 */
class DRText : public DRRectangleBase
{
    Q_OBJECT
public:
    DRText( DRDraw *pCanvas, const QPointF &pointPos = QPointF() );

    virtual QPixmap     getIcon() override;

    // save/load
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent ) override;
    virtual bool        doLoad( QDomElement *pdomElem ) override;

protected:
    DRSText *pSettingText;

    QString stringText;

    virtual void doDraw( QPainter * ) override;
    virtual void doConnectSettings() override;
    virtual void doDisconnectSettings() override;

protected slots:
    void slotRefreshText();
};

/*!
 * \brief DRText settings.
 * 
 * This provides settings for DRText. This means the text itself.
 * Most of the settings come from DRSRectangleBase.
 * 
 * \author pharvey (2026-11-23)
 */
class DRSText : public DRSRectangleBase 
{
    Q_OBJECT
    Q_PROPERTY(QString stringText READ getText WRITE setText NOTIFY signalChanged)
public:
    DRSText();

    void setText( const QString &stringText );
    void setSelected( const QString &stringText );

    QString getText() { return stringText; }

    QWidget *   getButton( QWidget *pParent ) override;
    QWidget *   getToolBar( QWidget *pParent ) override;
    QWidget *   getPanel( QWidget *pParent ) override;
    QWidget *   getSwatch( QWidget *pParent ) override;
    QToolBox *  getToolBox( QWidget *pParent ) override;

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) override;
    void doLoad( QDomElement *pdomElem ) override;

protected:
    QString stringText;
};

class DRSTextToolBar : public QWidget
{
    Q_OBJECT
public:
    DRSTextToolBar( QWidget *p );

public slots:
    void slotRefresh();

protected slots:
    void slotText( const QString & );
    void slotMore();

protected:
    DRSText *               pSetting;

    QLineEdit *             pLineEdit = nullptr;
    QToolButton *           pMore;
};

class DRSTextPanel : public QWidget
{
    Q_OBJECT
public:
    DRSTextPanel( QWidget *p );

public slots:
    void slotRefresh();

protected slots:
    void slotText( const QString & );

protected:
    DRSText *               pSetting;

    QLineEdit *             pLineEdit = nullptr;
};

#endif
