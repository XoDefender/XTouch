#include <cairo/cairo.h>
#include <cairomm/context.h>
#include <cairomm/surface.h>
#include <memory>
#include <gtkmm.h>

class IShape
{
public:
    virtual ~IShape() = default;
    virtual void Draw(const Cairo::RefPtr<Cairo::Context> &p_context) = 0;
};

class Pencil : public IShape
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
    Pencil(float p_left, float p_up, float x, float y, float R, float G, float B, float SIZE);

public:
    void Draw(const Cairo::RefPtr<Cairo::Context> &p_context) override;
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
    Rectangle(double p_left, double p_up, double p_width, double p_height, float R, float G, float B, float SIZE);
    void Draw(const Cairo::RefPtr<Cairo::Context> &p_context) override;
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
    Circle(double p_cX, double p_cY, double p_radius, float R, float G, float B, float SIZE);
    void Draw(const Cairo::RefPtr<Cairo::Context> &p_context) override;
};

class DrawHelper : public Gtk::DrawingArea
{
public:
    enum class Shape
    {
        Line,
        Pencil,
        Rectangle,
        Circle,
    };

    std::vector<std::unique_ptr<class IShape>> m_alreadyDrawn;

    float prevX;
    float prevY;
    bool isFirstInit = true;

    Cairo::RefPtr<Cairo::Context> currentContext;
    Glib::RefPtr<Gdk::Pixbuf> m_buffer;

    struct DrawParams
    {
        Gdk::RGBA color;
        double lineWidth = 8.0;
    };

    DrawParams drawParams;

    int ImgOffsetX, ImgOffsetY;
    int defaultWidth = 1920;
    int defaultHeight = 820;
    Gtk::Fixed* parentContainer;

private:
    float curX;
    float curY;

    double m_startX;
    double m_startY;

    double m_endX;
    double m_endY;

    double m_width;
    double m_height;

    Shape m_currentShape = Shape::Pencil;

public:
    DrawHelper();
    void SetCurrentShape(Shape p_shape);

private:
    bool on_draw(const Cairo::RefPtr<Cairo::Context> &p_context) override;
    bool OnMouseMotion(GdkEventMotion *p_event);
    bool OnButtonPressed(GdkEventButton *p_event);
    bool OnButtonReleased(GdkEventButton *p_event);
    void ClearShapeData();
};