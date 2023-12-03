#include <memory>
#include <vector>

#include <gtkmm.h>
#include <cairo/cairo.h>
#include <cairomm/context.h>
#include <cairomm/surface.h>
#include <iostream>

#include <string>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;

int DEFAULT_WIDTH = 1920;
int DEFAULT_HEIGHT = 820;
double IMG_OFFSET_X = 0;
double IMG_OFFSET_Y = 0;

double LINE_WIDTH = 5.0;
double R = 0;
double G = 0;
double B = 1;

Gdk::RGBA color;

bool isAbsolute = false;

Gtk::Fixed *drawContainer;

string saveFolderPath;
string saveFolderPrefix;
int lastFileIndex = 0;

void CalculateLastFileIndex()
{
    lastFileIndex = 0;

    vector<string> fileNames = {};
    for (const auto &entry : fs::directory_iterator(saveFolderPath))
        fileNames.push_back(entry.path());

    for (string fileName : fileNames)
    {
        int currentIndex = stoi(fileName.substr(fileName.find("_") + 1));
        if (currentIndex > lastFileIndex)
        {
            lastFileIndex = currentIndex;
        }
    }
}

string ParseString(vector<string> data, int index)
{
    return data[index].substr(data[index].find("=") + 2);
}

void ParseGlobalData()
{
    fstream dataFile;
    dataFile.open("/home/xodefender/Apps/XTouch/XTouchClient/res/Config");

    vector<string> fileData = {};

    if (dataFile.is_open())
    {
        string tp;
        while (getline(dataFile, tp))
        {
            if (tp[0] != '#' && tp != "")
                fileData.push_back(tp);
        }

        dataFile.close();

        saveFolderPath = ParseString(fileData, 4);
        saveFolderPrefix = ParseString(fileData, 5);
    }
}

void DrawPancil(const Cairo::RefPtr<Cairo::Context> &p_context,
                float curX,
                float curY,
                float prevX,
                float prevY,
                float r,
                float g,
                float b,
                float size)
{
    if (prevX == 0 && prevY == 0)
        return;

    p_context->save();

    p_context->set_line_width(size);
    p_context->set_line_cap(Cairo::LINE_CAP_ROUND);
    p_context->set_source_rgba(r, g, b, 1);
    p_context->move_to(curX, curY);
    p_context->line_to(prevX, prevY);
    p_context->stroke();

    p_context->restore();
}

void DrawRectangle(const Cairo::RefPtr<Cairo::Context> &p_context,
                   double p_startX,
                   double p_startY,
                   double p_width,
                   double p_height,
                   float r,
                   float g,
                   float b,
                   float size)
{
    p_context->save();

    p_context->set_line_width(size);
    p_context->set_source_rgba(r, g, b, 1);
    p_context->rectangle(p_startX, p_startY, p_width, p_height);
    p_context->stroke();

    p_context->restore();
}

void DrawCircle(const Cairo::RefPtr<Cairo::Context> &p_context,
                double p_startX,
                double p_startY,
                double p_width,
                float r,
                float g,
                float b,
                float size)
{
    p_context->save();

    p_context->set_line_width(size);
    p_context->set_source_rgba(r, g, b, 1);
    p_context->arc(p_startX, p_startY, p_width, 0, 2 * M_PI);
    p_context->stroke();

    p_context->restore();
}

class IShape
{

public:
    virtual ~IShape() = default;

    virtual void Draw(const Cairo::RefPtr<Cairo::Context> &p_context) = 0;
};

class Pancil : public IShape
{
public:
    float prevX;
    float prevY;
    float curX;
    float curY;
    float r;
    float g;
    float b;
    float size;

public:
    Pancil(float p_left, float p_up, float x, float y, float R, float G, float B, float SIZE)
        : curX{p_left}, curY{p_up}, prevX{x}, prevY{y}, r{R}, g{G}, b{B}, size{SIZE}
    {
    }

public:
    void Draw(const Cairo::RefPtr<Cairo::Context> &p_context) override
    {
        DrawPancil(p_context, curX, curY, prevX, prevY, r, g, b, size);
    }
};

class Rectangle : public IShape
{
private:
    double m_up;
    double m_left;
    double m_width;
    double m_height;
    float r;
    float g;
    float b;
    float size;

public:
    Rectangle(double p_left, double p_up, double p_width, double p_height, float R, float G, float B, float SIZE)
        : m_left{p_left}, m_up{p_up}, m_width{p_width}, m_height{p_height}, r{R}, g{G}, b{B}, size{SIZE}
    {
    }

