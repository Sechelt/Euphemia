/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
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

