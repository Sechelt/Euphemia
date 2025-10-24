#ifndef H_LMAssociativeLink
#define H_LMAssociativeLink

//
#include "LMEntityBase.h"
#include "LMEntityNormal.h"
#include "LMEntityAssociative.h"
#include "LMRelationship.h"

class LMModel;

/*!

*/
class LMAssociativeLink : public LMRelationship
{
    Q_OBJECT
public:
    LMAssociativeLink( LMModel *pmodelParent );
    virtual ~LMAssociativeLink();

    // SETTERS

    // GETTERS
    virtual QPixmap                 getIcon();
    virtual QString                 getFriendlyClassName() { return tr( "Associative Link" ); }
    virtual AWDataWidget *          getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual AWPropWidget *          getPropWidget( QWidget *pWidgetParent = nullptr );
    virtual LMEntityNormal *        getEntityNormal();
    virtual LMEntityAssociative *   getEntityAssociative();

    // DO'rs
    virtual void doLoadDefaults();
};

#endif


