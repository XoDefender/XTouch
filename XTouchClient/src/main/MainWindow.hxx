#include "ControlPanel.hxx"
#include "SortDataManager.hxx"

enum class MsgTypes : uint32_t;

class MainWindow : public Gtk::Window,
                   public ControlPanel,
                   public SortDataManager
{
public:
    MainWindow();
    void OpenWindow();
    void ChangeServerStatus(std::string status);
    void SetSearchInputFocus(bool canFocus);
    void TurnOnSearchMode();

    Gtk::Stack *GetWindowStack();

private:
    Gtk::Stack *mainWindowStack;

    Glib::RefPtr<Gtk::Builder> uiBuilder;
    Gtk::Grid *grid;

    Gtk::Button *openDateSortBtn;
    Gtk::Button *createDateSortBtn;
    Gtk::Button *alphabetSortBtn;

    Gtk::Button *allModelsFilter;
    Gtk::Button *favoriteModelsFilter;

    Gtk::Button *searchBtn;
    Gtk::EventBox *clearInputBtn;

    Gtk::Entry *search_input;

    Gtk::ScrolledWindow *scrolledWindow;
    Glib::RefPtr<Gtk::Adjustment> position;

    // Gtk::Label *serverStatus;
    Gtk::Label *searchResultsText;

    Gtk::Button *quitSearch;
    Gtk::Box *sortContainer;

private:
    void CSSConnection();
    void ProcessWidgets();

    void FillGrid(MsgTypes msgType, net::message<MsgTypes> iMsg = Client::GetInstance().GetEmptyMessage()) override;

    void OnSortBtnClick(bool &isSortAsc,
                        std::string sortAsc,
                        std::string sortDesc);

    void OnModelsFilterClick(MsgTypes msgType, bool isFavFilterState);

    bool ScrollModelListView(GdkEventMotion *theEvent, int scrollStep);

    bool OnInputFocusIn(GdkEventFocus *focus);
    void OnInputKeyPress();
    void OnQuitSearchBtnClick();

private:
    bool isFavoriteFilterClicked = false;

    bool isOpenDateSortAsc = true;
    bool isCreateDateSortAsc = true;
    bool isAlphabetSortAsc = true;

    bool isAnySortButtonClicked = false;

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

        // Если модель не избранная, делаем ее таковой и наоборот
        void ProcessModelFavoriteState(Gtk::EventBox *clickedWidget);
        void ChangeFavoriteState(const MsgTypes &msgType, const bool &isFavState, const std::string &isFavImg);
        void MakeFavoriteBtnOn();

        bool OpenInFolderWindow(GdkEventButton *eventData, Gtk::EventBox *clickedWidget);
    };

    static inline std::vector<MainWindow::ModelCard> modelCards;
};