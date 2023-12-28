#include "DrawHelper.hxx"

void DrawPencil(const Cairo::RefPtr<Cairo::Context> &p_context,
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

Pencil::Pencil(float p_left, float p_up, float x, float y, float R, float G, float B, float SIZE)
    : curX{p_left}, curY{p_up}, prevX{x}, prevY{y}, r{R}, g{G}, b{B}, size{SIZE} {}

void Pencil::Draw(const Cairo::RefPtr<Cairo::Context> &p_context)
{
    DrawPencil(p_context, curX, curY, prevX, prevY, r, g, b, size);
}

Rectangle::Rectangle(double p_left, double p_up, double p_width, double p_height, float R, float G, float B, float SIZE)
    : m_left{p_left}, m_up{p_up}, m_width{p_width}, m_height{p_height}, r{R}, g{G}, b{B}, size{SIZE} {}

void Rectangle::Draw(const Cairo::RefPtr<Cairo::Context> &p_context)
{
    DrawRectangle(p_context, m_left, m_up, m_width, m_height, r, g, b, size);
}

Circle::Circle(double p_cX, double p_cY, double p_radius, float R, float G, float B, float SIZE)
    : m_cX{p_cX}, m_cY{p_cY}, m_radius{p_radius}, r{R}, g{G}, b{B}, size{SIZE} {}

void Circle::Draw(const Cairo::RefPtr<Cairo::Context> &p_context)
{
    DrawCircle(p_context, m_cX, m_cY, m_radius, r, g, b, size);
}

DrawHelper::DrawHelper()
{
    add_events(Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);

    signal_motion_notify_event().connect([this](GdkEventMotion *p_event)
                                         { return OnMouseMotion(p_event); });
    signal_button_release_event().connect([this](GdkEventButton *p_event)
                                          { return OnButtonReleased(p_event); });
    signal_button_press_event().connect([this](GdkEventButton *p_event)
                                        { return OnButtonPressed(p_event); });
}

void DrawHelper::SetCurrentShape(Shape p_shape)
{
    m_currentShape = p_shape;
}

bool DrawHelper::on_draw(const Cairo::RefPtr<Cairo::Context> &p_context)
{
    // Draw background
    if (!m_buffer)
    {
        m_buffer = Gdk::Pixbuf::create_from_file("/tmp/file.png");

        if (m_buffer->get_height() >= 820)
        {
            ImgOffsetX = (defaultWidth - m_buffer->get_width()) / 2;
            ImgOffsetY = ((defaultHeight - 820) / 2);

            set_size_request(m_buffer->get_width(), 820);
        }
        else
        {
            ImgOffsetX = (defaultWidth - m_buffer->get_width()) / 2;
            ImgOffsetY = ((defaultHeight - m_buffer->get_height()) / 2);

            set_size_request(m_buffer->get_width(), m_buffer->get_height());
        }

        parentContainer->move(*this, ImgOffsetX, ImgOffsetY);
    }

    Gdk::Cairo::set_source_pixbuf(p_context, m_buffer, 0, 0);

    p_context->paint();

    for (const auto &shape : m_alreadyDrawn)
    {
        shape->Draw(p_context);
    }

    if (m_currentShape == Shape::Rectangle)
    {
        DrawRectangle(p_context, m_startX, m_startY, m_width, m_height,
                      drawParams.color.get_red(), drawParams.color.get_green(), drawParams.color.get_blue(),
                      drawParams.lineWidth);
    }
    else if (m_currentShape == Shape::Circle)
    {
        DrawCircle(p_context, m_startX, m_startY, m_width + m_height,
                   drawParams.color.get_red(), drawParams.color.get_green(), drawParams.color.get_blue(),
                   drawParams.lineWidth);
    }
    else if (m_currentShape == Shape::Line)
    {
        DrawPencil(p_context, curX, curY, prevX, prevY,
                   drawParams.color.get_red(), drawParams.color.get_green(), drawParams.color.get_blue(),
                   drawParams.lineWidth);
    }
    else
    {
        prevX = curX;
        prevY = curY;
    }

    currentContext = p_context;

    return false;
}

bool DrawHelper::OnMouseMotion(GdkEventMotion *p_event)
{
    if (m_currentShape == Shape::Pencil)
    {
        curX = p_event->x;
        curY = p_event->y;

        if (isFirstInit)
        {
            prevX = curX;
            prevY = curY;

            isFirstInit = false;
        }

        m_alreadyDrawn.push_back(std::make_unique<Pencil>(curX, curY, prevX, prevY,
                                                          drawParams.color.get_red(), drawParams.color.get_green(), drawParams.color.get_blue(),
                                                          drawParams.lineWidth));
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

bool DrawHelper::OnButtonPressed(GdkEventButton *p_event)
{
    m_startX = p_event->x;
    m_startY = p_event->y;

    return true;
}

bool DrawHelper::OnButtonReleased(GdkEventButton *p_event)
{
    if (m_currentShape == Shape::Rectangle)
    {
        m_alreadyDrawn.push_back(std::make_unique<Rectangle>(m_startX, m_startY, m_width, m_height,
                                                             drawParams.color.get_red(), drawParams.color.get_green(), drawParams.color.get_blue(),
                                                             drawParams.lineWidth));
    }
    else if (m_currentShape == Shape::Circle)
    {
        m_alreadyDrawn.push_back(std::make_unique<Circle>(m_startX, m_startY, m_width + m_height,
                                                          drawParams.color.get_red(), drawParams.color.get_green(), drawParams.color.get_blue(),
                                                          drawParams.lineWidth));
    }
    else if (m_currentShape == Shape::Line)
    {
        m_alreadyDrawn.push_back(std::make_unique<Pencil>(curX, curY, prevX, prevY,
                                                          drawParams.color.get_red(), drawParams.color.get_green(), drawParams.color.get_blue(),
                                                          drawParams.lineWidth));
    }

    ClearShapeData();

    return true;
}

void DrawHelper::ClearShapeData()
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

void DrawHelper::SetParentContainer(Gtk::Fixed *drawContainer)
{
    parentContainer = drawContainer;
}

void DrawHelper::ClearDrawBuffer()
{
    m_buffer.reset();
    m_alreadyDrawn.clear();
}

void DrawHelper::SetColor(Gdk::RGBA color)
{
    drawParams.color = color;
}

void DrawHelper::SetLineWidth(double lineWidth)
{
    drawParams.lineWidth = lineWidth;
}

Cairo::RefPtr<Cairo::Context> DrawHelper::GetDrawContext()
{
    return currentContext;
}

int DrawHelper::GetImgOffsetX()
{
    return ImgOffsetX;
}

int DrawHelper::GetImgOffsetY()
{
    return ImgOffsetY;
}