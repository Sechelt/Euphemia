/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_LSGeneral
#define H_LSGeneral

#include "SSettings.h"

/*!
 * \brief Preferences/Context settings for general use.
 * 
 * \author pharvey (3/15/23)
 */
class LSGeneral : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(bool bAutoCommit READ getAutoCommit WRITE setAutoCommit NOTIFY signalChanged)
    Q_PROPERTY(bool bPage READ getPage WRITE setPage NOTIFY signalChanged)
    Q_PROPERTY(bool bGrid READ getGrid WRITE setGrid NOTIFY signalChanged)
    Q_PROPERTY(bool bRuler READ getRuler WRITE setRuler NOTIFY signalChanged)
public:
    LSGeneral();

    void setAutoCommit( bool );
    void setPage( bool );
    void setGrid( bool );
    void setRuler( bool );
    void setSelected( bool, bool, bool, bool );

    bool    getAutoCommit()     { return bAutoCommit;       }
    bool    getPage()           { return bPage;             }
    bool    getGrid()           { return bGrid;             }
    bool    getRuler()          { return bRuler;            }

    QWidget *getButton( QWidget * /* pParent */ ) { return nullptr; }
    QWidget *getToolBar( QWidget *pParent ) override;
    QWidget *getPanel( QWidget *pParent ) override;

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) override;
    void doLoad( QDomElement *pdomElem ) override;

protected:
    bool    bAutoCommit         = true;             /*!< objects derived from PRasterTool can be manipulated before committing to image - here you can turn it on/off        */
    bool    bPage               = true;
    bool    bGrid               = true;             /*!< true to show grid                      */
    bool    bRuler              = true;             /*!< true to show ruler                     */
};

/*!
 * \brief   Widget for editing \sa LSGeneral.
 * 
 * \author pharvey (3/14/23)
 */
class LSGeneralWidget : public QWidget
{
    Q_OBJECT
public:
    LSGeneralWidget( QWidget *pParent, Qt::Orientation n );

public slots:
    void slotRefresh();

protected:
    LSGeneral *             pSetting;
    //
    QCheckBox *             pAutoCommit;      
    QCheckBox *             pPage;
    QCheckBox *             pGrid;
    QCheckBox *             pRuler;

protected slots:
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    void slotAutoCommit( Qt::CheckState );
    void slotPage( Qt::CheckState );
    void slotGrid( Qt::CheckState );
    void slotRuler( Qt::CheckState );
#else
    void slotAutoCommit( int );
    void slotPage( int );
    void slotGrid( int );
    void slotRuler( int );
#endif
};

#endif


