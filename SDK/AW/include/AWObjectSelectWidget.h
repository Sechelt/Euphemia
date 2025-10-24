#ifndef AWOBJECTSELECTWIDGET_H
#define AWOBJECTSELECTWIDGET_H

#include "AWObject.h"

class AWObjectSelectModel;

class AWObjectSelectWidget : public QTableWidget
{
    Q_OBJECT
public:
    AWObjectSelectWidget( QWidget *pwidgetParent, const QList<ADObject*> &l, bool bMultiSelect = true );
    virtual ~AWObjectSelectWidget();

    // SETTERS
    virtual void setSelected( ADObject *pObject );
    virtual void setSelected( const QList<ADObject*> &l );

    // GETTERS
    virtual QList<ADObject*> getSelected();

protected:
    QList<ADObject*> listObjects;
};

#endif 


