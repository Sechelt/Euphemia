#ifndef H_LMRelationship
#define H_LMRelationship

#include "LMLine.h"
#include "LMEntityBase.h"

class LMModel;

class LMRelationship : public LMLine
{
    Q_OBJECT
public:
    /*!
     * \brief Flag to indicate line-end features to be drawn. 
     * \author pharvey (1/2/20)
     */
    enum ComposedFlag
    {
        ComposedNone=0,
        ComposedCrowsFoot=1,      // draw crows feet (else nothing)
        ComposedTriangle=2,       // draw triangle on line (else nothing)
        ComposedPerpendicular=4   // draw | (else draws O)
    };
    Q_DECLARE_FLAGS(ComposedFlags, ComposedFlag);

    explicit LMRelationship( LMModel *pmodelParent );
    virtual ~LMRelationship();

    // SETTERS
    virtual void setVisible( bool b = true );
    virtual void setGenerate( bool b = true );
    virtual void setDominant( CBD::EOLTypes n, bool b = true );
    virtual void setDependent( CBD::EOLTypes n, bool b = true );
    virtual void setIdentified( CBD::EOLTypes n, bool b = true );
    virtual void setCardinality( CBD::EOLTypes n, const QString &s );
    virtual void setRole( CBD::EOLTypes n, const QString &s );

    // GETTERS
    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Relationship" ); }
    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual AWPropWidget *      getPropWidget( QWidget *pWidgetParent = nullptr );
    virtual QString             getParentEntity();
    virtual QString             getChildEntity();
    virtual LMEntityBase *      getParentEntityObject();
    virtual LMEntityBase *      getChildEntityObject();
    virtual CBD::EOLTypes       getDominant()               { return nDominant; }
    virtual bool                isGenerate()                { return bGenerate; }
    virtual bool                isDominant( CBD::EOLTypes n );
    virtual bool                isDependent( CBD::EOLTypes n );
    virtual bool                isIdentified( CBD::EOLTypes n );
    virtual QString             getCardinality( CBD::EOLTypes n );
    virtual QString             getRole( CBD::EOLTypes n );
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses );

    // DO'rs
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElemThis );
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );
    virtual void doLoadDefaults();
    virtual void doPostLoad();

public slots:
    virtual void slotDescription( const QString &s )    { setDescription( s );  }
    virtual void slotAnnotation( const QString &s )     { setAnnotation( s );   }    

protected:
    bool            bGenerate;              // generate in PM
    CBD::EOLTypes   nDominant;              // (Begin|End|Neither) Dominant Entity will be parent Table in PM; only viable when 1-1
    QString         stringBeginCardinality; // format = "min,max"
    QString         stringEndCardinality;   // format = "min,max"
    QString         stringBeginRole;
    QString         stringEndRole;

    // floating text for line ends show Role + Cardinality
    QGraphicsTextItem * pFloatingTextBegin;
    QGraphicsTextItem * pFloatingTextEnd;
    QPointF             pointDeltaFloatingTextBegin;
    QPointF             pointDeltaFloatingTextEnd;
    ComposedFlags       ComposedFlagBegin;
    ComposedFlags       ComposedFlagEnd;

    virtual void setFloatingText( CBD::EOLTypes n );

    virtual QPointF getFloatingTextBeginPos();  // scene coord
    virtual QPointF getFloatingTextEndPos();    // scene coord

    virtual void doHandleMove( DHandle *pHandle, const QPointF &pointScenePos );
    virtual void doHandlesCreate();
    virtual void doHandlesSync();

    virtual void doSourcesCreate();
    virtual void doSourcesDelete();
    virtual void doSourcesSync();

    virtual void doDrawBegin( QPainter *pPainter, const QLineF &lineSegment, const QLineF &lineIntersected );
    virtual void doDrawEnd( QPainter *pPainter, const QLineF &lineSegment, const QLineF &lineIntersected );

    virtual void doDrawComposedArrow( QPainter *pPainter, float x, float y, float vecX, float vecY, float borderX1, float borderY1, float borderX2, float borderY2, float scale, ComposedFlags Flag, bool withLine = true );
    virtual void doRotate( float x, float y, float * _x, float * _y, float angle );
};

Q_DECLARE_OPERATORS_FOR_FLAGS(LMRelationship::ComposedFlags);

#endif


