#ifndef H_AWClassNameWidget
#define H_AWClassNameWidget

#include <CBD.h>

/*!
 * \class AWClassNameWidget 
 * \brief Provides a text edit widget for use as the 1st tab in a Class widget. 
 *  
 * The idea here is that derived classes can catch this (using this class name) 
 * in their \sa getClassWidget and set the text. 
 *  
 * Ideas for the text; 
 *      - icon 
 *      - description 
 *      - version 
 *  
 * The line edit is read-only. 
 * 
 * \author pharvey (1/21/20)
 */
class AWClassNameWidget : public QTextEdit
{
    Q_OBJECT
public:
    AWClassNameWidget( QWidget *pwidgetParent );

    static QString getCompanyInfo();
    static QString getProductInfo();
};

#endif


