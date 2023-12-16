#include "global.hxx"

#include "OcctGtkViewer.hxx"

#include <OpenGl_Context.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_FrameBuffer.hxx>
#include <EGL/egl.h>

#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>

#include <TopoDS_Iterator.hxx>

#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFPrs_AISObject.hxx>
#include <XCAFPrs_Style.hxx>
#include <BinXCAFDrivers.hxx>

#include <STEPCAFControl_Reader.hxx>

#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <gtk/gtk.h>

#include <cairo.h>
#include <cairo-xlib.h>

#include "InFolderWindow.hxx"
#include "MainWindow.hxx"

using namespace std;

namespace GlobalFunctions
{
  // содержит ли деталь потомков
  bool IsEmptyShape(const TopoDS_Shape &shape)
  {
    if (shape.IsNull())
      return true;

    if (shape.ShapeType() >= TopAbs_FACE)
      return false;

    int numSubShapes = 0;
    for (TopoDS_Iterator it(shape); it.More(); it.Next())
      numSubShapes++;

    return numSubShapes == 0;
  }

  int GetLabelIndex(vector<TDF_Label> v, TDF_Label K)
  {
    auto it = find(v.begin(), v.end(), K);

    // If element was found
    if (it != v.end())
    {
      int index = it - v.begin();
      return index;
    }
    else
    {
      // If the element is not
      // present in the vector
      return -1;
    }
  }
}

bool OcctGtkViewer::HideWindow()
{
  hide();
  return false;
}

void OcctGtkViewer::AddTreeElement(int depthLevel, TCollection_AsciiString modelPartName, bool isParent, int labelId)
{
  if (isParent)
  {
    if (depthLevel == 1)
    {
      row = *(treeStore->append());
      row[m_Columns.m_col_id] = labelId;
      row[m_Columns.m_col_name] = modelPartName.ToCString();
    }
    else if (depthLevel == 2)
    {
      childrow1 = *(treeStore->append(row.children()));
      childrow1[m_Columns.m_col_id] = labelId;
      childrow1[m_Columns.m_col_name] = modelPartName.ToCString();
    }
    else if (depthLevel == 3)
    {
      childrow2 = *(treeStore->append(childrow1.children()));
      childrow2[m_Columns.m_col_id] = labelId;
      childrow2[m_Columns.m_col_name] = modelPartName.ToCString();
    }
    else if (depthLevel == 4)
    {
      childrow3 = *(treeStore->append(childrow2.children()));
      childrow3[m_Columns.m_col_id] = labelId;
      childrow3[m_Columns.m_col_name] = modelPartName.ToCString();
    }
    else if (depthLevel == 5)
    {
      childrow4 = *(treeStore->append(childrow3.children()));
      childrow4[m_Columns.m_col_id] = labelId;
      childrow4[m_Columns.m_col_name] = modelPartName.ToCString();
    }
    else if (depthLevel == 6)
    {
      childrow5 = *(treeStore->append(childrow4.children()));
      childrow5[m_Columns.m_col_id] = labelId;
      childrow5[m_Columns.m_col_name] = modelPartName.ToCString();
    }
  }
  else
  {
    if (depthLevel == 0)
    {
      row = *(treeStore->append());
      row[m_Columns.m_col_id] = labelId;
      row[m_Columns.m_col_name] = modelPartName.ToCString();
    }
    else if (depthLevel == 1)
    {
      childrow1 = *(treeStore->append(row.children()));
      childrow1[m_Columns.m_col_id] = labelId;
      childrow1[m_Columns.m_col_name] = modelPartName.ToCString();
    }
    else if (depthLevel == 2)
    {
      childrow2 = *(treeStore->append(childrow1.children()));
      childrow2[m_Columns.m_col_id] = labelId;
      childrow2[m_Columns.m_col_name] = modelPartName.ToCString();
    }
    else if (depthLevel == 3)
    {
      childrow3 = *(treeStore->append(childrow2.children()));
      childrow3[m_Columns.m_col_id] = labelId;
      childrow3[m_Columns.m_col_name] = modelPartName.ToCString();
    }
    else if (depthLevel == 4)
    {
      childrow4 = *(treeStore->append(childrow3.children()));
      childrow4[m_Columns.m_col_id] = labelId;
      childrow4[m_Columns.m_col_name] = modelPartName.ToCString();
    }
    else if (depthLevel == 5)
    {
      childrow5 = *(treeStore->append(childrow4.children()));
      childrow5[m_Columns.m_col_id] = labelId;
      childrow5[m_Columns.m_col_name] = modelPartName.ToCString();
    }
  }
}

// Обрабатываем каждый отдельный лейбл (модель)
int OcctGtkViewer::TraverseLabel(const TDF_Label &theLabel,
                                 const TCollection_AsciiString &theNamePrefix,
                                 const TopLoc_Location &theLoc)
{
  TDF_Label aRefLabel = theLabel;
  XCAFDoc_ShapeTool::GetReferredShape(theLabel, aRefLabel);

  Handle(TDataStd_Name) aNodeName;
  aRefLabel.FindAttribute(TDataStd_Name::GetID(), aNodeName);
  TCollection_AsciiString aName = aNodeName->Get();

  TCollection_AsciiString modelPartName = aName;

  aName = theNamePrefix + aName;

  // если у части модели есть потомки
  if (XCAFDoc_ShapeTool::IsAssembly(aRefLabel))
  {
    aName += "/";
    string str = aName.ToCString();
    int countSymbols = count(str.begin(), str.end(), '/');

    if (!modelPartName.IsEmpty())
      AddTreeElement(countSymbols, modelPartName, true, aRefLabel.Tag());

    const TopLoc_Location aLoc = theLoc * XCAFDoc_ShapeTool::GetLocation(theLabel);
    for (TDF_ChildIterator aChildIter(aRefLabel); aChildIter.More(); aChildIter.Next())
    {
      // рекурсивно проходимся по потомкам в глубину
      if (TraverseLabel(aChildIter.Value(), aName, aLoc) == 1)
      {
        return 1;
      }
    }

    return 0;
  }

  // считаем количество разделителей вложенности, тем самым определяя уровень вложенности детали
  string str = aName.ToCString();
  int countSymbols = count(str.begin(), str.end(), '/');

  if (!modelPartName.IsEmpty())
    AddTreeElement(countSymbols, modelPartName, false, aRefLabel.Tag());

  return 0;
}

void OcctGtkViewer::SelectModelPart(TDF_Label modelPartLabel)
{
  currentIndex = GlobalFunctions::GetLabelIndex(labels, modelPartLabel);
  if (currentIndex != -1)
  {
    myContext->SetSelected(aisObjects[currentIndex], true);
    viewerInteractor->ProcessKeyPress(Aspect_VKey_F);
    UpdateView();
  }
}

