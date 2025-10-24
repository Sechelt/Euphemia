#ifndef H_DStencilsWidget
#define H_DStencilsWidget

#include <WStackedWidget.h>
#include <WIconLayout.h>
#include <WIconWidget.h>

#include <ADObject.h>

class DStencils;
class DStencil;
class DStencilItem;

/*!
 * \brief A DStencilItem widget.
 *  
 * We extend \sa WIconWidget to include a pointer to the \sa DStencilItem. In this way we can 
 * package for a drag. 
 * 
 * \author pharvey (5/7/20)
 */
class DStencilItemWidget : public WIconWidget
{
    Q_OBJECT
public:
    explicit DStencilItemWidget( QWidget *pWidget, DStencilItem *pStencilItem );
    virtual ~DStencilItemWidget();

    virtual DStencilItem *getStencilItem() { return pStencilItem; }

public slots:
    virtual void slotModified();

protected:
    DStencilItem *pStencilItem;

    void mousePressEvent( QMouseEvent *pMouseEvent ) override;
};

/*!
 * \brief A DStencil widget. 
 *  
 * This contains 0-n DStencilItemWidget's in a WIconLayout.
 *  
 * This populates itself in the constructor and is kept up-to-date via; 
 * \sa slotChildCreated 
 * \sa slotChildDeleted 
 *  
 * \author pharvey (5/7/20)
 */
class DStencilWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DStencilWidget( QWidget *pWidget, DStencil *pStencil );
    virtual ~DStencilWidget();

    virtual DStencil *getStencil() { return pStencil; }

public slots:
    virtual void slotChildCreated( ADObject *p );
    virtual void slotChildDeleted( ADObject *p );
 
protected:
    QList<DStencilItemWidget*>  listStencilItemWidgets;
    DStencil *                  pStencil;
    WIconLayout *               pLayout;

    virtual void doLoad( DStencil *pStencil );
    virtual void doClear();
};

/*!
 * \brief A DStencils widget. 
 *  
 * This is a stack of DStencilWidget's. 
 *  
 * The underlying WStackedWidget provides a combobox for choosing which DStencilWidget to show. 
 *  
 * A DWorkspace can have 0-1 DStencils but we do not want to remove this widget so this must be 
 * prepared to function with pStencils == nullptr. 
 * 
 * This populates itself in the constructor and is kept up-to-date via ;
 * \sa slotCreated 
 * \sa slotDeleted 
 * \sa slotChildCreated 
 * \sa slotChildDeleted 
 *  
 * \author pharvey (5/7/20)
 */
class DStencilsWidget : public WStackedWidget 
{
    Q_OBJECT
public:
    explicit DStencilsWidget( QWidget *pParent, DStencils *pStencils = nullptr );
    virtual ~DStencilsWidget();

public slots:
    virtual void slotCreated( ADObject *p );
    virtual void slotDeleted( ADObject *p );
    virtual void slotChildCreated( ADObject *p );
    virtual void slotChildDeleted( ADObject *p );
    virtual void slotModified( ADObject *pStencil );

protected:
    DStencils *pStencils;

    virtual void doLoad( DStencils *pStencils );
    virtual void doClear();
};

#endif

