#ifndef H_DATAProfileWidget
#define H_DATAProfileWidget

#include <CBD.h>

class DATAProfile;

/*!
 * \brief A tab widget with tabs for various data source profile info.
 * 
 * \author pharvey (2020-06-08)
 */
class DATAProfileWidget : public QTabWidget
{
    Q_OBJECT
public:
    DATAProfileWidget( DATAProfile *pProfile, QWidget *pParent );
};

#endif 