int OcctGtkViewer::TraverseLabel(const TDF_Label &theLabel,
                                 const TCollection_AsciiString &theNamePrefix,
                                 const TopLoc_Location &theLoc, int labelId, TDF_Label &foundLabel)
{
  TDF_Label aRefLabel = theLabel;
  XCAFDoc_ShapeTool::GetReferredShape(theLabel, aRefLabel);

  Handle(TDataStd_Name) aNodeName;
  aRefLabel.FindAttribute(TDataStd_Name::GetID(), aNodeName);
  TCollection_AsciiString aName = aNodeName->Get();

  if (aRefLabel.Tag() == labelId)
  {
    foundLabel = aRefLabel;

    return 1;
  }

  // если у части модели есть потомки
  if (XCAFDoc_ShapeTool::IsAssembly(aRefLabel))
  {
    const TopLoc_Location aLoc = theLoc * XCAFDoc_ShapeTool::GetLocation(theLabel);
    for (TDF_ChildIterator aChildIter(aRefLabel); aChildIter.More(); aChildIter.Next())
    {
      // рекурсивно проходимся по потомкам в глубину
      if (TraverseLabel(aChildIter.Value(), aName, aLoc, labelId, foundLabel) == 1)
      {
        return 1;
      }
    }

    return 0;
  }

  return 0;
}

// получаем иерархию лейблов из документа модели
int OcctGtkViewer::CreateModelTree()
{
  TDF_LabelSequence aLabels;
  XCAFDoc_DocumentTool::ShapeTool(occtDoc->Main())->GetFreeShapes(aLabels);

  for (TDF_LabelSequence::Iterator aLabIter(aLabels); aLabIter.More(); aLabIter.Next())
  {
    const TDF_Label &aLabel = aLabIter.Value();
    if (TraverseLabel(aLabel, "", TopLoc_Location()) == 1)
    {
      return 1;
    }
  }

  return 0;
}

int OcctGtkViewer::FindModelPartLabelById(int labelId, TDF_Label &foundLabel)
{
  TDF_LabelSequence aLabels;
  XCAFDoc_DocumentTool::ShapeTool(occtDoc->Main())->GetFreeShapes(aLabels);

  for (TDF_LabelSequence::Iterator aLabIter(aLabels); aLabIter.More(); aLabIter.Next())
  {
    const TDF_Label &aLabel = aLabIter.Value();
    if (TraverseLabel(aLabel, "", TopLoc_Location(), labelId, foundLabel) == 1)
    {
      return 1;
    }
  }

  return 0;
}

bool OcctGtkViewer::DisplayModel()
{
  if (occtDoc.IsNull())
    return true;

  // Clear the viewer.
  myContext->RemoveAll(false);

  // Get XDE tools.
  Handle(XCAFDoc_ShapeTool)
      ShapeTool = XCAFDoc_DocumentTool::ShapeTool(occtDoc->Main());

  // Get root shapes to visualize.
  TDF_LabelSequence roots;
  ShapeTool->GetFreeShapes(roots);

  // Prepare default style.
  XCAFPrs_Style defaultStyle;
  defaultStyle.SetColorSurf(Quantity_NOC_GREEN);
  defaultStyle.SetColorCurv(Quantity_Color(0.0, 0.4, 0.0, Quantity_TOC_sRGB));

  // Visualize objects recursively.
  NCollection_DataMap<TDF_Label, NCollection_List<Handle(AIS_InteractiveObject)>, TDF_LabelMapHasher> mapOfOriginals;

  for (TDF_LabelSequence::Iterator lit(roots); lit.More(); lit.Next())
  {
    const TDF_Label &L = lit.Value();
    TopLoc_Location parentLoc = ShapeTool->GetLocation(L);

    try
    {
      DisplayModelPart(L, parentLoc, defaultStyle, "", mapOfOriginals);
    }
    catch (...)
    {
      TCollection_AsciiString entry;
      TDF_Tool::Entry(L, entry);
    }
  }

  return true;
}

void OcctGtkViewer::DisplayModelPart(const TDF_Label &label,
                                     const TopLoc_Location &parentTrsf,
                                     const XCAFPrs_Style &parentStyle,
                                     const TCollection_AsciiString &parentId,
                                     NCollection_DataMap<TDF_Label,
                                                         NCollection_List<Handle(AIS_InteractiveObject)>,
                                                         TDF_LabelMapHasher> &mapOfOriginals)

{
  // Get XDE tools.
  Handle(XCAFDoc_ShapeTool) ShapeTool = XCAFDoc_DocumentTool::ShapeTool(occtDoc->Main());
  Handle(XCAFDoc_ColorTool) ColorTool = XCAFDoc_DocumentTool::ColorTool(occtDoc->Main());

  // Get referred label for instances or root refs.
  TDF_Label refLabel = label;
  //
  if (ShapeTool->IsReference(label))
    ShapeTool->GetReferredShape(label, refLabel);

  // Build path ID which is the unique identifier of the assembly item
  // in the hierarchical assembly graph.
  TCollection_AsciiString itemId;
  TDF_Tool::Entry(label, itemId);
  //
  if (!parentId.IsEmpty())
  {
    itemId.Prepend("/");
    itemId.Prepend(parentId);
  }

  // If the label contains a part and not an assembly, we can create the
  // corresponding AIS object. All part instances will reference that object.
  if (!ShapeTool->IsAssembly(refLabel))
  {
    Handle(AIS_ConnectedInteractive) brepConnected;

    Handle(TCollection_HAsciiString) hItemId = new TCollection_HAsciiString(itemId);

    // Use AIS_ConnectedInteractive to refer to the same AIS objects instead of
    // creating copies. That's the typical instancing thing you'd expect to have
    // in any good enough 3D graphics API.
    NCollection_List<Handle(AIS_InteractiveObject)> *aisListPtr = mapOfOriginals.ChangeSeek(refLabel);

    if (aisListPtr == nullptr)
    {
      NCollection_List<Handle(AIS_InteractiveObject)> itemRepresList;

      //* set BRep representation
      TopoDS_Shape shape = ShapeTool->GetShape(refLabel);

      if (!GlobalFunctions::IsEmptyShape(shape))
      {
        /* Construct the original AIS object and create a connected interactive
         * object right away. The thing is that we never show the original objects
         * themselves. We always have reference objects in our scene.
         */

        // Get label ID.
        TCollection_AsciiString refEntry;
        TDF_Tool::Entry(refLabel, refEntry);

        Handle(AIS_Shape) brepPrs = new XCAFPrs_AISObject(refLabel);

        // Connected.
        brepConnected = new AIS_ConnectedInteractive();
        brepConnected->Connect(brepPrs);

        itemRepresList.Append(brepPrs);
      }

      aisListPtr = mapOfOriginals.Bound(refLabel, itemRepresList);
    }
    else
    {
      /* If here, we are not going to create an original AIS object, but
       * we still have to construct the connected interactive object and
       * make a link to the already existing original AIS shape.
       */

      NCollection_List<Handle(AIS_InteractiveObject)>::Iterator it(*aisListPtr);

      for (; it.More(); it.Next())
      {
        const Handle(AIS_InteractiveObject) &aisOriginal = it.Value();

        if (aisOriginal->IsKind(STANDARD_TYPE(XCAFPrs_AISObject)))
        {
          Handle(XCAFPrs_AISObject) brepPrs = Handle(XCAFPrs_AISObject)::DownCast(it.Value());

          const TDF_Label &originalLab = brepPrs->GetLabel();
          TCollection_AsciiString originalEntry;
          TDF_Tool::Entry(originalLab, originalEntry);

          // Connected.
          brepConnected = new AIS_ConnectedInteractive();
          brepConnected->Connect(brepPrs);
        }
      }
    }

    if (!brepConnected.IsNull())
    {
      labels.push_back(refLabel);
      aisObjects.push_back(brepConnected);
      aisObjectsParentTransformations.push_back(parentTrsf.Transformation());

      brepConnected->SetLocalTransformation(parentTrsf);
      brepConnected->SetDisplayMode(AIS_Shaded);

      try
      {
        myContext->Display(brepConnected, false);
      }
      catch (...)
      {
        std::cout << "DisplayScene::Execute(): invalid shape for item '"
                  << itemId.ToCString()
                  << "'"
                  << std::endl;

        myContext->Remove(brepConnected, Standard_False);
        mapOfOriginals.UnBind(refLabel);
      }
    }

    return; // We're done
  }

  // Разукрашиваем деталь
  XCAFPrs_Style defStyle = parentStyle;
  Quantity_ColorRGBA color;

  // In case of an assembly, move on to the nested component.
  for (TDF_ChildIterator childIt(refLabel); childIt.More(); childIt.Next())
  {
    TDF_Label childLabel = childIt.Value();

    if (!childLabel.IsNull() && (childLabel.HasAttribute() || childLabel.HasChild()))
    {
      TopLoc_Location trsf = parentTrsf * ShapeTool->GetLocation(childLabel);
      DisplayModelPart(childLabel, trsf, defStyle, itemId, mapOfOriginals);
    }
  }
}