    void Draw(const Cairo::RefPtr<Cairo::Context> &p_context) override
    {
        DrawRectangle(p_context, m_left, m_up, m_width, m_height, r, g, b, size);
    }
};

class Circle : public IShape
{
private:
    double m_cX;
    double m_cY;
    double m_radius;
    float r;
    float g;
    float b;
    float size;

public:
    Circle(double p_cX, double p_cY, double p_radius, float R, float G, float B, float SIZE)
        : m_cX{p_cX}, m_cY{p_cY}, m_radius{p_radius}, r{R}, g{G}, b{B}, size{SIZE}
    {
    }

    void Draw(const Cairo::RefPtr<Cairo::Context> &p_context) override
    {
        DrawCircle(p_context, m_cX, m_cY, m_radius, r, g, b, size);
    }
};

class DrawHelper : public Gtk::DrawingArea
{
public:
    enum class Shape
    {
        Line,
        Pancil,
        Rectangle,
        Circle,
    };

    vector<unique_ptr<IShape>> m_alreadyDrawn;

    float prevX;
    float prevY;
    bool isFirstInit = true;

    Cairo::RefPtr<Cairo::Context> currentContext;
    Glib::RefPtr<Gdk::Pixbuf> m_buffer;

private:
    float curX;
    float curY;

    double m_startX;
    double m_startY;

    double m_endX;
    double m_endY;

    double m_width;
    double m_height;

    Shape m_currentShape = Shape::Pancil;

public:
    DrawHelper()
    {
        add_events(Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);

        signal_motion_notify_event().connect([this](GdkEventMotion *p_event)
                                             { return OnMouseMotion(p_event); });
        signal_button_release_event().connect([this](GdkEventButton *p_event)
                                              { return OnButtonReleased(p_event); });
        signal_button_press_event().connect([this](GdkEventButton *p_event)
                                            { return OnButtonPressed(p_event); });
    }

    void SetCurrentShape(Shape p_shape)
    {
        m_currentShape = p_shape;
    }

private:
    // All drawing occurs here and only here:
    bool on_draw(const Cairo::RefPtr<Cairo::Context> &p_context) override
    {
        // Draw background:
        if (!m_buffer)
        {
            m_buffer = Gdk::Pixbuf::create_from_file("/tmp/file.png");

            if (m_buffer->get_height() >= 820)
            {
                IMG_OFFSET_X = (DEFAULT_WIDTH - m_buffer->get_width()) / 2;
                IMG_OFFSET_Y = ((DEFAULT_HEIGHT - 820) / 2);

                set_size_request(m_buffer->get_width(), 820);
            }
            else
            {
                IMG_OFFSET_X = (DEFAULT_WIDTH - m_buffer->get_width()) / 2;
                IMG_OFFSET_Y = ((DEFAULT_HEIGHT - m_buffer->get_height()) / 2);

                set_size_request(m_buffer->get_width(), m_buffer->get_height());
            }

            drawContainer->move(*this, IMG_OFFSET_X, IMG_OFFSET_Y);
        }

        Gdk::Cairo::set_source_pixbuf(p_context, m_buffer, 0, 0);

        p_context->paint();

        for (const auto &shape : m_alreadyDrawn)
        {
            shape->Draw(p_context);
        }

        if (m_currentShape == Shape::Rectangle)
        {
            DrawRectangle(p_context, m_startX, m_startY, m_width, m_height, R, G, B, LINE_WIDTH);
        }
        else if (m_currentShape == Shape::Circle)
        {
            if (isAbsolute)
                DrawCircle(p_context, m_startX, m_startY, abs(m_width) + abs(m_height), R, G, B, LINE_WIDTH);
            else
                DrawCircle(p_context, m_startX, m_startY, m_width + m_height, R, G, B, LINE_WIDTH);
        }
        else if (m_currentShape == Shape::Line)
        {
            DrawPancil(p_context, curX, curY, prevX, prevY, R, G, B, LINE_WIDTH);
        }
        else
        {
            prevX = curX;
            prevY = curY;
        }

        currentContext = p_context;

        return false;
    }

