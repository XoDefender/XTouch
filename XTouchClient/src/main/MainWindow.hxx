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

    void ClearGrid();

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
        std::string modelDate;
        std::string modelFolderPath;

    public:
        ModelCard(Gtk::Grid *grid,
                  int column,
                  int row,
                  const char *modelName,
                  const char *modelFolderPath,
                  const char *modelDate,
                  bool isFav);

        void ProcessModelFavoriteState(Gtk::EventBox *clickedWidget);
        void ChangeFavoriteState(const bool &isFavState, const std::string &isFavImg);
        void MakeFavoriteBtnOn();

        bool OpenInFolderWindow(GdkEventButton *eventData, Gtk::EventBox *clickedWidget);
    };

private:
    struct ModelData
    {
        ModelData(std::string modelName, std::string modelCreateDate, std::string modelPath, int isFav) :
                 modelName(modelName), modelCreateDate(modelCreateDate), modelPath(modelPath), isFav(isFav){};;

        std::string modelName;
        std::string modelCreateDate;
        std::string modelPath;
        bool isFav = false;
    };

    void FillGrid(std::vector<ModelData> modelData);
    static inline std::vector<MainWindow::ModelData> modelData;
    static inline std::vector<MainWindow::ModelCard> modelCards;
};