//! Native window wrapper.
class OcctGtkWindow : public Aspect_NeutralWindow
{
public:
  //! Constructor.
  OcctGtkWindow() {}

  //! Return device pixel ratio (logical to backing store scale factor).
  virtual Standard_Real DevicePixelRatio() const override { return myPixelRatio; }

  //! Set pixel ratio.
  void SetDevicePixelRatio(Standard_Real theRatio) { myPixelRatio = theRatio; }

private:
  double myPixelRatio = 1.0;
};

class OcctQtFrameBuffer : public OpenGl_FrameBuffer
{
  DEFINE_STANDARD_RTTI_INLINE(OcctQtFrameBuffer, OpenGl_FrameBuffer)
public:
  //! Empty constructor.
  OcctQtFrameBuffer() {}

  //! Make this FBO active in context.
  virtual void BindBuffer(const Handle(OpenGl_Context) & theGlCtx) override
  {
    OpenGl_FrameBuffer::BindBuffer(theGlCtx);
    theGlCtx->SetFrameBufferSRGB(true, false);
  }

  //! Make this FBO as drawing target in context.
  virtual void BindDrawBuffer(const Handle(OpenGl_Context) & theGlCtx) override
  {
    OpenGl_FrameBuffer::BindDrawBuffer(theGlCtx);
    theGlCtx->SetFrameBufferSRGB(true, false);
  }

  //! Make this FBO as reading source in context.
  virtual void BindReadBuffer(const Handle(OpenGl_Context) & theGlCtx) override
  {
    OpenGl_FrameBuffer::BindReadBuffer(theGlCtx);
  }
};

void OcctGtkViewer::MakeInitialExplotion()
{
  Glib::signal_timeout().connect([this]()
                                 {
                                  if(scaleVal >= explotionDistance)
                                  {
                                    return false;
                                  }
                                  else
                                  {
                                    scaleVal += explotionStep;
                                    scale->set_value(scaleVal);
                                    OnScaleBarValueChanged(scale);

                                    return true;
                                  } },
                                 explotionTimeDelay);
}

void OcctGtkViewer::MakeInitialAssembly()
{
  Glib::signal_timeout().connect([this]()
                                 {
                                  if(scaleVal <= 0)
                                  {
                                    return false;
                                  }
                                  else
                                  {
                                    scaleVal -= explotionStep;
                                    scale->set_value(scaleVal);
                                    OnScaleBarValueChanged(scale);

                                    return true;
                                  } },
                                 explotionTimeDelay);
}

