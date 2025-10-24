#include "LibInfo.h"
#include "PMViewColumn.h"

#include "PMModel.h"
#include "PMView.h"

/*!
 * \brief A view column. 
 *  
 * A view column name is of the form; TABLE.COLUMN 
 * A view can have 0-n of these and they are created/deleted during the parse 
 * of the SQL in PMView. 
 *  
 * The primary purpose of these, at present, is for drawing the view in a diagram. 
 * If the parse fails and the SQL is viable - the DDL should still be valid. 
 *  
 * \author pharvey (5/24/20)
 * 
 * \param pParent 
 */
PMViewColumn::PMViewColumn( PMView *pParent )
    : AWObject( pParent ), PMObject( this )
{ 
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif


#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

PMViewColumn::~PMViewColumn()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

QPixmap PMViewColumn::getIcon()
{
    return QPixmap( ":PM/Column" );
}

/*!
 * \brief Get widget for editing view column in a dialog. 
 *  
 * We intervene on the default here because there is nothing to edit in this context. 
 * 
 * \author pharvey (5/24/20)
 * 
 * \param pWidgetParent 
 * 
 * \return AWDataWidget* 
 */
AWDataWidget *PMViewColumn::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = new AWDataWidget( this, pWidgetParent );
    return p;
}

/*!
 * \brief Get widget for editing view column in a dock.
 * 
 * We intervene on the default here because there is nothing to edit in this context. 
 * 
 * \author pharvey (5/24/20)
 * 
 * \param pWidgetParent 
 * 
 * \return AWPropWidget* 
 */
AWPropWidget *PMViewColumn::getPropWidget( QWidget *pWidgetParent )
{
    AWPropWidget *pPropWidget = new AWPropWidget( this, pWidgetParent );
    return pPropWidget;
}

ADValidationDatabase PMViewColumn::getValidationConfiguration( ADValidationDatabase &mapClasses )
{
    return mapClasses;
}

QDomElement PMViewColumn::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = AWObject::doSave( pdomDoc, pdomElemParent );
    doSavePMObject( pdomDoc, &domElemThis );

    return domElemThis;
}

bool PMViewColumn::doLoad( QDomElement *pdomElemThis )
{
    AWObject::doLoad( pdomElemThis );
    doLoadPMObject( pdomElemThis );

    return true;
}

void PMViewColumn::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    Q_UNUSED(nStrictness);
    Q_UNUSED(nValueType);
    Q_UNUSED(listObjectPathItems);
}

