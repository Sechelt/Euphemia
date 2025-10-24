#ifndef H_AWPalWidget
#define H_AWPalWidget

#include <CBD.h>

class AWEditorWidget;

/*!
 * \class AWPalWidget 
 * \brief Base class for widgets shown in a pallette dock window. 
 *  
 * A pallette widget provides things that need to be handy when editing. 
 *  
 * For example; 
 * diagram - a pallette of shapes that can be dragged to the editor 
 * SQL editor - a tree widget which soft seeks to context and provides drag to editor (for tables, columns etc)
 *  
 * An AWEditorWidget provides one of these via \sa ADObject::getPalWidget().  
 *  
 * This can be used in two ways; 
 * 1. derive a new class from this and then return an instance via getPalWidget 
 * 2. create an instance of this, with widgets layed out on it, and then return it via getPalWidget  
 *  
 * The application will display this appropriately (perhaps in a widget stack). The 
 * application should not have to interact with the widget or any of its children. The 
 * interaction should be with the editor.
 *  
 * \sa AWDataWidget 
 * \sa AWPropWidget 
 *  
 * \author pharvey (11/17/19)
 */
class AWPalWidget : public QWidget
{
    Q_OBJECT
public:
    AWPalWidget( AWEditorWidget *pEditor, QWidget *pwidgetParent );
    virtual ~AWPalWidget();

    virtual AWEditorWidget *getEditor() { return pEditor; }

protected:
    AWEditorWidget *pEditor;
};

#endif 