void OcctGtkViewer::ProcessWidgets()
{
  ui = Gtk::Builder::create_from_file(global::viewportWindowUI);

  ui->get_widget<Gtk::TreeView>("TreeView", treeView);
  ui->get_widget<Gtk::Box>("ViewportScreen", viewportScreen);
  ui->get_widget<Gtk::GLArea>("GLArea", myGLArea);
  ui->get_widget<Gtk::Scale>("Scale", scale);
  ui->get_widget<Gtk::Label>("FileName", fileName);
  ui->get_widget<Gtk::EventBox>("BackBtn", goBackBtn);
  ui->get_widget<Gtk::EventBox>("CenterBtn", centerBtn);
  ui->get_widget<Gtk::EventBox>("ViewPerspective", viewPerspective);
  ui->get_widget<Gtk::EventBox>("ViewRight", viewRight);
  ui->get_widget<Gtk::EventBox>("ViewUp", viewUp);
  ui->get_widget<Gtk::EventBox>("ViewForward", viewForward);
  ui->get_widget<Gtk::EventBox>("ExplodeBtn", explodeBtn);
  ui->get_widget<Gtk::EventBox>("Screenshot", screenshotBtn);

  ui->get_widget<Gtk::EventBox>("ShowTreeBtn", showTreeBtn);
  ui->get_widget<Gtk::EventBox>("HideTreeBtn", hideTreeBtn);
  ui->get_widget<Gtk::EventBox>("ShowControlsBtn", showControlsBtn);
  ui->get_widget<Gtk::EventBox>("HideControlsBtn", hideControlsBtn);

  ui->get_widget<Gtk::Box>("TreeBox", treeBox);
  ui->get_widget<Gtk::Box>("ControlsBox", controlsBox);

  refTreeSelection = treeView->get_selection();
  treeStore = Glib::RefPtr<Gtk::TreeStore>::cast_dynamic(ui->get_object("TreeStore"));

  fileName->set_single_line_mode(true);
  fileName->set_ellipsize(Pango::ELLIPSIZE_END);
  fileName->set_text(global::currentFileName);

  explodeBtn->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                  {
    if(isExploadBarVisible)
    {
      scale->hide();
      isExploadBarVisible = false;

      if(scale->get_value() >= explotionDistance && scale->get_value() < explotionDistance + 0.1)
        MakeInitialAssembly();
    }
    else
    {
      scale->show();
      isExploadBarVisible = true;

      if(scale->get_value() == 0)
      {
        scaleVal = 0;
        MakeInitialExplotion();
      }
    }
    
    return true; });

  showTreeBtn->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                   {treeBox->show(); showTreeBtn->hide(); return true; });
  hideTreeBtn->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                   {treeBox->hide();showTreeBtn->show(); return true; });
  showControlsBtn->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                       {controlsBox->show(); showControlsBtn->hide(); return true; });
  hideControlsBtn->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                       {controlsBox->hide(); showControlsBtn->show(); return true; });

  refTreeSelection->signal_changed().connect(sigc::mem_fun(*this, &OcctGtkViewer::OnSelectChanged), false);
  scale->signal_value_changed().connect(sigc::bind(sigc::mem_fun(*this, &OcctGtkViewer::OnScaleBarValueChanged), scale));

  goBackBtn->add_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::SCROLL_MASK | Gdk::FOCUS_CHANGE_MASK);
  centerBtn->add_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::SCROLL_MASK | Gdk::FOCUS_CHANGE_MASK);
  viewPerspective->add_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::SCROLL_MASK | Gdk::FOCUS_CHANGE_MASK);
  viewRight->add_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::SCROLL_MASK | Gdk::FOCUS_CHANGE_MASK);
  viewUp->add_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::SCROLL_MASK | Gdk::FOCUS_CHANGE_MASK);
  viewForward->add_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::SCROLL_MASK | Gdk::FOCUS_CHANGE_MASK);
  screenshotBtn->add_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::SCROLL_MASK | Gdk::FOCUS_CHANGE_MASK);

  goBackBtn->signal_button_press_event().connect(sigc::mem_fun(*this, &OcctGtkViewer::GoBack), false);
  centerBtn->signal_button_press_event().connect(sigc::mem_fun(*this, &OcctGtkViewer::Center), false);
  viewPerspective->signal_button_press_event().connect(sigc::mem_fun(*this, &OcctGtkViewer::OnViewPerspective), false);
  viewRight->signal_button_press_event().connect(sigc::mem_fun(*this, &OcctGtkViewer::OnViewRight), false);
  viewUp->signal_button_press_event().connect(sigc::mem_fun(*this, &OcctGtkViewer::OnViewUp), false);
  viewForward->signal_button_press_event().connect(sigc::mem_fun(*this, &OcctGtkViewer::OnViewForward), false);
  screenshotBtn->signal_button_press_event().connect(sigc::mem_fun(*this, &OcctGtkViewer::OnScreenshotBtnClick), false);

  // connect to Gtk::GLArea events
  myGLArea->signal_unrealize().connect(sigc::mem_fun(*this, &OcctGtkViewer::onGlAreaReleased), false);
  myGLArea->signal_realize().connect(sigc::mem_fun(*this, &OcctGtkViewer::onGlAreaRealized));
  myGLArea->signal_render().connect(sigc::mem_fun(*this, &OcctGtkViewer::onGlAreaRender), false);

  // connect to mouse input events
  myGLArea->add_events(Gdk::POINTER_MOTION_MASK | Gdk::LEAVE_NOTIFY_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::SCROLL_MASK | Gdk::FOCUS_CHANGE_MASK);
  motionNotifyConn = myGLArea->signal_motion_notify_event().connect(sigc::mem_fun(*this, &OcctGtkViewer::onMouseMotion), false);
  myGLArea->signal_leave_notify_event().connect(sigc::mem_fun(*this, &OcctGtkViewer::onMouseLeaveEvent), false);
  myGLArea->signal_button_press_event().connect(sigc::mem_fun(*this, &OcctGtkViewer::onMouseButtonPressed), false);
  myGLArea->signal_button_release_event().connect(sigc::mem_fun(*this, &OcctGtkViewer::onMouseButtonReleased), false);
  myGLArea->signal_scroll_event().connect(sigc::mem_fun(*this, &OcctGtkViewer::onMouseScroll), false);

  ProcessPanel(ui, this, "ViewportScreen");
}

