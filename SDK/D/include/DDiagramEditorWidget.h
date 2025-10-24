#ifndef H_DDiagramEditorWidget
#define H_DDiagramEditorWidget

#include <WZoomWidget.h>
#include <WPen.h>
#include <WBrush.h>
#include <WText.h>
#include <WObjectHAlignComboBox.h>
#include <WObjectVAlignComboBox.h>
#include <WShapeSizeComboBox.h>
#include <WCoordWidget.h>
#include <WRulerWidget.h>

#include <ADObjectSelectionManager.h>

#include <AWEditorWidget.h>

class DGraphicsSceneProxy;
class DGraphicsView;
class DDiagram;
class DObject;

class DDiagramEditorWidget : public AWEditorWidget
{
    Q_OBJECT
public:
    DDiagramEditorWidget( DDiagram *pDiagram, QWidget *pParent = nullptr );
    virtual ~DDiagramEditorWidget();

    // SETTERS
    virtual void setRuler( bool b = true );

    // GETTERS
    virtual DDiagram *              getDiagram();
    virtual AWPalWidget *           getPalWidget() override;
    virtual AWPanWidget *           getPanWidget() override;

    virtual DGraphicsSceneProxy *   getScene();
    virtual DGraphicsView *         getView() { return pView; }
    virtual WRulerWidget *          getRuler( Qt::Orientation n );
    virtual bool                    isRuler() { return pRulerCorner; };

    virtual void doGoTo( DObject *p );

protected:
    DDiagram *                  pDiagram;
    
    // selection
    // ADObjectSelectionManager *  pSelectionManager;
    // QList<ADObject *>           listSelected;

    DGraphicsView * pView;
    QVBoxLayout *   pLayoutCentral;

    // menu
    QMenu *pMenuEdit;
    QAction *pActionCut; 
    QAction *pActionCopy; 
    QAction *pActionPaste; 
    QAction *pActionUndo;
    QAction *pActionRedo;
    QAction *pActionSelectAll;
    QAction *pActionSelectNone;
    QAction *pActionGroup;
    QAction *pActionUngroup;
    QAction *pActionToFront;
    QAction *pActionToBack;
    QAction *pActionOptimizeSize;
    QAction *pActionSize16;
    QAction *pActionSize32;
    QAction *pActionSize48;
    QAction *pActionSize64;
    QAction *pActionSize96;
    QAction *pActionSize128;
    QAction *pActionResetTransformation;
    QAction *pActionSnapGrid; 
    QAction *pActionDelete; 

    QMenu *pMenuView;
    QAction *pActionPageBreaks; 
    QAction *pActionGrid; 
    QAction *pActionRuler; 
    QAction *pActionCrossHairs; 

    QMenu *pMenuSettings;
    QAction *pActionSettingsGrid; 
    QAction *pActionSettingsRuler; 

    // toolbar controls
    WPenButton *            pPenButton;
    WBrushButton *          pBrushButton;
    WTextButton *           pTextButton;
    WObjectHAlignComboBox * pAlignHObjectWidget;
    WObjectVAlignComboBox * pAlignVObjectWidget;
    WShapeSizeComboBox *    pShapeSize;
    QComboBox *             pClassesWidget;

    // statusbar controls
    WCoordWidget *  plabelCell;
    WCoordWidget *  plabelPixel;
    WZoomWidget *   pZoom;

    // ruler
    QHBoxLayout *       pLayoutHTop;
    QVBoxLayout *       pLayoutRulerLeft;
    QVBoxLayout *       pLayoutVTop;
    WRulerCornerWidget *pRulerCorner;
    WRulerWidget *      pRulerH;
    WRulerWidget *      pRulerV;
 
    virtual void doInitCentralArea();
    virtual void doInitActions();
    virtual void doInitMenus();
    virtual void doInitToolbar();
    virtual void doInitStatusBar();

protected slots:
    virtual void slotSelectionChanged( bool );
    virtual void slotSettingRulerChanged();
    virtual void slotClipboardChanged( QClipboard::Mode );
};

#endif


