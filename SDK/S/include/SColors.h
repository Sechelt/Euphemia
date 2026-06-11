/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_SColors
#define H_SColors

#include "S.h"

/*!
 * \brief A customizable colour palette.
 *  
 * Is a color palette based upon a vector of QColor. 
 *  
 * Has a default palette which can be; 
 *      - edited
 *      - saved
 *      - loaded
 *      - reset
 *  
 * No concept of 'current' color or 'selected' color. 
 *  
 * There should be just one of these and it should be global. The global should be 
 * initialized/finilized explicitly in main.cpp after Qt application and before exec 
 * main widget. 
 *  
 * \sa g_SColors 
 *  
 * \author pharvey (11/30/22)
 */
class SColors : public QObject
{
    Q_OBJECT
public:
    SColors();
    ~SColors();

    void setValue( int nIndex, const QColor &color ); 

    QString                 getFileName()   { return stringFileName; }
    QVector<QColor>         getColors()     { return vectorColors;  }
    static QVector<QColor>  getDefaultColors();

    void doInit();
    void doFini();

    bool isModified() { return bModifiedPalette; }

signals:
    void signalModifiedFileName();          // file name modified
    void signalModifiedPalette();           // palette modified

public slots:
    void slotLoad();
    void slotSave();
    void slotSaveAs();
    void slotReset();

protected:
    QString         stringFileName;
    bool            bModifiedPalette = false;   // is palette modified? (we do not care about current or file name)
    QVector<QColor> vectorColors;               /*!< customizable list of colors                            */

    void setFileName( const QString &s );
    void setModifiedPalette( bool b );

    bool doLoad( const QString &stringFileName );
    bool doSave( const QString &stringFileName );
};

/*!
 * \brief Global for color palette.
 *  
 * App main.cpp would be a good place to init/fini this. 
 *  
 * \author pharvey (4/12/23)
 */
extern SColors *g_SColors;

#endif

