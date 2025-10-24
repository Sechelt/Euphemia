#ifndef H_LMAttributesUsedWidget
#define H_LMAttributesUsedWidget

#include <AWDataTable.h>

#include "LMIdentifier.h"

class LMEntityBase;

/*!
 * \class LMAttributesUsedWidget 
 * \brief Add/remove references to attributes. 
 *  
 * Identifiers can reference 0-n attributes. 
 * A useful Identifier references at least one attribute. 
 * 
 * \author pharvey (12/29/19)
 */
class LMAttributesUsedWidget : public AWDataTable
{
    Q_OBJECT
public:
    LMAttributesUsedWidget( LMIdentifier *pIdentifier, QWidget *pParent );
    virtual ~LMAttributesUsedWidget();

public slots:
    virtual void slotNew();

protected:
    LMEntityBase *  pEntity;
    LMIdentifier *  pIdentifier;

    virtual void doRowGet( int nRow, AWObject *pmodel );
    virtual void doRowSet( int nRow, AWObject *pmodel );
};

#endif 


