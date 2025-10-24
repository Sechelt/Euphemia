#ifndef H_ORGPerson
#define H_ORGPerson

#include <DRectangle.h>

class ORGDiagram;

class ORGPerson : public DRectangle
{
    Q_OBJECT
public:
    ORGPerson( ORGDiagram *pDiagram, const QString &stringName = QString() );
    virtual ~ORGPerson();

    // SETTERS
    virtual void setSelectType();
    virtual void setPhoto( const QPixmap &pixmap );
    virtual void setTitle( const QString &s );
    virtual void setRole( const QString &s );
    virtual void setLocation( const QString &s );
    virtual void setEMail( const QString &s );
    virtual void setPhone( const QString &s );
    virtual void setProjectRole( const QString &s );

    // GETTERS
    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Person" ); }
    virtual ORGPerson *         getSuperior();
    virtual QList<ORGPerson*>   getSubordinates();
    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual AWPropWidget *      getPropWidget( QWidget *pWidgetParent = nullptr );
    virtual QPixmap             getPhoto()          { return pixmapPhoto;       }
    virtual QString             getTitle()          { return stringTitle;       }
    virtual QString             getRole()           { return stringRole;        }
    virtual QString             getLocation()       { return stringLocation;    }
    virtual QString             getEMail()          { return stringEMail;       }
    virtual QString             getPhone()          { return stringPhone;       }
    virtual QString             getProjectRole()    { return stringProjectRole; }

    // DO'RS
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElem );

    //
    virtual void mousePressHandle( DHandle *pHandle, const QPointF &pointScene );  
    virtual void mouseMoveHandle( DHandle *pHandle, const QPointF &pointScene );   
    virtual void mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene );
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );

public slots:
    virtual void slotDelete();

protected:
    // Name
    QPixmap pixmapPhoto;        // photo
    // Project
    QString stringProjectRole;
    QMap<QDateTime,QString> mapEvents;
    // Employment
    QString stringTitle;        // title 
    QString stringRole;         // primary role
    QString stringLocation;     // primary work location
    QString stringEMail;        // work email
    QString stringPhone;        // work phone

    virtual void doSinksCreate();
    virtual void doSinksSync();
    virtual void doSinksSyncRaw();

    virtual void doMenu( DHandle *pHandle, const QPointF &pointScenePos = QPointF() );
    virtual void doInitMenu();
    virtual void doFiniMenu();
    virtual void doSyncMenu();

    virtual void doInitResize();
};

#endif 

