/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_SRuler
#define H_SRuler

#include <SSettings.h>

/*!
 * \brief Preference/Context for ruler.
 *  
 * There will be more features in here in the future. 
 *  
 * \author pharvey (4/20/23)
 */
class SRuler : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(int nPixelMajour READ getPixelMajour WRITE setPixelMajour NOTIFY signalChanged)
    Q_PROPERTY(int nPixelMinor READ getPixelMinor WRITE setPixelMinor NOTIFY signalChanged)
public:
    SRuler( QObject *pParent = nullptr );

    void setPixelMajour( int );
    void setPixelMinor( int );
    void setSelected( int, int );

    int getPixelMajour()    { return nPixelMajour;  }
    int getPixelMinor()     { return nPixelMinor;   }

    QWidget *getButton( QWidget * ) override { return nullptr; }
    QWidget *getToolBar( QWidget *pParent ) override;
    QWidget *getPanel( QWidget *pParent ) override;

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) override;
    void doLoad( QDomElement *pdomElem ) override;

protected:
    int     nPixelMajour = 10;
    int     nPixelMinor  = 5;
};

/*!
 * \brief Widget to edit SRuler.
 *  
 * \author pharvey (3/14/23)
 */
class SRulerWidget : public QWidget
{
    Q_OBJECT
public:
    SRulerWidget( SRuler *pRuler, QWidget *pParent, Qt::Orientation n );

protected:
    SRuler *    pRuler = nullptr;
    QSpinBox *  pMajour;
    QSpinBox *  pMinor;

protected slots:
    void slotRefresh();
    void slotMajour( int n );
    void slotMinor( int n );
};

#endif


