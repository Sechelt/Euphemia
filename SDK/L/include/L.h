/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_L
#define H_L

#include <CBD.h>

class L : public QObject
{
    Q_OBJECT
public:
    enum GridTypes
    {
        GridTypeNone,       /*!< do not show/use a grid             */
        GridTypeStandard,   /*!< cell size user specified           */
        GridTypePixel       /*!< cell size auto to represents pixel */
    };
    Q_ENUM( GridTypes );

    enum GridSnaps
    {
        GridSnapNone,       /*!< no snap                                */
        GridSnapLine,       /*!< mouse snaps to nearest grid line       */
        GridSnapHLine,      /*!< mouse snaps to nearest H grid line     */
        GridSnapVLine,      /*!< mouse snaps to nearest V grid line     */
        GridSnapCenter      /*!< mouse snaps to nearest cell center     */
    };
    Q_ENUM( GridSnaps );
};

#endif