void OcctGtkViewer::SelectScreenArea(int *x, int *y, int *width, int *height)
{
  bool isPressed = false;
  bool isShifted = false;

  int init_x = 0;
  int init_y = 0;
  int fin_x = 0;
  int fin_y = 0;

  int prevInitX, prevInitY, prevFinX, prevFinY;

  Display *xdisplay = XOpenDisplay(0);
  XEvent xevent;
  ::Window xroot = gdk_x11_window_get_xid(gtk_widget_get_window((GtkWidget *)myGLArea->gobj()));
  int scr = DefaultScreen(xdisplay);
  bool syncFlag = true;

  cairo_surface_t *surface = cairo_xlib_surface_create(xdisplay, xroot, DefaultVisual(xdisplay, scr), DisplayWidth(xdisplay, scr), DisplayHeight(xdisplay, scr));
  cairo_t *cr = cairo_create(surface);

  const char *backgroundImagePath = "/tmp/test.png";
  cairo_surface_write_to_png(
      surface,
      backgroundImagePath);

  cairo_surface_t *surfaceTmp = cairo_image_surface_create_from_png(backgroundImagePath);

  int currentTime = 0;
  int delay = currentTime;
  printf("select an area\n");
  while (1)
  {
    int result = XGrabPointer(xdisplay, xroot, 1, PointerMotionMask | ButtonPressMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);

    while (result == 1)
    {
      XUngrabPointer(xdisplay, CurrentTime);
      result = XGrabPointer(xdisplay, xroot, 1, PointerMotionMask | ButtonPressMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
    }

    XNextEvent(xdisplay, &xevent);

    if (xevent.type == ButtonPress)
    {
      if (!isPressed)
      {
        init_x = xevent.xmotion.x;
        init_y = xevent.xmotion.y;

        printf("drawing area: init_x = %d, init_y = %d\n", init_x, init_y);

        isPressed = true;
      }
    }
    else if (xevent.type == ButtonRelease)
    {
      if (!isShifted)
      {
        fin_x = xevent.xmotion.x;
        fin_y = xevent.xmotion.y;
      }

      XFlush(xdisplay);
      XCloseDisplay(xdisplay);

      printf("drawing limit: fin_x = %d, fin_y = %d\n", fin_x, fin_y);

      break;
    }

    if (xevent.type == MotionNotify && isPressed == true)
    {
      if (currentTime > delay + 3)
      {
        isShifted = true;

        int tmp_x = xevent.xmotion.x;
        int tmp_y = xevent.xmotion.y;

        cairo_set_line_width(cr, 6);
        cairo_set_source_surface(cr, surfaceTmp, 0, 0);
        cairo_rectangle(cr, prevInitX, prevInitY, prevFinX, prevFinY);
        cairo_stroke(cr);

        cairo_set_line_width(cr, 5);
        cairo_set_source_rgb(cr, 0.5, 0.7, 0);
        cairo_rectangle(cr, init_x, init_y, tmp_x - init_x, tmp_y - init_y);
        cairo_stroke(cr);

        prevInitX = init_x;
        prevInitY = init_y;
        prevFinX = tmp_x - init_x;
        prevFinY = tmp_y - init_y;

        fin_x = xevent.xmotion.x;
        fin_y = xevent.xmotion.y;

        delay = currentTime;
      }
    }

    currentTime++;
  }

  *x = init_x + 3;
  *y = init_y + 3;
  *width = fin_x - init_x - 6;
  *height = fin_y - init_y - 6;

  if (*width < 0 && *height < 0)
  {
    *x = fin_x + 3;
    *y = fin_y + 3;
    *width = init_x - fin_x - 6;
    *height = init_y - fin_y - 6;
    printf("inverted, both y and x were decrementing\n");
  }
  else if (*width < 0)
  {
    *x = fin_x + 3;
    *width = init_x - fin_x - 6;
    printf("inverted only x was decrementing\n");
  }
  else if (*height < 0)
  {
    *y = fin_y + 3;
    *height = init_y - fin_y - 6;
    printf("inverted only y was decrementing\n");
  }
  else
  {
    printf("uninverted\n");
  }

  printf("height, %d and width, %d\n", *height, *width);
}

bool OcctGtkViewer::OnScreenshotBtnClick(GdkEventButton *theEvent)
{
  if (isScreenshotBtnActivated)
  {
    motionNotifyConn.disconnect();
    screenshotBtn->set_sensitive(false);
    isScreenshotBtnActivated = false;

    MakeScreenshot();
    Glib::signal_timeout().connect([this]()
                                   { isScreenshotBtnActivated = true;
                                     motionNotifyConn = myGLArea->signal_motion_notify_event().connect(sigc::mem_fun(*this, &OcctGtkViewer::onMouseMotion), false);
                                     return false; },
                                   100);
  }

  return true;
}

bool OcctGtkViewer::MakeScreenshot()
{
  int x, y, width, height;

  SelectScreenArea(&x, &y, &width, &height);

  GdkPixbuf *screen;
  Display *disp;
  ::Window root;
  cairo_surface_t *surface;
  int scr;

  disp = XOpenDisplay(NULL);
  scr = DefaultScreen(disp);
  root = gdk_x11_window_get_xid(gtk_widget_get_window((GtkWidget *)myGLArea->gobj()));

  if (width > 0 && height > 0)
  {
    surface = cairo_xlib_surface_create(disp, root, DefaultVisual(disp, scr), DisplayWidth(disp, scr), DisplayHeight(disp, scr));

    screen = gdk_pixbuf_get_from_surface(surface, x, y, width, height);
    gdk_pixbuf_save(screen, "/tmp/file.png", "png", NULL, "quality", "100", NULL);

    system("./paint/paint");
  }

  myGLArea->hide();
  myGLArea->show();

  screenshotBtn->set_sensitive(true);

  return false;
}

void OcctGtkViewer::OnSelectChanged()
{
  Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();

  // Если строка выбрана
  if (iter)
  {
    Gtk::TreeModel::Row row = *iter;

    // ищем значение колонки по заданному столбцу
    int labelId = row.get_value(m_Columns.m_col_id);

    TDF_Label foundLabel;
    FindModelPartLabelById(labelId, foundLabel);
    SelectModelPart(foundLabel);

    UpdateView();
  }
}

// конфигурируем контейнер, содержащий дерево модели
void OcctGtkViewer::ConstructTree()
{
  treeStore = Gtk::TreeStore::create(m_Columns);

  treeView->set_model(treeStore);

  treeView->append_column("Ид.", m_Columns.m_col_id);
  treeView->append_column("Имя", m_Columns.m_col_name);
}

void OcctGtkViewer::ExplodeModel(double distance, ExplodeDirection explodeDirection)
{
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;

  for (int i = 0; i < aisObjects.size(); i++)
  {
    switch (explodeDirection)
    {
    case UP:
      z += distance;
      break;

    case FORWARD:
      y += distance;
      break;

    case RIGHT:
      x += distance;
      break;

    default:
      break;
    }

    gp_Vec translationVector(x, y, z);
    gp_Trsf translation;
    translation.SetTranslation(translationVector);

    translation *= aisObjectsParentTransformations[i];
    aisObjects[i]->SetLocalTransformation(translation);

    myContext->Update(aisObjects[i], 0);
  }
}

void OcctGtkViewer::OnScaleBarValueChanged(Gtk::Scale *scale)
{
  myView->Invalidate();
  ExplodeModel(scale->get_value(), ExplodeDirection::FORWARD);
}

// ================================================================
// Function : OcctGtkViewer
// Purpose  :
// ================================================================
OcctGtkViewer::OcctGtkViewer(string pathToFile)
{
  this->pathToFile = pathToFile;

  // Формируем документ с труктурой модели - для визуализации модели и доступа к данным фигуры
  occtApp = new TDocStd_Application();
  InitializeOcctDoc(pathToFile);

  Handle(Aspect_DisplayConnection) aDisp = new Aspect_DisplayConnection();
  Handle(OpenGl_GraphicDriver) aDriver = new OpenGl_GraphicDriver(aDisp, false);
  // lets Gtk::GLArea to manage buffer swap
  aDriver->ChangeOptions().buffersNoSwap = true;
  // don't write into alpha channel
  aDriver->ChangeOptions().buffersOpaqueAlpha = true;
  // offscreen FBOs should be always used
  aDriver->ChangeOptions().useSystemBuffer = false;

  // create viewer
  myViewer = new V3d_Viewer(aDriver);
  myViewer->SetDefaultBackgroundColor(Quantity_NOC_WHITE);
  myViewer->SetDefaultLights();
  myViewer->SetLightOn();

  // create AIS context
  myContext = new AIS_InteractiveContext(myViewer);
  myContext->HighlightStyle(Prs3d_TypeOfHighlight_Dynamic)->SetColor(Quantity_NOC_RED);
  myContext->HighlightStyle(Prs3d_TypeOfHighlight_Selected)->SetColor(Quantity_NOC_BLUE);

  // note - window will be created later within onGlAreaRealized() callback!
  myView = myViewer->CreateView();
  myView->SetImmediateUpdate(false);
  myView->ChangeRenderingParams().ToShowStats = true;
  myView->ChangeRenderingParams().CollectedStats = (Graphic3d_RenderingParams::PerfCounters)(Graphic3d_RenderingParams::PerfCounters_FrameRate | Graphic3d_RenderingParams::PerfCounters_Triangles);

  viewerInteractor = new ViewerInteractor(myView, myContext);

  // widgets
  ProcessWidgets();
  ConstructTree();
  CSSConnection();

  add(*viewportScreen);

  ClearDataAboutDisplayedModel();
}

void OcctGtkViewer::ClearDataAboutDisplayedModel()
{
  labels.clear();
  aisObjects.clear();
  aisObjectsParentTransformations.clear();
}

void OcctGtkViewer::InitializeOcctDoc(string pathToFile)
{
  occtApp->NewDocument("BinXCAF", occtDoc);
  BinXCAFDrivers::DefineFormat(occtApp);

  if (!global::downloadStepFile)
  {
    occtApp->Open(pathToFile.c_str(), occtDoc);
    occtApp->Close(occtDoc);
  }
  else
  {
    if (pathToFile == "")
      pathToFile = global::testModelFolder;

    STEPCAFControl_Reader aStepReader;
    aStepReader.Perform(pathToFile.c_str(), occtDoc);

    // occtApp->SaveAs(occtDoc, "./ 1.xbf");
    // Проверить, нужен ли occtApp->Close(occtDoc); в этом блоке
  }
}

void OcctGtkViewer::RestoreUI()
{
  fileName->set_text(global::currentFileName);

  treeView->columns_autosize();
  treeStore->clear();
  scale->set_value(0);

  isExploadBarVisible = false;
}

void OcctGtkViewer::UpdateView()
{
  onGlAreaRealized();
  myGLArea->queue_draw();
}

void OcctGtkViewer::HideWidgets()
{
  scale->hide();
  controlsBox->hide();
  treeBox->hide();
}

void OcctGtkViewer::OpenWindow(string pathToFile)
{
  this->pathToFile = pathToFile;

  InitializeOcctDoc(pathToFile);
  ClearDataAboutDisplayedModel();
  RestoreUI();
  UpdateView();
}

void OcctGtkViewer::CenterModel()
{
  viewerInteractor->ProcessKeyPress(Aspect_VKey_B);
  viewerInteractor->ProcessKeyPress(Aspect_VKey_F);
}

bool OcctGtkViewer::GoBack(GdkEventButton *theEvent)
{
  ShowWindow(mainWindow);
  mainWindow->GetWindowStack()->set_visible_child("InFolderScreen");

  mainWindow->TurnOnSearchMode();

  myContext->RemoveAll(false);

  Glib::signal_timeout().connect(sigc::mem_fun(*this, &OcctGtkViewer::HideWindow), 300);

  return true;
}

void OcctGtkViewer::CSSConnection()
{
  GtkCssProvider *cssProvider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(cssProvider, global::viewportWindowCSS.c_str(), NULL);
  gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                            GTK_STYLE_PROVIDER(cssProvider),
                                            GTK_STYLE_PROVIDER_PRIORITY_USER);
}

