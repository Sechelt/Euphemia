/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_PAToolFree
#define H_PAToolFree

#include "PATool.h"

/*!
 * \brief A shape which acts like a cursor and functions as a free hand pen/brush/stamp/spray.
 * 
 * \author pharvey (3/3/23)
 */
class PAToolFree : public PATool
{
public:
    PAToolFree( PAPaint *pLayer );
    virtual ~PAToolFree();

    virtual void doDoubleClick( PMouseEvent * ) {}
    virtual void doCommit() {}
    virtual void doCopy() {}

protected:
    virtual void doManipulateState() {}
    virtual void doCreateHandles() {}
    virtual void doSyncHandles() {}
};

#endif
