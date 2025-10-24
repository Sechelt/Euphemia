/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */
#ifndef H_PASGradient
#define H_PASGradient

#include "SSettings.h"

#include "PAGradientFill.h"

/*!
 * \brief Preferences/Context settings for PASGradient.
 * 
 * \author pharvey (3/15/23)
 */
class PASGradient : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(int nType READ getType WRITE setType NOTIFY signalChanged)
    Q_PROPERTY(QGradient::Spread nSpread READ getSpread WRITE setSpread NOTIFY signalChanged)
public:
    PASGradient();

    void setType( int );
    void setSpread( QGradient::Spread );
    void setSelected( int, QGradient::Spread );

    int                 getType()   { return nType;     }
    QGradient::Spread   getSpread() { return nSpread;   }

    QWidget *           getButton( QWidget * /* pParent */ ) { return nullptr; }
    QWidget *           getToolBar( QWidget *pParent ) override;
    QWidget *           getPanel( QWidget *pParent ) override;

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) override;
    void doLoad( QDomElement *pdomElem ) override;

protected:
    int                 nType   = PAGradientFill::StandardGradientLinear;                  /*!< value will be either a QGradient::Preset or PASGradient::StandardGradient               */
    QGradient::Spread   nSpread = QGradient::PadSpread;

};

/*!
 * \brief   Edit widget for use in a tool bar. 
 *          Edits PASGradient.
 *  
 * Horizontal layout.
 * 
 * \author pharvey (3/14/23)
 */
class PASGradientToolBar : public QWidget
{
    Q_OBJECT
public:
    PASGradientToolBar( QWidget *p );

public slots:
    void slotRefresh();

protected slots:
    void slotType( int );
    void slotSpread( int );

protected:
    PASGradient *pSetting;
    QComboBox *pType;
    QComboBox *pSpread;

private:
    QPixmap getSwatch( const QSize size, int nType );

    void doAddType( const QString &stringText, int nType );
};

/*!
 * \brief   Edit widget for use in a panel.
 *          Edits PASGradient.
 *  
 * Vertical layout.
 * 
 * \author pharvey (3/14/23)
 */
class PASGradientPanel : public QWidget
{
    Q_OBJECT
public:
    PASGradientPanel( QWidget *p );

public slots:
    void slotRefresh();

protected slots:
    void slotType( int );
    void slotSpread( int );

protected:
    PASGradient *pSetting;
    QComboBox *pType;
    QComboBox *pSpread;

private:
    QPixmap getSwatch( const QSize size, int nType );

    void doAddType( const QString &stringText, int nType );
};

#endif


