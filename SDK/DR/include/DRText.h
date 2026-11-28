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
 * \brief Text with the rectangle not displayed.
 * 
 * \author pharvey (2026-11-23)
 */
class DRText : public DRRectangleBase
{
    Q_OBJECT
public:
    DRText( DRDraw *pCanvas, const QPointF &pointPos = QPointF() );

    virtual QPixmap     getIcon() override;

protected:
    virtual void doDraw( QPainter * ) override;
    virtual void doConnectSettings() override;
    virtual void doDisconnectSettings() override;
};

/*!
 * \brief DRText settings.
 * 
 * This provides settings for DRText.
 * We do not display a rect so we want to ignore pen and brush in our settings.
 * 
 * \author pharvey (2026-11-23)
 */
class DRSText : public SSetting 
{
    Q_OBJECT
public:
    DRSText();

    SText *         pText;
    STextContent *  pTextContent;

    QWidget *   getButton( QWidget *pParent ) override;
    QWidget *   getToolBar( QWidget *pParent ) override;
    QWidget *   getPanel( QWidget *pParent ) override;
    QWidget *   getPanelMax( QWidget *pParent ) override;

    virtual void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) override;
    virtual void doLoad( QDomElement *pdomElem ) override;                       
};

/*!
 * \brief Tool bar widget for DRSText.
 * 
 * We have this because we need to invoke a dialog for DRSText not STextContent.
 * 
 * \author pharvey (2026-11-27)
 */
class DRSTextToolBar : public STextContentToolBar
{
    Q_OBJECT
public:
    DRSTextToolBar( QWidget *pParent );

protected slots:
    virtual void slotMore() override;
};

#endif