    bool OnMouseMotion(GdkEventMotion *p_event)
    {
        if (m_currentShape == Shape::Pancil)
        {
            curX = p_event->x;
            curY = p_event->y;

            if (isFirstInit)
            {
                prevX = curX;
                prevY = curY;

                isFirstInit = false;
            }

            m_alreadyDrawn.push_back(std::make_unique<Pancil>(curX, curY, prevX, prevY, R, G, B, LINE_WIDTH));
        }
        else if (m_currentShape == Shape::Line)
        {
            curX = p_event->x;
            curY = p_event->y;

            if (isFirstInit)
            {
                prevX = curX;
                prevY = curY;

                isFirstInit = false;
            }
        }
        else if (m_currentShape == Shape::Circle || m_currentShape == Shape::Rectangle)
        {
            m_endX = p_event->x;
            m_endY = p_event->y;

            m_width = m_endX - m_startX;
            m_height = m_endY - m_startY;
        }

        queue_draw();

        return true;
    }

    bool OnButtonPressed(GdkEventButton *p_event)
    {
        m_startX = p_event->x;
        m_startY = p_event->y;

        return true;
    }

    bool OnButtonReleased(GdkEventButton *p_event)
    {
        if (m_currentShape == Shape::Rectangle)
        {
            m_alreadyDrawn.push_back(std::make_unique<Rectangle>(m_startX, m_startY, m_width, m_height, R, G, B, LINE_WIDTH));
        }
        else if (m_currentShape == Shape::Circle)
        {
            if (isAbsolute)
                m_alreadyDrawn.push_back(std::make_unique<Circle>(m_startX, m_startY, abs(m_width) + abs(m_height), R, G, B, LINE_WIDTH));
            else
                m_alreadyDrawn.push_back(std::make_unique<Circle>(m_startX, m_startY, m_width + m_height, R, G, B, LINE_WIDTH));
        }
        else if (m_currentShape == Shape::Line)
        {
            m_alreadyDrawn.push_back(std::make_unique<Pancil>(curX, curY, prevX, prevY, R, G, B, LINE_WIDTH));
        }

        ClearShapeData();

        return true;
    }

    void ClearShapeData()
    {
        m_endX = 0;
        m_endY = 0;
        m_width = 0;
        m_height = 0;

        curX = 0;
        curY = 0;
        prevX = 0;
        prevY = 0;

        isFirstInit = true;
    }
};

class MyWindow : public Gtk::Window
{
private:
    Gtk::EventBox *colorBtn1;
    Gtk::EventBox *colorBtn2;
    Gtk::EventBox *colorBtn3;
    Gtk::EventBox *colorBtn4;
    Gtk::EventBox *colorBtn5;

    Gtk::EventBox *sizeBtn1;
    Gtk::EventBox *sizeBtn2;
    Gtk::EventBox *sizeBtn3;
    Gtk::EventBox *sizeBtn4;

    Gtk::EventBox *eraseBtn;
    // Gtk::EventBox *eraseAllBtn;

    Gtk::EventBox *drawRectangleBtn;
    Gtk::EventBox *drawCircleBtn;
    Gtk::EventBox *drawLineBtn;
    Gtk::EventBox *drawPencilBtn;

    // Gtk::EventBox *circleTypeBtn;

    Gtk::EventBox *saveNewBtn;
    Gtk::EventBox *closeWindow;
    Gtk::Fixed *container;

    Gtk::EventBox *showSizes;
    Gtk::EventBox *showShapes;

    Gtk::Stack *toolsStack;
    Gtk::Box *sizes;
    Gtk::Box *shapes;

    Gtk::Button *selectColorBtn;
    Gtk::ColorChooserDialog *colorChooser;

    Gtk::HeaderBar m_headerBar;

    DrawHelper m_drawArea;

    const std::vector<Glib::ustring> key = {"quality"};
    const std::vector<Glib::ustring> value = {"100"};

    Glib::RefPtr<Gdk::Pixbuf> save_buffer;
    Glib::RefPtr<Gtk::Builder> uiBuilder;

private:
    void SetPassiveToolTypeBtnStyle(Gtk::EventBox *previousBtn,
                                    string previousBtnIcon,
                                    string previousBtnClass)
    {
        Glib::RefPtr<Gtk::CssProvider> cssProviderPrev = Gtk::CssProvider::create();
        string previousButtonStyle = previousBtnClass + string("{background: url('/home/xodefender/Apps/XTouch/XTouchClient/res/Windows/ScreenshotWindow/img/") + previousBtnIcon + string("') no-repeat center;\
                                                background-color:#e5ecff;\
                                                background-size: 35px;}");

        cssProviderPrev->load_from_data(previousButtonStyle);
        previousBtn->get_style_context()
            ->add_provider(
                cssProviderPrev, GTK_STYLE_PROVIDER_PRIORITY_USER);
    }

