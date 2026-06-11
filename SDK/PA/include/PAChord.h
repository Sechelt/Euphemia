/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_PAChord
#define H_PAChord

#include "PAArc.h"

class PAChord : public PAArc
{
    Q_OBJECT
public:
    PAChord( PAPaint *pLayer );

protected:
    virtual void doPaint( QPainter *, bool bCommit = true );
};


#endif
