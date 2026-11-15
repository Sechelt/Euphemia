#ifndef H_PMLine
#define H_PMLine

#include <DLine90Auto.h>

#include "PMObject.h"

class PMModel;

class PMLine : public DLine90Auto, public PMObject
{
    Q_OBJECT
public:
    PMLine( PMModel *pParent );
    virtual ~PMLine();

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool        doLoad( QDomElement *pdomElemTable );
};

#endif


