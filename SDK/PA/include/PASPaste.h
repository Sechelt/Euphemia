/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_PASPaste
#define H_PASPaste

#include "SSetting.h"

/*!
 * \brief Preferences/Context settings for PASPaste.
 * 
 * \author pharvey (3/15/23)
 */
class PASPaste : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(QPainter::CompositionMode nCompositionMode READ getCompositionMode WRITE setCompositionMode NOTIFY signalChanged)
    Q_PROPERTY(bool bStamp READ getStamp WRITE setStamp NOTIFY signalChanged)
public:
    PASPaste();

    void setCompositionMode( QPainter::CompositionMode );
    void setStamp( bool );
    void setSelected( QPainter::CompositionMode, bool );

    QPainter::CompositionMode   getCompositionMode()    { return nCompositionMode;  }
    bool                        getStamp()              { return bStamp;            }

    QWidget *getButton( QWidget * /* pParent */ ) { return nullptr; }
    QWidget *getToolBar( QWidget *pParent );
    QWidget *getPanel( QWidget *pParent );

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem );
    void doLoad( QDomElement *pdomElem );

protected:
    QPainter::CompositionMode nCompositionMode = QPainter::CompositionMode_SourceOver;
    bool bStamp = false;
};

/*!
 * \brief   Edit widget for use in a tool bar. 
 *          Edits PASPaste.
 *  
 * Horizontal layout.
 * 
 * \author pharvey (3/14/23)
 */
class PASPasteToolBar : public QWidget
{
    Q_OBJECT
public:
    PASPasteToolBar( QWidget *p );

public slots:
    void slotRefresh();

protected:
    PASPaste *    pSetting;
    QComboBox * pComposition;
    QCheckBox * pStamp;

protected slots:
    void slotComposition( int );
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    void slotStamp( Qt::CheckState );
#else
    void slotStamp( int );
#endif
};

/*!
 * \brief   Edit panel for use in a dialog or dock. 
 *          Edits PASPaste.
 *  
 * Vertical layout.
 * 
 * \author pharvey (3/14/23)
 */
class PASPastePanel : public QWidget
{
    Q_OBJECT
public:
    PASPastePanel( QWidget *p );

public slots:
    void slotRefresh();

protected:
    PASPaste *    pSetting;
    QComboBox * pComposition;
    QCheckBox * pStamp;

protected slots:
    void slotComposition( int );
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    void slotStamp( Qt::CheckState );
#else
    void slotStamp( int );
#endif
};

#endif


