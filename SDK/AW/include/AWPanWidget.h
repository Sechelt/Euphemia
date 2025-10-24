#ifndef AWPANWIDGET_H
#define AWPANWIDGET_H

#include <CBD.h>

// class ADObject;
class AWEditorWidget;

class AWPanWidget : public QWidget
{
    Q_OBJECT
public:
    AWPanWidget( AWEditorWidget *pEditor, QWidget *pwidgetParent );
    virtual ~AWPanWidget();

    virtual AWEditorWidget *getEditor() { return pEditor; }

protected:
    AWEditorWidget *pEditor;
};

#endif 

