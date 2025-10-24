#include "LibInfo.h"
#include "DSpacer.h"

#include "DSpacerWidget.h"

DSpacer::DSpacer( ADObject *pObjectParent, const QString &stringName )
    : DRectangleBase( pObjectParent, stringName )
{
    pen.setColor( Qt::magenta );
    nSizeType = SizeExpanding;
}

DSpacer::~DSpacer()
{
}

void DSpacer::setSizeType( DSpacer::SizeTypes n )
{
    nSizeType = n;
    emit signalChangedSizeType( nSizeType );
}

AWPropWidget *DSpacer::getPropWidget( QWidget *pWidgetParent )
{
    AWPropWidget *pPropWidget = DRectangleBase::getPropWidget( pWidgetParent );

    pPropWidget->removeWidget( tr( "Layout" ) );

    pPropWidget->addWidget( tr("Layout"), new DSpacerWidget( this, pPropWidget ) );

    return pPropWidget;
}

QDomElement DSpacer::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = DRectangleBase::doSave( pdomDoc, pdomElemParent );

    domElemThis.setAttribute( "SizeType", (int)nSizeType );

    return domElemThis;
}

bool DSpacer::doLoad( QDomElement *pdomElemObject )
{
    DRectangleBase::doLoad( pdomElemObject );

    nSizeType = (DSpacer::SizeTypes)pdomElemObject->attribute( "SizeType", QString::number( (int)nSizeType ) ).toInt();

    return true;
}

void DSpacer::slotSizeType( DSpacer::SizeTypes n )
{
    if ( n == nSizeType ) return;
    setSizeType( n );
    setModified();
}

void DSpacer::doHandleMove( DHandle *pHandle, const QPointF &pointScenePos )
{
    switch ( getSelectType() )
    {
        case SelectResize:
            doResize( pHandle, pointScenePos );
            break;
        case SelectTransformRotate:
        case SelectTransformShear:
        case SelectMenu:
        case SelectOnly:
            break;
    }
}

void DSpacer::doHandlesCreate()
{
    Q_ASSERT( vectorHandles.count() == 0 );

    switch ( getSelectType() )
    {
        case SelectResize:
            doInitResize();
            break;
        case SelectTransformRotate:
        case SelectTransformShear:
        case SelectMenu:
            break;
        case SelectOnly:
            doInitSelect();
            break;
    }
}

void DSpacer::doHandlesDelete()
{
    switch ( getSelectType() )
    {
        case SelectResize:
            doFiniResize();
            break;
        case SelectTransformRotate:
        case SelectTransformShear:
        case SelectMenu:
            break;
        case SelectOnly:
            doFiniSelect();
            break;
    }
}

void DSpacer::doHandlesSync()
{
    Q_ASSERT( vectorHandles.count() > 0 );

    switch ( getSelectType() )
    {
        case SelectResize:
            doSyncResize();
            break;
        case SelectTransformRotate:
        case SelectTransformShear:
        case SelectMenu:
            break;
        case SelectOnly:
            doSyncSelect();
            break;
    }
}



