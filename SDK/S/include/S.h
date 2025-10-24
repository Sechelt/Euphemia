#ifndef H_S
#define H_S

#include <CBD.h>

class S : public QObject
{
    Q_OBJECT
public:
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

