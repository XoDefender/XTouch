#include "ControlPanel.hxx"
#include "DrawHelper.hxx"

class PaintWindow : public Gtk::Window,
                    public ControlPanel
{
public:
    PaintWindow();
    void OpenWindow();

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
    Gtk::EventBox *drawRectangleBtn;
    Gtk::EventBox *drawCircleBtn;
    Gtk::EventBox *drawLineBtn;
    Gtk::EventBox *drawPencilBtn;

    Gtk::EventBox *saveNewBtn;
    Gtk::EventBox *goBackBtn;
    Gtk::Fixed *container;
    Gtk::Fixed *drawContainer;

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

    int savedFileIndex;

private:
    void SetPassiveToolTypeBtnStyle(Gtk::EventBox *previousBtn,
                                    std::string previousBtnIcon,
                                    std::string previousBtnClass);

    void SetActiveToolTypeBtnStyle(Gtk::EventBox *currentBtn,
                                   std::string currentBtnIcon,
                                   std::string currentBtnClass);

    void SetPassiveToolBtnStyle(Gtk::EventBox *previousBtn,
                                std::string previousBtnIcon,
                                std::string previousBtnClass);

    void SetActiveToolBtnStyle(Gtk::EventBox *currentBtn,
                               std::string currentBtnIcon,
                               std::string currentBtnClass);

    void ProcessWidgets();
    void ConnectCSS();

    void CalculateLastFileIndex();
};