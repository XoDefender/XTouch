#ifndef _OcctGtkViewer_HeaderFile
#define _OcctGtkViewer_HeaderFile

#include <AIS_InteractiveContext.hxx>
#include <AIS_ViewController.hxx>
#include <AIS_ViewCube.hxx>
#include <AIS_Shape.hxx>
#include <AIS_ConnectedInteractive.hxx>

#include <Aspect_NeutralWindow.hxx>

#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>

#include <TopoDS_Solid.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TDataStd_Name.hxx>

#include <XCAFPrs_DocumentExplorer.hxx>

#include "ViewerInteractor.hxx"
#include "ControlPanel.hxx"

//! GTK window widget with embedded OCCT Viewer.
class OcctGtkViewer : public Gtk::Window, public AIS_ViewController, public ControlPanel
{
public:
  OcctGtkViewer(std::string pathToFile);
  void OpenWindow(std::string pathToFile);
  void CenterModel();
  void HideWidgets();
  int CreateModelTree();
  bool DisplayModel();

protected:
  //! Handle mouse movement event.
  bool onMouseMotion(GdkEventMotion *theEvent);

  //! Handle mouse button press event.
  bool onMouseButtonPressed(GdkEventButton *theEvent);

  //! Handle mouse button released event.
  bool onMouseButtonReleased(GdkEventButton *theEvent);

  //! Handle mouse scroll event.
  bool onMouseScroll(GdkEventScroll *theEvent);

  bool onMouseLeaveEvent(GdkEventCrossing *theEvent);

protected:
  //! Allocate OpenGL resources.
  void onGlAreaRealized();

  //! Release OpenGL resources
  void onGlAreaReleased();

  //! Redraw viewer content.
  bool onGlAreaRender(const Glib::RefPtr<Gdk::GLContext> &theGlCtx);

protected:
  //! Print OpenGL context info.
  void dumpGlInfo(bool theIsBasic);

  //! Initialize pixel scale ratio.
  void initPixelScaleRatio();

  //! Handle view redraw.
  virtual void handleViewRedraw(const Handle(AIS_InteractiveContext) & theCtx,
                                const Handle(V3d_View) & theView) override;

protected:
  bool GoBack(GdkEventButton *theEvent);
  bool GoHome(GdkEventButton *theEvent);

  void CSSConnection();
  void ProcessWidgets();

  void ConstructTree();
  void OnSelectChanged();

  bool HideWindow();

  void onValueChanged(const Glib::RefPtr<Gtk::Adjustment> &theAdj);

  void OnScaleBarValueChanged(Gtk::Scale *scale);

  bool OnScreenshotBtnClick(GdkEventButton *theEvent);
  bool MakeScreenshot();
  void SelectScreenArea(int *x, int *y, int *width, int *height);

  bool OnViewPerspective(GdkEventButton *theEvent);
  bool OnViewRight(GdkEventButton *theEvent);
  bool OnViewUp(GdkEventButton *theEvent);
  bool OnViewForward(GdkEventButton *theEvent);
  bool Center(GdkEventButton *theEvent);

  void AddTreeElement(int countSymbols,
                      TCollection_AsciiString currentModelName,
                      bool isParent,
                      int labelId);

  int TraverseLabel(const TDF_Label &theLabel,
                    const TCollection_AsciiString &theNamePrefix,
                    const TopLoc_Location &theLoc);

  int TraverseLabel(const TDF_Label &theLabel,
                    const TCollection_AsciiString &theNamePrefix,
                    const TopLoc_Location &theLoc,
                    int labelId, TDF_Label &foundLabel);

  int FindModelPartLabelById(int labelId, TDF_Label &foundLabel);

  void DisplayModelPart(const TDF_Label &label,
                        const TopLoc_Location &parentTrsf,
                        const XCAFPrs_Style &parentStyle,
                        const TCollection_AsciiString &parentId,
                        NCollection_DataMap<TDF_Label,
                                            NCollection_List<Handle(AIS_InteractiveObject)>,
                                            TDF_LabelMapHasher> &mapOfOriginals);

  enum ExplodeDirection
  {
    UP,
    RIGHT,
    FORWARD
  };

  void ExplodeModel(double distance, ExplodeDirection explodeDirection);

  void ClearDataAboutDisplayedModel();
  void RestoreUI();
  void UpdateView();

  void InitializeOcctDoc(std::string pathToFile);
  void SelectModelPart(TDF_Label modelPartLabel);

  void MakeInitialExplotion();
  void MakeInitialAssembly();

protected:
  Handle(V3d_Viewer) myViewer;
  Handle(V3d_View) myView;
  Handle(AIS_InteractiveContext) myContext;
  Handle(AIS_ViewCube) myViewCube;
  Handle(ViewerInteractor) viewerInteractor;
  Handle(TDocStd_Document) occtDoc;
  Handle(TDocStd_Application) occtApp;
  Handle(AIS_ConnectedInteractive) brepConnected;

protected:
  float myDevicePixelRatio = 1.0f;

  std::string pathToFile;
  bool isScreenshotBtnActivated = true;
  bool isExploadBarVisible = false;

  int currentIndex = 0;
  std::vector<TDF_Label> labels = {};
  std::vector<Handle(AIS_ConnectedInteractive)> aisObjects = {};
  std::vector<gp_Trsf> aisObjectsParentTransformations = {};

  sigc::connection motionNotifyConn;

  double scaleVal = 0;

protected:
  Glib::RefPtr<Gtk::Builder> ui;
  Gtk::GLArea *myGLArea;
  Gtk::Box *viewportScreen;
  Gtk::Scale *scale;
  Gtk::Label *fileName;

  Gtk::EventBox *goBackBtn;
  Gtk::EventBox *goHomeBtn;
  Gtk::EventBox *centerBtn;

  Gtk::EventBox *viewPerspective;
  Gtk::EventBox *viewRight;
  Gtk::EventBox *viewUp;
  Gtk::EventBox *viewForward;

  Gtk::EventBox *fullScreen;
  Gtk::EventBox *windowScreen;
  Gtk::EventBox *closeScreen;

  Gtk::EventBox *screenshotBtn;

  Gtk::TreeView *treeView;
  Glib::RefPtr<Gtk::TreeStore> treeStore;
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection;

  Gtk::TreeModel::Row row;
  Gtk::TreeModel::Row childrow1;
  Gtk::TreeModel::Row childrow2;
  Gtk::TreeModel::Row childrow3;
  Gtk::TreeModel::Row childrow4;
  Gtk::TreeModel::Row childrow5;

  Gtk::EventBox *showTreeBtn;
  Gtk::EventBox *hideTreeBtn;

  Gtk::EventBox *showControlsBtn;
  Gtk::EventBox *hideControlsBtn;

  Gtk::EventBox *explodeBtn;

  Gtk::Box *treeBox;
  Gtk::Box *controlsBox;

  double explotionDistance = 0.6;
  int explotionTimeDelay = 10;
  double explotionStep = 0.03;

private:
  class ModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:
    ModelColumns()
    {
      add(m_col_id);
      add(m_col_name);
    }

    Gtk::TreeModelColumn<int> m_col_id;
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
  };

  ModelColumns m_Columns;
};

#endif // _OcctGtkViewer_HeaderFile