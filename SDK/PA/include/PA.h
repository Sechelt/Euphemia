/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */
#ifndef P_H
#define P_H

#include <QtGui>
#include <QtWidgets>
#include <QPrinter>
#include <QPrintDialog>
#include <QtMath>

namespace P 
{

};

class PMouseEvent
{
public:
    PMouseEvent( QGraphicsSceneMouseEvent *p )
    {
        Q_ASSERT( p );
        nButton = p->button();
        nButtons = p->buttons();
        nModifiers = p->modifiers();
        pointPos = p->scenePos().toPoint();
    }

    Qt::MouseButton button() const              { return nButton;       }
    Qt::MouseButtons buttons() const            { return nButtons;      }
    Qt::KeyboardModifiers modifiers() const     { return nModifiers;    }
    QPoint pos() const                          { return pointPos;      }

// not needed - just use constructor
/*
    inline T& operator=(const T& other)    
    {                                      
        if ( this == &other ) return *this;
        nButton = other.button();          
        nButtons = other.buttons();        
        nModifiers = other.modifiers();    
        pointPos = other.pos();  
        return *this;                      
    }                                      
*/

protected:
    Qt::MouseButton         nButton;
    Qt::MouseButtons        nButtons; 
    Qt::KeyboardModifiers   nModifiers;
    QPoint                  pointPos;
};

#endif



