#ifndef ADEXPORT_H
#define ADEXPORT_H

#include <CBD.h>

class ADExportHTML
{
public:
    ADExportHTML() 
    {
        pprogress = NULL;
    }
    virtual ~ADExportHTML() 
    {
        if ( pprogress )
            delete pprogress;
    }
    QProgressDialog *   pprogress;
    QString             stringDir;
    QFile               fileBrowser;
    QTextStream         streamBrowser;
    QString             stringDescHeader;
    QString             stringDescFooter;
};

class ADExportRTF
{
public:
    QFile   file;
};

#endif