    void SetActiveToolTypeBtnStyle(Gtk::EventBox *currentBtn,
                                   string currentBtnIcon,
                                   string currentBtnClass)
    {
        Glib::RefPtr<Gtk::CssProvider> cssProviderCurr = Gtk::CssProvider::create();
        string currentButtonStyle = currentBtnClass + string("{background: url('/home/xodefender/Apps/XTouch/XTouchClient/res/Windows/ScreenshotWindow/img/") + currentBtnIcon + string("') no-repeat center;\
                                                background-color:#e5ecff;\
                                                background-size: 35px;}");

        cssProviderCurr->load_from_data(currentButtonStyle);
        currentBtn->get_style_context()
            ->add_provider(
                cssProviderCurr, GTK_STYLE_PROVIDER_PRIORITY_USER);
    }

    void SetPassiveToolBtnStyle(Gtk::EventBox *previousBtn,
                                string previousBtnIcon,
                                string previousBtnClass)
    {
        Glib::RefPtr<Gtk::CssProvider> cssProviderPrev = Gtk::CssProvider::create();
        string previousButtonStyle = previousBtnClass + string("{background: url('/home/xodefender/Apps/XTouch/XTouchClient/res/Windows/ScreenshotWindow/img/") + previousBtnIcon + string("') no-repeat center;\
                                                border-radius: 10px;\
                                                border: 2px solid rgb(219, 219, 219);\
                                                background-size: 35px;}");

        cssProviderPrev->load_from_data(previousButtonStyle);
        previousBtn->get_style_context()
            ->add_provider(
                cssProviderPrev, GTK_STYLE_PROVIDER_PRIORITY_USER);
    }

    void SetActiveToolBtnStyle(Gtk::EventBox *currentBtn,
                               string currentBtnIcon,
                               string currentBtnClass)
    {
        Glib::RefPtr<Gtk::CssProvider> cssProviderCurr = Gtk::CssProvider::create();
        string currentButtonStyle = currentBtnClass + string("{background: url('/home/xodefender/Apps/XTouch/XTouchClient/res/Windows/ScreenshotWindow/img/") + currentBtnIcon + string("') no-repeat center;\
                                                border-radius: 10px;\
                                                border: 2px solid #4B6EEE;\
                                                background-size: 35px;}");

        cssProviderCurr->load_from_data(currentButtonStyle);
        currentBtn->get_style_context()
            ->add_provider(
                cssProviderCurr, GTK_STYLE_PROVIDER_PRIORITY_USER);
    }

public:
    MyWindow()
    {
        uiBuilder = Gtk::Builder::create_from_file("/home/xodefender/Apps/XTouch/XTouchClient/res/Windows/ScreenshotWindow/ScreenshotWindow.glade");

        uiBuilder->get_widget<Gtk::EventBox>("CloseScreen", closeWindow);
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

        // uiBuilder->get_widget<Gtk::EventBox>("EraseAllBtn", eraseAllBtn);

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

        // uiBuilder->get_widget<Gtk::EventBox>("CircleTypeBtn", circleTypeBtn);

        uiBuilder->get_widget<Gtk::EventBox>("SaveNew", saveNewBtn);

        closeWindow->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                         { hide(); 
                                                           return false; });

