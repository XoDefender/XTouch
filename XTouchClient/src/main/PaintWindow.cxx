#include "global.hxx"

#include "OcctGtkViewer.hxx"
#include "PaintWindow.hxx"

#include <string>
#include <fstream>
#include <filesystem>

void PaintWindow::CalculateLastFileIndex()
{
    savedFileIndex = 0;

    std::vector<std::string> fileNames = {};
    for (const auto &entry : std::filesystem::directory_iterator(global::saveImagePath))
        fileNames.push_back(entry.path());

    for (std::string fileName : fileNames)
    {
        int currentIndex = stoi(fileName.substr(fileName.find("_") + 1));
        if (currentIndex > savedFileIndex)
        {
            savedFileIndex = currentIndex;
        }
    }
}

void PaintWindow::SetPassiveToolTypeBtnStyle(Gtk::EventBox *previousBtn,
                                             std::string previousBtnIcon,
                                             std::string previousBtnClass)
{
    Glib::RefPtr<Gtk::CssProvider> cssProviderPrev = Gtk::CssProvider::create();
    std::string previousButtonStyle = previousBtnClass + std::string("{background: url('../../../XTouchClient/res/Windows/PaintWindow/img/") + previousBtnIcon + std::string("') no-repeat center;\
                                                background-color:#e5ecff;\
                                                background-size: 35px;}");

    cssProviderPrev->load_from_data(previousButtonStyle);
    previousBtn->get_style_context()
        ->add_provider(
            cssProviderPrev, GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void PaintWindow::SetActiveToolTypeBtnStyle(Gtk::EventBox *currentBtn,
                                            std::string currentBtnIcon,
                                            std::string currentBtnClass)
{
    Glib::RefPtr<Gtk::CssProvider> cssProviderCurr = Gtk::CssProvider::create();
    std::string currentButtonStyle = currentBtnClass + std::string("{background: url('../../../XTouchClient/res/Windows/PaintWindow/img/") + currentBtnIcon + std::string("') no-repeat center;\
                                                background-color:#e5ecff;\
                                                background-size: 35px;}");

    cssProviderCurr->load_from_data(currentButtonStyle);
    currentBtn->get_style_context()
        ->add_provider(
            cssProviderCurr, GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void PaintWindow::SetPassiveToolBtnStyle(Gtk::EventBox *previousBtn,
                                         std::string previousBtnIcon,
                                         std::string previousBtnClass)
{
    Glib::RefPtr<Gtk::CssProvider> cssProviderPrev = Gtk::CssProvider::create();
    std::string previousButtonStyle = previousBtnClass + std::string("{background: url('../../../XTouchClient/res/Windows/PaintWindow/img/") + previousBtnIcon + std::string("') no-repeat center;\
                                                border-radius: 10px;\
                                                border: 2px solid rgb(219, 219, 219);\
                                                background-size: 35px;}");

    cssProviderPrev->load_from_data(previousButtonStyle);
    previousBtn->get_style_context()
        ->add_provider(
            cssProviderPrev, GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void PaintWindow::SetActiveToolBtnStyle(Gtk::EventBox *currentBtn,
                                        std::string currentBtnIcon,
                                        std::string currentBtnClass)
{
    Glib::RefPtr<Gtk::CssProvider> cssProviderCurr = Gtk::CssProvider::create();
    std::string currentButtonStyle = currentBtnClass + std::string("{background: url('../../../XTouchClient/res/Windows/PaintWindow/img/") + currentBtnIcon + std::string("') no-repeat center;\
                                                border-radius: 10px;\
                                                border: 2px solid #4B6EEE;\
                                                background-size: 35px;}");

    cssProviderCurr->load_from_data(currentButtonStyle);
    currentBtn->get_style_context()
        ->add_provider(
            cssProviderCurr, GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void PaintWindow::ProcessWidgets()
{
    uiBuilder = Gtk::Builder::create_from_file(global::paintWindowUI);

    uiBuilder->get_widget<Gtk::EventBox>("BackBtn", goBackBtn);
    uiBuilder->get_widget<Gtk::Fixed>("ScreenshotScreen", container);
    uiBuilder->get_widget<Gtk::Fixed>("DrawContainer", drawContainer);

    uiBuilder->get_widget<Gtk::EventBox>("ColorBtnRed", colorBtn1);
    uiBuilder->get_widget<Gtk::EventBox>("ColorBtnGreen", colorBtn2);
    uiBuilder->get_widget<Gtk::EventBox>("ColorBtnBlue", colorBtn3);
    uiBuilder->get_widget<Gtk::EventBox>("ColorBtnWhite", colorBtn4);
    uiBuilder->get_widget<Gtk::EventBox>("ColorBtnBlack", colorBtn5);

    uiBuilder->get_widget<Gtk::EventBox>("SizeBtnThin", sizeBtn1);
    uiBuilder->get_widget<Gtk::EventBox>("SizeBtnMedium", sizeBtn2);
    uiBuilder->get_widget<Gtk::EventBox>("SizeBtnBold", sizeBtn3);
    uiBuilder->get_widget<Gtk::EventBox>("SizeBtnBoldPlus", sizeBtn4);

    uiBuilder->get_widget<Gtk::EventBox>("EraseBtn", eraseBtn);

    uiBuilder->get_widget<Gtk::EventBox>("DrawRectangleBtn", drawRectangleBtn);
    uiBuilder->get_widget<Gtk::EventBox>("DrawCircleBtn", drawCircleBtn);
    uiBuilder->get_widget<Gtk::EventBox>("DrawLineBtn", drawLineBtn);
    uiBuilder->get_widget<Gtk::EventBox>("DrawPencilBtn", drawPencilBtn);

    uiBuilder->get_widget<Gtk::EventBox>("ShowSizes", showSizes);
    uiBuilder->get_widget<Gtk::EventBox>("ShowShapes", showShapes);

    uiBuilder->get_widget<Gtk::Stack>("ToolsStack", toolsStack);
    uiBuilder->get_widget<Gtk::Box>("Sizes", sizes);
    uiBuilder->get_widget<Gtk::Box>("Shapes", shapes);

    uiBuilder->get_widget<Gtk::Button>("SelectColorBtn", selectColorBtn);
    uiBuilder->get_widget<Gtk::ColorChooserDialog>("ColorChooser", colorChooser);

    uiBuilder->get_widget<Gtk::EventBox>("SaveNew", saveNewBtn);

    goBackBtn->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                   { hide(); 
                                                    ShowWindow(aGtkWin);
                                                    aGtkWin->HideWidgets();
                                                    return false; });

    showSizes->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                   { toolsStack->set_visible_child("Sizes");

                                                        SetActiveToolTypeBtnStyle(showSizes, "brushA.png", ".ShowSizes");
                                                        SetPassiveToolTypeBtnStyle(showShapes, "shapes.png", ".ShowShapes");

                                                        return false; });

    showShapes->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                    { toolsStack->set_visible_child("Shapes");

                                                        SetActiveToolTypeBtnStyle(showShapes, "shapesA.png", ".ShowShapes");
                                                        SetPassiveToolTypeBtnStyle(showSizes, "brush.png", ".ShowSizes");

                                                        return false; });

    drawRectangleBtn->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                          { m_drawArea.SetCurrentShape(DrawHelper::Shape::Rectangle);

                                                              SetActiveToolBtnStyle(drawRectangleBtn, "squareA.png", ".Rectangle");
                                                              SetPassiveToolBtnStyle(drawCircleBtn, "circle.png", ".Circle");
                                                              SetPassiveToolBtnStyle(drawLineBtn, "line.png", ".Line");
                                                              SetPassiveToolBtnStyle(drawPencilBtn, "pencil.png", ".Pencil");

                                                              return false; });

    drawCircleBtn->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                       { m_drawArea.SetCurrentShape(DrawHelper::Shape::Circle); 

                                                            SetPassiveToolBtnStyle(drawRectangleBtn, "square.png", ".Rectangle");
                                                            SetActiveToolBtnStyle(drawCircleBtn, "circleA.png", ".Circle");
                                                            SetPassiveToolBtnStyle(drawLineBtn, "line.png", ".Line");
                                                            SetPassiveToolBtnStyle(drawPencilBtn, "pencil.png", ".Pencil");

                                                            return false; });

    drawLineBtn->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                     { m_drawArea.SetCurrentShape(DrawHelper::Shape::Line);

                                                            SetPassiveToolBtnStyle(drawRectangleBtn, "square.png", ".Rectangle");
                                                            SetPassiveToolBtnStyle(drawCircleBtn, "circle.png", ".Circle");
                                                            SetActiveToolBtnStyle(drawLineBtn, "lineA.png", ".Line");
                                                            SetPassiveToolBtnStyle(drawPencilBtn, "pencil.png", ".Pencil");

                                                            return false; });

    drawPencilBtn->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                       { m_drawArea.SetCurrentShape(DrawHelper::Shape::Pencil);

                                                            SetPassiveToolBtnStyle(drawRectangleBtn, "square.png", ".Rectangle");
                                                            SetPassiveToolBtnStyle(drawCircleBtn, "circle.png", ".Circle");
                                                            SetPassiveToolBtnStyle(drawLineBtn, "line.png", ".Line");
                                                            SetActiveToolBtnStyle(drawPencilBtn, "pencilA.png", ".Pencil");

                                                            return false; });

    colorBtn1->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                   {m_drawArea.drawParams.color.set_rgba(1, 0, 0); return false; });

    colorBtn2->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                   {m_drawArea.drawParams.color.set_rgba(0, 1, 0); return false; });

    colorBtn3->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                   {m_drawArea.drawParams.color.set_rgba(0, 0, 1); return false; });

    colorBtn4->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                   {m_drawArea.drawParams.color.set_rgba(1, 1, 1); return false; });

    colorBtn5->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                   {m_drawArea.drawParams.color.set_rgba(0, 0, 0); return false; });

    selectColorBtn->signal_clicked().connect([this]()
                                             {colorChooser->set_transient_for(*this);
                                              const int result = colorChooser->run();
  switch(result)
  {
    case Gtk::RESPONSE_OK:
    {
      m_drawArea.drawParams.color = colorChooser->get_rgba();
      colorChooser->hide();
      break;
    }
    case Gtk::RESPONSE_CANCEL:
    {
      colorChooser->hide();
      break;
    }
    default:
    {
      break;
    }
  } });

    sizeBtn1->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                  { m_drawArea.drawParams.lineWidth = 5.0;

                                                        SetActiveToolBtnStyle(sizeBtn1, "thinA.png", ".SizeThin");
                                                        SetPassiveToolBtnStyle(sizeBtn2, "regular.png", ".SizeMedium");
                                                        SetPassiveToolBtnStyle(sizeBtn3, "bold.png", ".SizeBold");
                                                        SetPassiveToolBtnStyle(sizeBtn4, "large.png", ".SizeLarge");

                                                        return false; });

    sizeBtn2->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                  { m_drawArea.drawParams.lineWidth = 8.0;

                                                        SetPassiveToolBtnStyle(sizeBtn1, "thin.png", ".SizeThin");
                                                        SetActiveToolBtnStyle(sizeBtn2, "regularA.png", ".SizeMedium");
                                                        SetPassiveToolBtnStyle(sizeBtn3, "bold.png", ".SizeBold");
                                                        SetPassiveToolBtnStyle(sizeBtn4, "large.png", ".SizeLarge");

                                                        return false; });

    sizeBtn3->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                  { m_drawArea.drawParams.lineWidth = 10.0;

                                                        SetPassiveToolBtnStyle(sizeBtn1, "thin.png", ".SizeThin");
                                                        SetPassiveToolBtnStyle(sizeBtn2, "regular.png", ".SizeMedium");
                                                        SetActiveToolBtnStyle(sizeBtn3, "boldA.png", ".SizeBold");
                                                        SetPassiveToolBtnStyle(sizeBtn4, "large.png", ".SizeLarge");

                                                      return false; });

    sizeBtn4->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                  { m_drawArea.drawParams.lineWidth = 15.0;

                                                        SetPassiveToolBtnStyle(sizeBtn1, "thin.png", ".SizeThin");
                                                        SetPassiveToolBtnStyle(sizeBtn2, "regular.png", ".SizeMedium");
                                                        SetPassiveToolBtnStyle(sizeBtn3, "bold.png", ".SizeBold");
                                                        SetActiveToolBtnStyle(sizeBtn4, "largeA.png", ".SizeLarge");

                                                        return false; });

    eraseBtn->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                  {
                                            for(int i = 0; i < 10; i++)
                                            {
                                                if(!m_drawArea.m_alreadyDrawn.empty())
                                                {
                                                    m_drawArea.m_alreadyDrawn.pop_back();
                                                    m_drawArea.queue_draw();
                                                }
                                            } 

                                            return false; });

    saveNewBtn->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                    {
                                            saveNewBtn->set_sensitive(false);

                                            save_buffer = Gdk::Pixbuf::create(
                                                        m_drawArea.currentContext->get_target(),
                                                        m_drawArea.ImgOffsetX,
                                                        m_drawArea.ImgOffsetY + 88,
                                                        m_drawArea.get_width(),
                                                        m_drawArea.get_height());

                                             CalculateLastFileIndex();
                                             std::string path = global::saveImagePath + global::saveImagePrefix + "_" + std::to_string(savedFileIndex + 1) + ".jpeg";

                                             save_buffer->save(path, "jpeg", key, value);
                                             
                                             std::string notificationText = "Скриншот " +
                                              global::saveImagePrefix + "_" + std::to_string(savedFileIndex + 1) +
                                                  " сохранен в " + global::saveImagePath; 

                                            Glib::signal_timeout().connect([this](){
                                                saveNewBtn->set_sensitive(true);
                                                m_drawArea.queue_draw();
                                                return false;}
                                                , 1000);
                                            
                                            return false; });

    m_headerBar.set_show_close_button(true);
    set_titlebar(m_headerBar);

    drawContainer->add(m_drawArea);
    add(*container);

    SetActiveToolTypeBtnStyle(showShapes, "shapesA.png", ".ShowShapes");
    SetActiveToolBtnStyle(sizeBtn1, "thinA.png", ".SizeThin");
    SetActiveToolBtnStyle(drawPencilBtn, "pencilA.png", ".Pencil");

    ProcessPanel(uiBuilder, this, "PaintScreen");
}

void PaintWindow::ConnectCSS()
{
    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, global::paintWindowCSS.c_str(), NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                              GTK_STYLE_PROVIDER(cssProvider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER);
}

PaintWindow::PaintWindow()
{
    ProcessWidgets();
    ConnectCSS();
    m_drawArea.parentContainer = drawContainer;
}

void PaintWindow::OpenWindow()
{
    ShowWindow(this);
    m_drawArea.m_buffer.reset();
    m_drawArea.m_alreadyDrawn.clear();
}