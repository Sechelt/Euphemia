#ifndef H_DLine90V
#define H_DLine90V

#include "DLine90.h"

/*!
 * \brief A line which maintains a single 90.
 *  
 * This implementation results in the first line segment always being vertical 
 * while the second line segment is always horizontal. 
 *  
 * \author pharvey (2/27/20)
 */
class DLine90V : public DLine90
{
    Q_OBJECT
public:
    explicit DLine90V( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DLine90V();

    virtual QPixmap getIcon();
};

#endif


