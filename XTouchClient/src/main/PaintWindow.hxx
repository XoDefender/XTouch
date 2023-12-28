#include "ControlPanel.hxx"
#include "DrawHelper.hxx"

class PaintWindow : public Gtk::Window,
                    public ControlPanel
{
public:
    PaintWindow();
    void OpenWindow();

private:
    Gtk::EventBox *colorBtnRed;
    Gtk::EventBox *colorBtnGreen;
    Gtk::EventBox *colorBtnBlue;
    Gtk::EventBox *colorBtnWhite;
    Gtk::EventBox *colorBtnBlack;

    Gtk::EventBox *sizeBtnThin;
    Gtk::EventBox *sizeBtnMedium;
    Gtk::EventBox *sizeBtnBold;
    Gtk::EventBox *sizeBtnBoldPlus;

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

    void SelectDrawColor();

    bool SetDrawSizeBtnActive(GdkEventButton *widget);
    bool SetDrawShapeBtnActive(GdkEventButton *widget);

    bool SetRectBtnActive(GdkEventButton *widget);
    bool SetCircleBtnActive(GdkEventButton *widget);
    bool SetLineBtnActive(GdkEventButton *widget);
    bool SetPencilBtnActive(GdkEventButton *widget);

    bool SetSizeThinBtnActive(GdkEventButton *widget);
    bool SetSizeMediumBtnActive(GdkEventButton *widget);
    bool SetSizeBoldBtnActive(GdkEventButton *widget);
    bool SetSizeBoldPlusBtnActive(GdkEventButton *widget);

    bool OnGoBackBtnClick(GdkEventButton *widget);

    bool OnSaveImgBtnClick(GdkEventButton *widget); 
    bool OnEraseShapesBtnClick(GdkEventButton *widget);

    void ProcessWidgets();
    void ProcessEvents();
    void ConnectCSS();

    void CalculateLastFileIndex();
};