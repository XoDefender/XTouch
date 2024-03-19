#include "global.hxx"

#include "OcctGtkViewer.hxx"
#include "PaintWindow.hxx"

#include <string>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

void PaintWindow::CalculateLastFileIndex()
{
    savedFileIndex = 0;

    std::vector<std::string> fileNames = {};
    for (const auto &entry : fs::directory_iterator(global::saveImagePath))
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

void PaintWindow::SelectDrawColor()
{
    colorChooser->set_transient_for(*this);
    const int result = colorChooser->run();
    switch (result)
    {
    case Gtk::RESPONSE_OK:
    {
        m_drawArea.SetColor(colorChooser->get_rgba());
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
    }
}

bool PaintWindow::OnGoBackBtnClick(GdkEventButton *widget)
{
    hide();
    ShowWindow(aGtkWin);
    aGtkWin->HideWidgets();
    return true;
}

bool PaintWindow::SetDrawSizeBtnActive(GdkEventButton *widget)
{
    toolsStack->set_visible_child("Sizes");
    SetActiveToolTypeBtnStyle(showSizes, "brushA.png", ".ShowSizes");
    SetPassiveToolTypeBtnStyle(showShapes, "shapes.png", ".ShowShapes");

    return true;
}

bool PaintWindow::SetDrawShapeBtnActive(GdkEventButton *widget)
{
    toolsStack->set_visible_child("Shapes");
    SetActiveToolTypeBtnStyle(showShapes, "shapesA.png", ".ShowShapes");
    SetPassiveToolTypeBtnStyle(showSizes, "brush.png", ".ShowSizes");

    return true;
}

bool PaintWindow::SetRectBtnActive(GdkEventButton *widget)
{
    m_drawArea.SetCurrentShape(DrawHelper::Shape::Rectangle);

    SetActiveToolBtnStyle(drawRectangleBtn, "squareA.png", ".Rectangle");
    SetPassiveToolBtnStyle(drawCircleBtn, "circle.png", ".Circle");
    SetPassiveToolBtnStyle(drawLineBtn, "line.png", ".Line");
    SetPassiveToolBtnStyle(drawPencilBtn, "pencil.png", ".Pencil");

    return true;
}

bool PaintWindow::SetCircleBtnActive(GdkEventButton *widget)
{
    m_drawArea.SetCurrentShape(DrawHelper::Shape::Circle);

    SetPassiveToolBtnStyle(drawRectangleBtn, "square.png", ".Rectangle");
    SetActiveToolBtnStyle(drawCircleBtn, "circleA.png", ".Circle");
    SetPassiveToolBtnStyle(drawLineBtn, "line.png", ".Line");
    SetPassiveToolBtnStyle(drawPencilBtn, "pencil.png", ".Pencil");

    return true;
}

bool PaintWindow::SetLineBtnActive(GdkEventButton *widget)
{
    m_drawArea.SetCurrentShape(DrawHelper::Shape::Line);

    SetPassiveToolBtnStyle(drawRectangleBtn, "square.png", ".Rectangle");
    SetPassiveToolBtnStyle(drawCircleBtn, "circle.png", ".Circle");
    SetActiveToolBtnStyle(drawLineBtn, "lineA.png", ".Line");
    SetPassiveToolBtnStyle(drawPencilBtn, "pencil.png", ".Pencil");

    return true;
}

bool PaintWindow::SetPencilBtnActive(GdkEventButton *widget)
{
    m_drawArea.SetCurrentShape(DrawHelper::Shape::Pencil);

    SetPassiveToolBtnStyle(drawRectangleBtn, "square.png", ".Rectangle");
    SetPassiveToolBtnStyle(drawCircleBtn, "circle.png", ".Circle");
    SetPassiveToolBtnStyle(drawLineBtn, "line.png", ".Line");
    SetActiveToolBtnStyle(drawPencilBtn, "pencilA.png", ".Pencil");

    return true;
}

bool PaintWindow::SetSizeThinBtnActive(GdkEventButton *widget)
{
    m_drawArea.SetLineWidth(5.0);

    SetActiveToolBtnStyle(sizeBtnThin, "thinA.png", ".SizeThin");
    SetPassiveToolBtnStyle(sizeBtnMedium, "regular.png", ".SizeMedium");
    SetPassiveToolBtnStyle(sizeBtnBold, "bold.png", ".SizeBold");
    SetPassiveToolBtnStyle(sizeBtnBoldPlus, "large.png", ".SizeLarge");

    return true;
}

bool PaintWindow::SetSizeMediumBtnActive(GdkEventButton *widget)
{
    m_drawArea.SetLineWidth(8.0);

    SetPassiveToolBtnStyle(sizeBtnThin, "thin.png", ".SizeThin");
    SetActiveToolBtnStyle(sizeBtnMedium, "regularA.png", ".SizeMedium");
    SetPassiveToolBtnStyle(sizeBtnBold, "bold.png", ".SizeBold");
    SetPassiveToolBtnStyle(sizeBtnBoldPlus, "large.png", ".SizeLarge");

    return true;
}

bool PaintWindow::SetSizeBoldBtnActive(GdkEventButton *widget)
{
    m_drawArea.SetLineWidth(10.0);

    SetPassiveToolBtnStyle(sizeBtnThin, "thin.png", ".SizeThin");
    SetPassiveToolBtnStyle(sizeBtnMedium, "regular.png", ".SizeMedium");
    SetActiveToolBtnStyle(sizeBtnBold, "boldA.png", ".SizeBold");
    SetPassiveToolBtnStyle(sizeBtnBoldPlus, "large.png", ".SizeLarge");

    return true;
}

bool PaintWindow::SetSizeBoldPlusBtnActive(GdkEventButton *widget)
{
    m_drawArea.SetLineWidth(15.0);

    SetPassiveToolBtnStyle(sizeBtnThin, "thin.png", ".SizeThin");
    SetPassiveToolBtnStyle(sizeBtnMedium, "regular.png", ".SizeMedium");
    SetPassiveToolBtnStyle(sizeBtnBold, "bold.png", ".SizeBold");
    SetActiveToolBtnStyle(sizeBtnBoldPlus, "largeA.png", ".SizeLarge");

    return true;
}

bool PaintWindow::OnSaveImgBtnClick(GdkEventButton *widget)
{
    saveNewBtn->set_sensitive(false);

    save_buffer = Gdk::Pixbuf::create(
        m_drawArea.GetDrawContext()->get_target(),
        m_drawArea.GetImgOffsetX(),
        m_drawArea.GetImgOffsetY() + 88,
        m_drawArea.get_width(),
        m_drawArea.get_height());

    if (!fs::exists(global::saveImagePath)) 
        fs::create_directory(global::saveImagePath);

    CalculateLastFileIndex();
    std::string path = global::saveImagePath + global::saveImagePrefix + "_" + std::to_string(savedFileIndex + 1) + ".jpeg";

    save_buffer->save(path, "jpeg", key, value);

    std::string notificationText = "Скриншот " +
                                   global::saveImagePrefix + "_" + std::to_string(savedFileIndex + 1) +
                                   " сохранен в " + global::saveImagePath;

    Glib::signal_timeout().connect([this]()
                                   { saveNewBtn->set_sensitive(true);
                                     m_drawArea.queue_draw();
                                     return false; },
                                   1000);

    return true;
}

bool PaintWindow::OnEraseShapesBtnClick(GdkEventButton *widget)
{
    for (int i = 0; i < 10; i++)
    {
        if (!m_drawArea.m_alreadyDrawn.empty())
        {
            m_drawArea.m_alreadyDrawn.pop_back();
            m_drawArea.queue_draw();
        }
    }

    return true;
}

void PaintWindow::ProcessEvents()
{
    goBackBtn->signal_button_press_event().connect(sigc::mem_fun(*this, &PaintWindow::OnGoBackBtnClick), false);

    showSizes->signal_button_press_event().connect(sigc::mem_fun(*this, &PaintWindow::SetDrawSizeBtnActive), false);
    showShapes->signal_button_press_event().connect(sigc::mem_fun(*this, &PaintWindow::SetDrawShapeBtnActive), false);

    drawRectangleBtn->signal_button_press_event().connect(sigc::mem_fun(*this, &PaintWindow::SetRectBtnActive), false);
    drawCircleBtn->signal_button_press_event().connect(sigc::mem_fun(*this, &PaintWindow::SetCircleBtnActive), false);
    drawLineBtn->signal_button_press_event().connect(sigc::mem_fun(*this, &PaintWindow::SetLineBtnActive), false);
    drawPencilBtn->signal_button_press_event().connect(sigc::mem_fun(*this, &PaintWindow::SetPencilBtnActive), false);

    selectColorBtn->signal_clicked().connect(sigc::mem_fun(*this, &PaintWindow::SelectDrawColor), false);

    sizeBtnThin->signal_button_press_event().connect(sigc::mem_fun(*this, &PaintWindow::SetSizeThinBtnActive), false);
    sizeBtnMedium->signal_button_press_event().connect(sigc::mem_fun(*this, &PaintWindow::SetSizeMediumBtnActive), false);
    sizeBtnBold->signal_button_press_event().connect(sigc::mem_fun(*this, &PaintWindow::SetSizeBoldBtnActive), false);
    sizeBtnBoldPlus->signal_button_press_event().connect(sigc::mem_fun(*this, &PaintWindow::SetSizeBoldPlusBtnActive), false);

    eraseBtn->signal_button_press_event().connect(sigc::mem_fun(*this, &PaintWindow::OnEraseShapesBtnClick), false);
    saveNewBtn->signal_button_press_event().connect(sigc::mem_fun(*this, &PaintWindow::OnSaveImgBtnClick), false);

    colorBtnRed->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                     {m_drawArea.SetColor(Gdk::RGBA("#FF0000")); return false; });
    colorBtnGreen->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                       {m_drawArea.SetColor(Gdk::RGBA("#00FF00")); return false; });
    colorBtnBlue->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                      {m_drawArea.SetColor(Gdk::RGBA("#0000FF")); return false; });
    colorBtnWhite->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                       {m_drawArea.SetColor(Gdk::RGBA("#FFFFFF")); return false; });
    colorBtnBlack->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                       {m_drawArea.SetColor(Gdk::RGBA("#000000")); return false; });
}

