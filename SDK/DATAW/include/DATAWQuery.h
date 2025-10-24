#ifndef H_DATAWQuery
#define H_DATAWQuery

#include <AWQuery.h>

/*!
 * \brief Base class for all of our item objects.
 * 
 * \author pharvey (12/5/20)
 */
class DATAWQuery : public AWQuery
{
    Q_OBJECT
public:
    explicit DATAWQuery( ADObject *pParent, const QString &stringName = QString() );
    virtual ~DATAWQuery();

};

#endif