        // circleTypeBtn->signal_button_press_event().connect([this](GdkEventButton *theEvent)
        //                                                    { if(isAbsolute) isAbsolute = false;
        //                                                     else isAbsolute = true;
        //                                                     return false; });

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
                                                           { m_drawArea.SetCurrentShape(DrawHelper::Shape::Pancil);

                                                            SetPassiveToolBtnStyle(drawRectangleBtn, "square.png", ".Rectangle");
                                                            SetPassiveToolBtnStyle(drawCircleBtn, "circle.png", ".Circle");
                                                            SetPassiveToolBtnStyle(drawLineBtn, "line.png", ".Line");
                                                            SetActiveToolBtnStyle(drawPencilBtn, "pencilA.png", ".Pencil");

                                                            return false; });

        colorBtn1->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                       {R = 1; 
                                                        G = 0;
                                                        B = 0;
                                                        return false; });

        colorBtn2->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                       {R = 0;
                                                        G = 1;
                                                        B = 0;
                                                        return false; });

        colorBtn3->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                       {R = 0;
                                                        G = 0; 
                                                        B = 1;
                                                        return false; });

        colorBtn4->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                       {R = 1;
                                                        G = 1; 
                                                        B = 1;
                                                        return false; });

        colorBtn5->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                       {R = 0;
                                                        G = 0; 
                                                        B = 0;
                                                        return false; });

        selectColorBtn->signal_clicked().connect([this]()
                                                 { 
                                                    colorChooser->set_transient_for(*this);
                                                    const int result = colorChooser->run();

  //Обработка результата взаимодействия с пользователем:
  switch(result)
  {
    case Gtk::RESPONSE_OK:
    {
      color = colorChooser->get_rgba();

      R = color.get_red();
      G = color.get_green();
      B = color.get_blue();

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
                                                      { LINE_WIDTH = 5.0;

                                                        SetActiveToolBtnStyle(sizeBtn1, "thinA.png", ".SizeThin");
                                                        SetPassiveToolBtnStyle(sizeBtn2, "regular.png", ".SizeMedium");
                                                        SetPassiveToolBtnStyle(sizeBtn3, "bold.png", ".SizeBold");
                                                        SetPassiveToolBtnStyle(sizeBtn4, "large.png", ".SizeLarge");

                                                        return false; });

        sizeBtn2->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                      { LINE_WIDTH = 8.0;

                                                        SetPassiveToolBtnStyle(sizeBtn1, "thin.png", ".SizeThin");
                                                        SetActiveToolBtnStyle(sizeBtn2, "regularA.png", ".SizeMedium");
                                                        SetPassiveToolBtnStyle(sizeBtn3, "bold.png", ".SizeBold");
                                                        SetPassiveToolBtnStyle(sizeBtn4, "large.png", ".SizeLarge");

                                                        return false; });

        sizeBtn3->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                      { LINE_WIDTH = 10.0;

                                                        SetPassiveToolBtnStyle(sizeBtn1, "thin.png", ".SizeThin");
                                                        SetPassiveToolBtnStyle(sizeBtn2, "regular.png", ".SizeMedium");
                                                        SetActiveToolBtnStyle(sizeBtn3, "boldA.png", ".SizeBold");
                                                        SetPassiveToolBtnStyle(sizeBtn4, "large.png", ".SizeLarge");

                                                      return false; });

        sizeBtn4->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                      { LINE_WIDTH = 15.0;

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

        // eraseAllBtn->signal_button_press_event().connect([this](GdkEventButton *theEvent)
        //                                                  {m_drawArea.m_alreadyDrawn.clear();
        //                                                   m_drawArea.queue_draw();
        //                                                   return false; });

        saveNewBtn->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                        {
                                            saveNewBtn->set_sensitive(false);

                                            save_buffer = Gdk::Pixbuf::create(
                                                        m_drawArea.currentContext->get_target(),
                                                        IMG_OFFSET_X,
                                                        IMG_OFFSET_Y + 88,
                                                        m_drawArea.get_width(),
                                                        m_drawArea.get_height());

                                             CalculateLastFileIndex();
                                             string path = saveFolderPath + saveFolderPrefix + "_" + to_string(lastFileIndex + 1) + ".jpeg";

                                             save_buffer->save(path, "jpeg", key, value);
                                             
                                             string notificationText = "Скриншот " +
                                              saveFolderPrefix + "_" + to_string(lastFileIndex + 1) +
                                                  " сохранен в " + saveFolderPath; 

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

        GtkCssProvider *cssProvider = gtk_css_provider_new();
        gtk_css_provider_load_from_path(cssProvider, "/home/xodefender/Apps/XTouch/XTouchClient/res/Windows/ScreenshotWindow/style.css", NULL);
        gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                                  GTK_STYLE_PROVIDER(cssProvider),
                                                  GTK_STYLE_PROVIDER_PRIORITY_USER);

        fullscreen();
        show_all();
    }
};

int main(int argc, char *argv[])
{
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.examples.base");

    ParseGlobalData();

    MyWindow window;

    return app->run(window);
}