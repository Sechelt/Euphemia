#ifndef H_LMLine
#define H_LMLine

#include <DLine90Auto.h>

#include "LMObject.h"

class LMModel;

/*!
 * \brief Base for all line objects in a logical model.
 *  
 * This is where we theme the appearance of all lines. 
 *  
 * \author pharvey (3/13/20)
 */
class LMLine : public DLine90Auto, public LMObject
{
    Q_OBJECT
public:
    LMLine( LMModel *pParent );
    virtual ~LMLine();
};


#endif


