#ifndef H_AWDevWidget
#define H_AWDevWidget

#include <CBD.h>

#include <ADObject.h>

class AWDevScriptWidget;

class AWDevWidget : public QTabWidget
{
    Q_OBJECT
public:
    AWDevWidget( QWidget *pParent );

protected:
    AWDevScriptWidget *pScript;
};

#endif