void PaintWindow::ProcessWidgets()
{
    uiBuilder = Gtk::Builder::create_from_file(global::paintWindowUI);

    uiBuilder->get_widget<Gtk::EventBox>("BackBtn", goBackBtn);
    uiBuilder->get_widget<Gtk::Fixed>("ScreenshotScreen", container);
    uiBuilder->get_widget<Gtk::Fixed>("DrawContainer", drawContainer);

    uiBuilder->get_widget<Gtk::EventBox>("ColorBtnRed", colorBtnRed);
    uiBuilder->get_widget<Gtk::EventBox>("ColorBtnGreen", colorBtnGreen);
    uiBuilder->get_widget<Gtk::EventBox>("ColorBtnBlue", colorBtnBlue);
    uiBuilder->get_widget<Gtk::EventBox>("ColorBtnWhite", colorBtnWhite);
    uiBuilder->get_widget<Gtk::EventBox>("ColorBtnBlack", colorBtnBlack);

    uiBuilder->get_widget<Gtk::EventBox>("SizeBtnThin", sizeBtnThin);
    uiBuilder->get_widget<Gtk::EventBox>("SizeBtnMedium", sizeBtnMedium);
    uiBuilder->get_widget<Gtk::EventBox>("SizeBtnBold", sizeBtnBold);
    uiBuilder->get_widget<Gtk::EventBox>("SizeBtnBoldPlus", sizeBtnBoldPlus);

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

    ProcessEvents();
    ProcessPanel(uiBuilder, this, "PaintScreen");

    SetActiveToolTypeBtnStyle(showShapes, "shapesA.png", ".ShowShapes");
    SetActiveToolBtnStyle(sizeBtnThin, "thinA.png", ".SizeThin");
    SetActiveToolBtnStyle(drawPencilBtn, "pencilA.png", ".Pencil");

    m_headerBar.set_show_close_button(true);
    set_titlebar(m_headerBar);

    drawContainer->add(m_drawArea);
    add(*container);
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
    m_drawArea.SetParentContainer(drawContainer);
}

void PaintWindow::OpenWindow()
{
    ShowWindow(this);
    m_drawArea.ClearDrawBuffer();
}