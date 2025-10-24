#ifndef H_DFile
#define H_DFile

#include <QPrinter>

#include <AWFile.h>

class DDiagram;
class DStencilsWidget;

/*!
 * \class DFile 
 * \brief File (root class).
 *  
 * This extends AWFile to provide support for 'Stencils'. 
 * Stencils are palettes. This means that they provide a means to drag to the canvas to create a new object. 
 * Another type of palette is the 'Classes' palette. 
 *  
 * Stencils - provided by the workspace, produces a DImage/DVector object with a specific image 
 * Classes - provided by a diagram, produces an object of the given class 
 *  
 * \author pharvey (2/7/20)
 */
class DFile : public AWFile
{
    Q_OBJECT
public:
    explicit DFile( const QString &stringName = QString() );
    virtual ~DFile();

    virtual void setStrictness( ADValidation::Strictness nStrictness );

    virtual ADValidation::Strictness    getStrictness() { return nStrictness; }
    virtual ADObject *                  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
    virtual DStencilsWidget *           getStencilsWidget( QWidget *p );
    virtual QMenu *                     getContextMenu( QWidget * );

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElem );
    virtual void doClear();

public slots:
    virtual void slotValidateFile();

protected:
    DStencilsWidget *pStencilsWidget;
    ADValidation::Strictness nStrictness;

protected slots:
    virtual void slotNewDiagram();
    virtual void slotNewStencils();
};

#endif


