#include "ControlPanel.hxx"
#include "../client/net_modules.hxx"

enum class MsgTypes : uint32_t;

class MainWindow : public Gtk::Window,
                   public ControlPanel
{
public:
    MainWindow();
    void OpenWindow();
    void SetSearchInputFocus(bool canFocus);
    void TurnOnSearchMode();

    Gtk::Stack *GetWindowStack();

private:
    Gtk::Stack *mainWindowStack;

    Glib::RefPtr<Gtk::Builder> uiBuilder;
    Gtk::Grid *grid;

    Gtk::Button *searchBtn;
    Gtk::EventBox *clearInputBtn;

    Gtk::Entry *search_input;

    Gtk::ScrolledWindow *scrolledWindow;
    Glib::RefPtr<Gtk::Adjustment> position;

    Gtk::Label *searchResultsText;
    Gtk::Button *quitSearch;

private:
    void CSSConnection();
    void ProcessWidgets();

    void FillGrid(MsgTypes msgType, net::message<MsgTypes> iMsg = Client::GetInstance().GetEmptyMessage());
    void ClearGrid(Gtk::Grid *grid)
    {
        while (true)
        {
            if (grid->get_child_at(1, 1) != nullptr)
                grid->remove_row(1);
            else
                break;
        }
    };

    bool ScrollModelListView(GdkEventMotion *theEvent, int scrollStep);

    bool OnInputFocusIn(GdkEventFocus *focus);
    void OnInputKeyPress();
    void OnQuitSearchBtnClick();

private:
    bool readyToScroll = false;
    float scrollDelta = 0.f;
    bool isInSearch = false;

public:
    class ModelCard
    {
    private:
        Gtk::Label *modelNameLabel;
        Gtk::Label *dateNameLabel;
        Gtk::Label *modelFolderPathLabel;

    public:
        bool isFavoriteClicked = false;

        Gtk::Button *favoriteBtn;
        Gtk::EventBox *modelBlock;

        std::string modelName;
        std::string dateName;
        std::string modelFolderPath;

    public:
        ModelCard(Gtk::Grid *grid,
                  int column,
                  int row,
                  const char *model_name,
                  const char *category_name,
                  const char *date_name);

        void ProcessModelFavoriteState(Gtk::EventBox *clickedWidget);
        void ChangeFavoriteState(const bool &isFavState, const std::string &isFavImg);
        void MakeFavoriteBtnOn();

        bool OpenInFolderWindow(GdkEventButton *eventData, Gtk::EventBox *clickedWidget);
    };

    static inline std::vector<MainWindow::ModelCard> modelCards;
};