bool OcctGtkViewer::OnViewPerspective(GdkEventButton *theEvent)
{
  viewerInteractor->ProcessKeyPress(Aspect_VKey_B);
  UpdateView();

  return true;
}
bool OcctGtkViewer::OnViewRight(GdkEventButton *theEvent)
{
  viewerInteractor->ProcessKeyPress(Aspect_VKey_R);
  UpdateView();

  return true;
}
bool OcctGtkViewer::OnViewUp(GdkEventButton *theEvent)
{
  viewerInteractor->ProcessKeyPress(Aspect_VKey_T);
  UpdateView();

  return true;
}
bool OcctGtkViewer::OnViewForward(GdkEventButton *theEvent)
{
  viewerInteractor->ProcessKeyPress(Aspect_VKey_L);
  UpdateView();

  return true;
}

bool OcctGtkViewer::Center(GdkEventButton *theEvent)
{
  viewerInteractor->ProcessKeyPress(Aspect_VKey_F);
  UpdateView();

  return true;
}

// ================================================================
// Function : dumpGlInfo
// Purpose  :
// ================================================================
void OcctGtkViewer::dumpGlInfo(bool theIsBasic)
{
  TColStd_IndexedDataMapOfStringString aGlCapsDict;
  myView->DiagnosticInformation(aGlCapsDict, theIsBasic ? Graphic3d_DiagnosticInfo_Basic : Graphic3d_DiagnosticInfo_Complete);
  if (theIsBasic)
  {
    TCollection_AsciiString aViewport;
    aGlCapsDict.FindFromKey("Viewport", aViewport);
    aGlCapsDict.Clear();
    aGlCapsDict.Add("Viewport", aViewport);
  }
  aGlCapsDict.Add("Display scale", TCollection_AsciiString(myDevicePixelRatio));

  // beautify output
  {
    TCollection_AsciiString *aGlVer = aGlCapsDict.ChangeSeek("GLversion");
    TCollection_AsciiString *aGlslVer = aGlCapsDict.ChangeSeek("GLSLversion");
    if (aGlVer != NULL && aGlslVer != NULL)
    {
      *aGlVer = *aGlVer + " [GLSL: " + *aGlslVer + "]";
      aGlslVer->Clear();
    }
  }

  TCollection_AsciiString anInfo;
  for (TColStd_IndexedDataMapOfStringString::Iterator aValueIter(aGlCapsDict); aValueIter.More(); aValueIter.Next())
  {
    if (!aValueIter.Value().IsEmpty())
    {
      if (!anInfo.IsEmpty())
      {
        anInfo += "\n";
      }
      anInfo += aValueIter.Key() + ": " + aValueIter.Value();
    }
  }

  Message::SendWarning(anInfo);
}

// ================================================================
// Function : onValueChanged
// Purpose  :
// ================================================================
void OcctGtkViewer::onValueChanged(const Glib::RefPtr<Gtk::Adjustment> &theAdj)
{
  float aVal = theAdj->get_value() / 360.0f;
  myView->SetBackgroundColor(Quantity_Color(aVal, aVal, aVal, Quantity_TOC_sRGB));
  myView->Invalidate();
  myGLArea->queue_draw();
}

//! Convert Gtk event mouse button to Aspect_VKeyMouse.
static Aspect_VKeyMouse mouseButtonFromGtk(guint theButton)
{
  switch (theButton)
  {
  case 1:
    return Aspect_VKeyMouse_LeftButton;
  case 2:
    return Aspect_VKeyMouse_MiddleButton;
  case 3:
    return Aspect_VKeyMouse_RightButton;
  }
  return Aspect_VKeyMouse_NONE;
}

//! Convert Gtk event mouse flags to Aspect_VKeyFlags.
static Aspect_VKeyFlags mouseFlagsFromGtk(guint theFlags)
{
  Aspect_VKeyFlags aFlags = Aspect_VKeyFlags_NONE;
  if ((theFlags & GDK_SHIFT_MASK) != 0)
  {
    aFlags |= Aspect_VKeyFlags_SHIFT;
  }
  if ((theFlags & GDK_CONTROL_MASK) != 0)
  {
    aFlags |= Aspect_VKeyFlags_CTRL;
  }
  if ((theFlags & GDK_META_MASK) != 0)
  {
    aFlags |= Aspect_VKeyFlags_META;
  }
  if ((theFlags & GDK_MOD1_MASK) != 0)
  {
    aFlags |= Aspect_VKeyFlags_ALT;
  }
  return aFlags;
}

