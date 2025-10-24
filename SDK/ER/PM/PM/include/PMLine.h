#ifndef H_PMLine
#define H_PMLine

#include <DLinePoly.h>

#include "PMObject.h"

class PMModel;

class PMLine : public DLinePoly, public PMObject
{
    Q_OBJECT
public:
    PMLine( PMModel *pParent );
    virtual ~PMLine();

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool        doLoad( QDomElement *pdomElemTable );
};

#endif


