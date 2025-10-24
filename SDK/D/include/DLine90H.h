#ifndef H_DLine90H
#define H_DLine90H

#include "DLine90.h"

/*!
 * \brief A line which maintains a single 90.
 *  
 * This implementation results in the first line segment always being horizontal 
 * while the second line segment is always vertical. 
 *  
 * \author pharvey (2/27/20)
 */
class DLine90H : public DLine90
{
    Q_OBJECT
public:
    explicit DLine90H( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DLine90H();

    virtual QPixmap getIcon();
};

#endif