// ================================================================
// Function : onMouseMotion
// Purpose  :
// ================================================================
bool OcctGtkViewer::onMouseMotion(GdkEventMotion *theEvent)
{
  myGLArea->hide();
  myGLArea->show();

  const bool isEmulated = false;
  const Aspect_VKeyMouse aButtons = PressedMouseButtons();
  const Graphic3d_Vec2d aNewPos2d = myView->Window()->ConvertPointToBacking(Graphic3d_Vec2d(theEvent->x, theEvent->y));
  const Graphic3d_Vec2i aNewPos2i = Graphic3d_Vec2i(aNewPos2d + Graphic3d_Vec2d(0.5));
  const Aspect_VKeyFlags aFlags = mouseFlagsFromGtk(theEvent->state);

  if (UpdateMousePosition(aNewPos2i, aButtons, aFlags, isEmulated))
  {
    myGLArea->queue_draw();
  }

  return false;
}

bool OcctGtkViewer::onMouseLeaveEvent(GdkEventCrossing *theEvent)
{
  const bool isEmulated = false;

  const Graphic3d_Vec2i aNewPos2i = Graphic3d_Vec2i(0, 0);
  const Aspect_VKeyFlags aFlags = mouseFlagsFromGtk(theEvent->state);

  if (ReleaseMouseButton(aNewPos2i, Aspect_VKeyMouse_MiddleButton, aFlags, isEmulated) ||
      ReleaseMouseButton(aNewPos2i, Aspect_VKeyMouse_LeftButton, aFlags, isEmulated) ||
      ReleaseMouseButton(aNewPos2i, Aspect_VKeyMouse_RightButton, aFlags, isEmulated))
  {
    myGLArea->queue_draw();
  }

  return true;
}

// ================================================================
// Function : onMouseButtonPressed
// Purpose  :
// ================================================================
bool OcctGtkViewer::onMouseButtonPressed(GdkEventButton *theEvent)
{
  const bool isEmulated = false;
  const Aspect_VKeyMouse aButton = mouseButtonFromGtk(theEvent->button);
  const Graphic3d_Vec2d aNewPos2d = myView->Window()->ConvertPointToBacking(Graphic3d_Vec2d(theEvent->x, theEvent->y));
  const Graphic3d_Vec2i aNewPos2i = Graphic3d_Vec2i(aNewPos2d + Graphic3d_Vec2d(0.5));
  const Aspect_VKeyFlags aFlags = mouseFlagsFromGtk(theEvent->state);

  if (aButton == Aspect_VKeyMouse_NONE)
  {
    return false;
  }
  else if (aButton == Aspect_VKeyMouse_RightButton)
  {
    if (PressMouseButton(aNewPos2i, Aspect_VKeyMouse_MiddleButton, aFlags, isEmulated))
    {
      myGLArea->queue_draw();
    }
  }
  else
  {
    if (PressMouseButton(aNewPos2i, aButton, aFlags, isEmulated))
    {
      myGLArea->queue_draw();
    }
  }

  return true;
}

// ================================================================
// Function : onMouseButtonReleased
// Purpose  :
// ================================================================
bool OcctGtkViewer::onMouseButtonReleased(GdkEventButton *theEvent)
{
  const bool isEmulated = false;
  const Aspect_VKeyMouse aButton = mouseButtonFromGtk(theEvent->button);
  if (aButton == Aspect_VKeyMouse_NONE)
  {
    return false;
  }
  const Graphic3d_Vec2d aNewPos2d = myView->Window()->ConvertPointToBacking(Graphic3d_Vec2d(theEvent->x, theEvent->y));
  const Graphic3d_Vec2i aNewPos2i = Graphic3d_Vec2i(aNewPos2d + Graphic3d_Vec2d(0.5));
  const Aspect_VKeyFlags aFlags = mouseFlagsFromGtk(theEvent->state);

  if (aButton == Aspect_VKeyMouse_RightButton)
  {
    if (ReleaseMouseButton(aNewPos2i, Aspect_VKeyMouse_MiddleButton, aFlags, isEmulated))
    {
      myGLArea->queue_draw();
    }
  }
  else
  {
    if (ReleaseMouseButton(aNewPos2i, aButton, aFlags, isEmulated))
    {
      myGLArea->queue_draw();
    }
  }
  return true;
}

// ================================================================
// Function : onMouseScroll
// Purpose  :
// ================================================================
bool OcctGtkViewer::onMouseScroll(GdkEventScroll *theEvent)
{
  int x, y;

  gp_Pnt gp = myContext->GravityPoint(myView);
  myView->Convert(gp.X(), gp.Y(), gp.Z(), x, y);

  if (theEvent->direction == 0)
  {
    const Graphic3d_Vec2d aNewPos2d = myView->Window()->ConvertPointToBacking(Graphic3d_Vec2d(x, y));
    const Aspect_ScrollDelta aScroll(Graphic3d_Vec2i(aNewPos2d), 1);

    if (UpdateMouseScroll(aScroll))
    {
      myGLArea->queue_draw();
    }
  }
  else if (theEvent->direction == 1)
  {
    const Graphic3d_Vec2d aNewPos2d = myView->Window()->ConvertPointToBacking(Graphic3d_Vec2d(x, y));
    const Aspect_ScrollDelta aScroll(Graphic3d_Vec2i(aNewPos2d), -1);

    if (UpdateMouseScroll(aScroll))
    {
      myGLArea->queue_draw();
    }
  }

  return true;
}

// ================================================================
// Function : initPixelScaleRatio
// Purpose  :
// ================================================================
void OcctGtkViewer::initPixelScaleRatio()
{
  SetTouchToleranceScale(myDevicePixelRatio);
  myView->ChangeRenderingParams().Resolution = (unsigned int)(96.0 * myDevicePixelRatio + 0.5);
  myContext->SetPixelTolerance(int(myDevicePixelRatio * 6.0));
}

