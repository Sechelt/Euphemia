#ifndef H_LMRectangle
#define H_LMRectangle

#include <DRectangle.h>

#include "LMObject.h"

class LMModel;

/*!
 * \brief Base for all rectangle objects in a logical model.
 *  
 * \author pharvey (3/13/20)
 */
class LMRectangle : public DRectangle, public LMObject
{
    Q_OBJECT
public:
    LMRectangle( LMModel *pParent );
    virtual ~LMRectangle();

    virtual void setSelectType() {}
    virtual void setSelectType( SelectTypes ) {}
};


#endif


