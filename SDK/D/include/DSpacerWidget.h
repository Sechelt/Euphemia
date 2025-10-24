#ifndef H_DSpacerWidget
#define H_DSpacerWidget

#include <CBD.h>

#include "DSpacer.h"

class WLayoutWidget;

/*!
 * \brief Widget for editing DSpacer specific properties.
 * 
 * \author pharvey (9/11/20)
 */
class DSpacerWidget : public QWidget
{
    Q_OBJECT
public:
    DSpacerWidget( DSpacer *pSpacer, QWidget *pParent, bool bToolBar = false );

protected slots:
    virtual void slotChangedType( int nIndex );                     // control changed so request change in object
    virtual void slotChangedType( DSpacer::SizeTypes nType );       // object changed (or denied request) so update control

protected:
    DSpacer *               pSpacer;
    QComboBox *             pTypes;
    WLayoutWidget *         pLayoutWidget;
};

#endif