// ================================================================
// Function : onGlAreaRealized
// Purpose  :
// ================================================================
void OcctGtkViewer::onGlAreaRealized()
{
  myGLArea->make_current();

  Graphic3d_Vec2i aLogicalSize(myGLArea->get_width(), myGLArea->get_height());

  EGLContext anEglCtx = eglGetCurrentContext();
#ifdef HAVE_GLES2
  if (anEglCtx == EGL_NO_CONTEXT)
  {
    Message::SendFail() << "Error: EGL context is not found";
    Gtk::MessageDialog aMsg("Error: EGL context is not found", false, Gtk::MESSAGE_ERROR);
    aMsg.run();
    return;
  }
#else
  if (anEglCtx != EGL_NO_CONTEXT)
  {
    Message::SendFail() << "Error: Wayland session (EGL context) is not unsupported";
    Gtk::MessageDialog aMsg("Error: Wayland session (EGL context) is not unsupported", false, Gtk::MESSAGE_ERROR);
    aMsg.run();
    return;
  }
#endif

  Handle(OpenGl_Context) aGlCtx = new OpenGl_Context();
  if (!aGlCtx->Init())
  {
    Message::SendFail() << "Error: OpenGl_Context is unable to wrap OpenGL context";
    Gtk::MessageDialog aMsg("Error: OpenGl_Context is unable to wrap OpenGL context", false, Gtk::MESSAGE_ERROR);
    aMsg.run();
    return;
  }

  try
  {
    OCC_CATCH_SIGNALS
    myGLArea->throw_if_error();

    const Graphic3d_Vec2i aViewSize = aLogicalSize * myGLArea->get_scale_factor();

    OcctGtkWindow *aWindow = new OcctGtkWindow();
    const bool isFirstInit = true;
    if (isFirstInit)
    {
      if (anEglCtx != EGL_NO_CONTEXT)
      {
        // wrap EGL surface
        EGLContext anEglDisplay = eglGetCurrentDisplay();
        EGLContext anEglSurf = eglGetCurrentSurface(EGL_DRAW);
        EGLint anEglCfgId = 0, aNbConfigs = 0;
        eglQuerySurface(anEglDisplay, anEglSurf, EGL_CONFIG_ID, &anEglCfgId);
        const EGLint aConfigAttribs[] = {EGL_CONFIG_ID, anEglCfgId, EGL_NONE};
        void *anEglCfg = NULL;
        eglChooseConfig(anEglDisplay, aConfigAttribs, &anEglCfg, 1, &aNbConfigs);

        Handle(OpenGl_GraphicDriver) aDriver = Handle(OpenGl_GraphicDriver)::DownCast(myContext->CurrentViewer()->Driver());
        if (!aDriver->InitEglContext(anEglDisplay, anEglCtx, anEglCfg))
        {
          Message::SendFail() << "Error: OpenGl_GraphicDriver cannot initialize EGL context";
          Gtk::MessageDialog aMsg("Error: OpenGl_GraphicDriver cannot initialize EGL context", false, Gtk::MESSAGE_ERROR);
          aMsg.run();
          return;
        }
      }
      else
      {
        ::Window anXWin = gdk_x11_window_get_xid(gtk_widget_get_window((GtkWidget *)myGLArea->gobj()));
        aWindow->SetNativeHandle(anXWin);
      }
    }

    const float aPixelRatio = float(aViewSize.y()) / float(aLogicalSize.y());
    aWindow->SetSize(aViewSize.x(), aViewSize.y());
    aWindow->SetDevicePixelRatio(aPixelRatio);
    myDevicePixelRatio = aPixelRatio;
    initPixelScaleRatio();

    myView->SetWindow(aWindow, aGlCtx->RenderingContext());
  }
  catch (const Gdk::GLError &theGlErr)
  {
    Message::SendFail() << "An error occurred making the context current during realize:\n"
                        << theGlErr.domain() << "-" << theGlErr.code() << "-" << theGlErr.what();
  }
  catch (const Standard_Failure &theErr)
  {
    Message::SendFail() << "An error occurred making the context current during realize:\n"
                        << theErr;
  }
}

// ================================================================
// Function : onGlAreaReleased
// Purpose  :
// ================================================================
void OcctGtkViewer::onGlAreaReleased()
{
  myGLArea->make_current();

  try
  {
    myGLArea->throw_if_error();

    // release OCCT viewer on application close
    Handle(Aspect_DisplayConnection) aDisp;
    if (!myContext.IsNull())
    {
      aDisp = myViewer->Driver()->GetDisplayConnection();

      myContext->RemoveAll(false);
      myContext.Nullify();

      myView->Remove();
      myView.Nullify();

      myViewer.Nullify();
    }

    myGLArea->make_current();
    aDisp.Nullify();
  }
  catch (const Gdk::GLError &theGlErr)
  {
    Message::SendFail() << "An error occurred making the context current during unrealize\n"
                        << theGlErr.domain() << "-" << theGlErr.code() << "-" << theGlErr.what() << "\n";
  }
}

int WIDTH = 1610;
int XPOSITION = 305;

// ================================================================
// Function : onGlAreaRender
// Purpose  :
// ================================================================
bool OcctGtkViewer::onGlAreaRender(const Glib::RefPtr<Gdk::GLContext> &theGlCtx)
{
  myGLArea->hide();
  myGLArea->show();

  (void)theGlCtx;
  if (myView->Window().IsNull())
  {
    return false;
  }

  myGLArea->throw_if_error();

  // wrap FBO created by Gtk::GLArea
  Handle(OpenGl_GraphicDriver) aDriver = Handle(OpenGl_GraphicDriver)::DownCast(myContext->CurrentViewer()->Driver());
  const Handle(OpenGl_Context) &aGlCtx = aDriver->GetSharedContext();
  Handle(OpenGl_FrameBuffer) aDefaultFbo = aGlCtx->DefaultFrameBuffer();

  if (aDefaultFbo.IsNull())
  {
    aDefaultFbo = new OcctQtFrameBuffer();
    aGlCtx->SetDefaultFrameBuffer(aDefaultFbo);
  }

  if (!aDefaultFbo->InitWrapper(aGlCtx))
  {
    aDefaultFbo.Nullify();
    Message::DefaultMessenger()->Send("Default FBO wrapper creation failed\n", Message_Fail);
    return false;
  }

  Graphic3d_Vec2i aViewSizeOld;
  const Graphic3d_Vec2i aLogicalSize(myGLArea->get_width(), myGLArea->get_height());
  const Graphic3d_Vec2i aViewSizeNew = aDefaultFbo->GetVPSize();
  const float aPixelRatio = float(aViewSizeNew.y()) / float(aLogicalSize.y());
  Handle(OcctGtkWindow) aWindow = Handle(OcctGtkWindow)::DownCast(myView->Window());
  aWindow->Size(aViewSizeOld.x(), aViewSizeOld.y());

  if (aViewSizeNew != aViewSizeOld)
  {
    aWindow->SetSize(aViewSizeNew.x(), aViewSizeNew.y());
    myView->MustBeResized();
    myView->Invalidate();
  }

  if (myDevicePixelRatio != aPixelRatio)
  {
    myDevicePixelRatio = aPixelRatio;
    aWindow->SetDevicePixelRatio(aPixelRatio);
    initPixelScaleRatio();
  }

  // flush pending input events and redraw the viewer
  myView->InvalidateImmediate();
  FlushViewEvents(myContext, myView, true);

  return true;
}

// ================================================================
// Function : handleViewRedraw
// Purpose  :
// ================================================================
void OcctGtkViewer::handleViewRedraw(const Handle(AIS_InteractiveContext) & theCtx,
                                     const Handle(V3d_View) & theView)
{
  AIS_ViewController::handleViewRedraw(theCtx, theView);
  if (myToAskNextFrame)
  {
    // ask more frames
    myGLArea->queue_draw();
  }
}
