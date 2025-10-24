#ifndef H_DATAWPropDialog
#define H_DATAWPropDialog

#include <QtWidgets>

class DATAWQuery;

/*!
 * \class DATAWPropDialog 
 *  
 * \brief A dialog box for showing the items OPropWidget.
 *  
 * \author pharvey (1/21/20)
 */
class DATAWPropDialog : public QDialog
{
    Q_OBJECT
public:
    DATAWPropDialog( DATAWQuery *pItem, QWidget *pWidgetParent );

protected:
    virtual void doLoadState();
    virtual void doSaveState();

protected slots:
    virtual void slotOk();
};

#endif

