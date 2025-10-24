#ifndef H_PMRectangle
#define H_PMRectangle

#include <DRectangle.h>

#include "PMObject.h"

class PMModel;

/*!
 * \brief Base for all rectangle objects in a physical model.
 *  
 * \author pharvey (3/13/20)
 */
class PMRectangle : public DRectangle, public PMObject
{
    Q_OBJECT
public:
    PMRectangle( PMModel *pParent );
    virtual ~PMRectangle();

    virtual void setSelectType() {}                 // get rid of DRectangle behaviour
    virtual void setSelectType( SelectTypes ) {}    // get rid of DRectangle behaviour

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool        doLoad( QDomElement *pdomElemTable );
};


#endif


