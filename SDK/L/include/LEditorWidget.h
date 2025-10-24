#ifndef H_LEditorWidget
#define H_LEditorWidget

#include <WRulerWidget.h>
#include <AWEditorWidget.h>
#include <SSetting.h>

class LGraphicsScene;
class LGraphicsView;
class LFile;

class LEditorWidget : public AWEditorWidget
{
    Q_OBJECT
public:
    LEditorWidget( LFile *pFile, QWidget *pParent = nullptr );
    virtual ~LEditorWidget();

    // SETTERS
    virtual void setRuler( bool b = true );

    // GETTERS
    virtual LFile *                 getFile();

    virtual LGraphicsScene *        getScene();
    virtual LGraphicsView *         getView() { return pView; }
    virtual WRulerWidget *          getRuler( Qt::Orientation n );

    virtual bool                    hasRuler() { return pRulerCorner; };

protected:
    LFile *         pFile;
    LGraphicsView * pView;
    QVBoxLayout *   pLayoutCentral;

    // setting
    SSetting *      pSettingGeneral = nullptr;

    // ruler
    QHBoxLayout *       pLayoutHTop;
    QVBoxLayout *       pLayoutRulerLeft;
    QVBoxLayout *       pLayoutVTop;
    WRulerCornerWidget *pRulerCorner;
    WRulerWidget *      pRulerH;
    WRulerWidget *      pRulerV;

    virtual void doInitCentralArea();

protected slots:
    virtual void slotRefreshGeneral();
};

#endif


