/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_W
#define H_W

#include <QtGui>
#include <QtWidgets>

namespace W 
{

    enum WLineEndStyles
    {
        // Common LineEnd styles.
        WLineEndNone,
        WLineEndArrow,
        WLineEndBall,
        // This indicates that some other factors will be used to determine
        // the style of the LineEnd. For example; an ERD may
        WLineEndComposed        
    };

    enum WImagePlacements
    {
        WImagePlacementNormal,
        WImagePlacementTiled,
        WImagePlacementScaled
    };

/*! 
 * \brief Set combox item current based upon the user data. 
 *  
 * The user data must be an int. This works will for Qt enum values 
 * such as Qt::PenStyle etc. 
 *  
 * \note Could use QComboBox::findData() 
 *  
 */
#define WSetComboBox( pComboBox, nUserData )                        \
    for ( int n = 0; n < pComboBox->count(); n++ )                  \
    {                                                               \
        if ( pComboBox->itemData( n ).toInt() == nUserData )        \
        {                                                           \
            pComboBox->setCurrentIndex( n );                        \
            break;                                                  \
        }                                                           \
    }                                                           

}; // namespace


#endif



