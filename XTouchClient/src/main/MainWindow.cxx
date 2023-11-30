#include <iostream>

#include "global.hxx"

#include "MainWindow.hxx"
#include "InFolderWindow.hxx"

using namespace std;

MainWindow::ModelCard::ModelCard(Gtk::Grid *grid,
                                 int column,
                                 int row,
                                 const char *model_name,
                                 const char *category_name,
                                 const char *date_name)
{
    auto ui_builder_temp = Gtk::Builder::create_from_file(global::mainWindowUI);

    ui_builder_temp->get_widget<Gtk::Label>("ModelName", modelNameLabel);
    ui_builder_temp->get_widget<Gtk::Label>("CategoryName", modelFolderPathLabel);
    ui_builder_temp->get_widget<Gtk::Label>("DateName", dateNameLabel);
    ui_builder_temp->get_widget<Gtk::Button>("FavoriteBtn", favoriteBtn);
    ui_builder_temp->get_widget<Gtk::EventBox>("ModelBlock", modelBlock);

    modelNameLabel->set_text(model_name);
    dateNameLabel->set_text(date_name);
    modelFolderPathLabel->set_text(category_name);

    // сохраняем данные о сформированном блоке, чтобы потом достать их через объект
    modelName = model_name;
    dateName = date_name;
    modelFolderPath = category_name;

    modelBlock->set_events(Gdk::BUTTON_PRESS_MASK);
    modelBlock->signal_button_press_event().connect(sigc::bind(sigc::mem_fun(*this, &ModelCard::OpenInFolderWindow), modelBlock));
    favoriteBtn->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &ModelCard::ProcessModelFavoriteState), modelBlock));

    grid->attach(*modelBlock, column, row);
}

void MainWindow::ModelCard::MakeFavoriteBtnOn()
{
    Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
    string favButtonStyle = ".StarButton {background: url('../src/Windows/MainWindow/MainScreen/Img/star2.png') no-repeat center;\
                                                outline: none;\
                                                border: none;\
                                                margin-right: 10px;}";

    css_provider->load_from_data(favButtonStyle);
    favoriteBtn->get_style_context()
        ->add_provider(
            css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

    isFavoriteClicked = true;
}

Gtk::Stack *MainWindow::GetWindowStack()
{
    return mainWindowStack;
}

void MainWindow::ModelCard::ChangeFavoriteState(const MsgTypes &msgType, const bool &isFavState, const string &isFavImg)
{
    try
    {
        net::message<MsgTypes> iMsg;
        iMsg << modelName.c_str();
        net::message<MsgTypes> oMsg = Client::GetInstance().SendRequestToServer(msgType, iMsg);
    }
    catch (...)
    {
        // mainWindow->ChangeServerStatus("Сервер недоступен");
        return;
    }

    Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
    string favButtonStyle = ".StarButton {background: url('../src/Windows/MainWindow/MainScreen/Img/" + isFavImg + string("') no-repeat center;\
                                                outline: none;\
                                                border: none;\
                                                margin-right: 10px;}");

    css_provider->load_from_data(favButtonStyle);
    favoriteBtn->get_style_context()
        ->add_provider(
            css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

    isFavoriteClicked = isFavState;
}

// Если модель не избранная, делаем ее таковой и наоборот
void MainWindow::ModelCard::ProcessModelFavoriteState(Gtk::EventBox *clickedWidget)
{
    for (int i = 0; i < MainWindow::modelCards.size(); i++)
    {
        if (MainWindow::modelCards[i].modelBlock != clickedWidget)
            continue;

        if (!MainWindow::modelCards[i].isFavoriteClicked)
            MainWindow::modelCards[i].ChangeFavoriteState(MsgTypes::AddModelToFavorite, true, "star2.png");
        else
            MainWindow::modelCards[i].ChangeFavoriteState(MsgTypes::RemoveModelFromFavorite, false, "star1.png");
    }
}

bool MainWindow::ModelCard::OpenInFolderWindow(GdkEventButton *eventData, Gtk::EventBox *clickedWidget)
{
    for (int i = 0; i < MainWindow::modelCards.size(); i++)
    {
        if (MainWindow::modelCards[i].modelBlock == (Gtk::Widget *)clickedWidget)
        {
            // получаем данные о модели, на которую кликнули
            global::currentModelName = MainWindow::modelCards[i].modelName;
            global::currentModelFolder = MainWindow::modelCards[i].modelFolderPath;
        }
    }

    mainWindow->SetSearchInputFocus(false);
    inFolderWindow->OpenWindow();

    return true;
}

void MainWindow::ChangeServerStatus(string status)
{
    // serverStatus->set_text(status);
}

bool MainWindow::OnInputFocusIn(GdkEventFocus *focus)
{
    if (!isInSearch)
    {
        quitSearch->show();
        sortContainer->hide();
        searchResultsText->show();
        clearInputBtn->show();

        ClearGrid(grid);
    }

    isInSearch = true;

    return true;
}

void MainWindow::OnInputKeyPress()
{
    if (search_input->get_text().size() >= 3)
    {
        ClearGrid(grid);

        net::message<MsgTypes> iMsg;
        iMsg << search_input->get_text().c_str();

        FillGrid(MsgTypes::GetModelByName, iMsg);
    }
}

void MainWindow::OnQuitSearchBtnClick()
{
    FillGrid(MsgTypes::GetModels);

    quitSearch->hide();
    searchResultsText->hide();
    sortContainer->show();
    clearInputBtn->hide();

    search_input->set_text("");

    isInSearch = false;
}

void MainWindow::FillGrid(MsgTypes msgType, net::message<MsgTypes> iMsg)
{
    if (!isServerActive) return;

    ClearGrid(grid);

    MainWindow::modelCards.clear();

    net::message<MsgTypes> oMsg = Client::GetInstance().SendRequestToServer(msgType, iMsg);

    int columns = 0;
    int rows = 3;

    char modelsAmount[1024];

    oMsg >> modelsAmount;

    double modelsAmountDouble = (double)atoi(modelsAmount);

    columns = (int)ceil(modelsAmountDouble / 2.0);

    grid->insert_row(1);
    grid->insert_row(2);

    for (int column = 1; column < columns + 1; column++)
    {
        grid->insert_column(column);

        for (int row = 1; row < rows + 1; row++)
        {
            char fileName[1024];
            char categoryName[1024];
            char dateName[1024];
            char isFavorite[1024];

            if (oMsg.size() <= 0)
                break;

            oMsg >> isFavorite >> dateName >> categoryName >> fileName;

            // создаем объект модели на основе данных, полученных с БД
            ModelCard model(grid, column, row, fileName, categoryName, dateName);

            string isFavoriteStr = isFavorite;
            if (isFavoriteStr == "true")
                model.MakeFavoriteBtnOn();

            // засовываем сформированный блок в массив, чтобы при клике идентифицировать нужный блок
            MainWindow::modelCards.push_back(model);
        }
    }

    Glib::signal_timeout().connect([this]()
                                   { isAnySortButtonClicked = false; return false; },
                                   100);
}

void MainWindow::CSSConnection()
{
    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, global::mainWindowCSS.c_str(), NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                              GTK_STYLE_PROVIDER(cssProvider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void MainWindow::OnSortBtnClick(bool &isSortAsc, string sortAsc, string sortDesc)
{
    if (isAnySortButtonClicked)
        return;

    isAnySortButtonClicked = true;

    if (!isFavoriteFilterClicked)
        SortData(MsgTypes::SortModels, isSortAsc, sortAsc, sortDesc);
    else
        SortData(MsgTypes::SortFavoriteModels, isSortAsc, sortAsc, sortDesc);
}

void MainWindow::OnModelsFilterClick(MsgTypes msgType, bool isFavFilterState)
{
    if (isAnySortButtonClicked)
        return;

    isAnySortButtonClicked = true;

    FillGrid(msgType);

    isFavoriteFilterClicked = isFavFilterState;
}

bool MainWindow::ScrollModelListView(GdkEventMotion *theEvent, int scrollStep)
{
    if (!readyToScroll)
        return false;

    if (theEvent->x >= scrollDelta && position->get_value() < position->get_upper())
        position->set_value(position->get_value() + scrollStep);
    else if (position->get_value() > position->get_lower())
        position->set_value(position->get_value() - scrollStep);

    scrollDelta = theEvent->x;

    return true;
}

void MainWindow::TurnOnSearchMode()
{
    if (isInSearch)
    {
        quitSearch->show();
        sortContainer->hide();
        searchResultsText->show();
        clearInputBtn->show();
    }
}

void MainWindow::ProcessWidgets()
{
    uiBuilder = Gtk::Builder::create_from_file(global::mainWindowUI);

    uiBuilder->get_widget<Gtk::Stack>("MainWindowStack", mainWindowStack);
    uiBuilder->get_widget<Gtk::Grid>("GridForModels", grid);
    uiBuilder->get_widget<Gtk::ScrolledWindow>("ScrolledWindow", scrolledWindow);
    uiBuilder->get_widget<Gtk::Entry>("SearchInput", search_input);
    uiBuilder->get_widget<Gtk::Button>("SortRecent", openDateSortBtn);
    uiBuilder->get_widget<Gtk::Button>("SortDate", createDateSortBtn);
    uiBuilder->get_widget<Gtk::Button>("SortAlphabet", alphabetSortBtn);
    uiBuilder->get_widget<Gtk::Button>("AllModelsFilter", allModelsFilter);
    uiBuilder->get_widget<Gtk::Button>("FavoriteModelsFilter", favoriteModelsFilter);
    // uiBuilder->get_widget<Gtk::Label>("ServerStatus", serverStatus);
    uiBuilder->get_widget<Gtk::Button>("QuitSearch", quitSearch);
    uiBuilder->get_widget<Gtk::Box>("SortContainer", sortContainer);
    uiBuilder->get_widget<Gtk::Label>("SearchResultsText", searchResultsText);
    uiBuilder->get_widget<Gtk::EventBox>("ClearInputBtn", clearInputBtn);

    Glib::RefPtr<Glib::Object> adjustmentObject = uiBuilder->get_object("ScrollAdj");
    position = Glib::RefPtr<Gtk::Adjustment>::cast_dynamic(adjustmentObject);

    grid->set_column_spacing(20);
    grid->set_row_spacing(30);

    SetSearchInputFocus(false);
    search_input->signal_focus_in_event().connect(sigc::mem_fun(*this, &MainWindow::OnInputFocusIn), false);

    search_input->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::OnInputKeyPress), false);

    clearInputBtn->signal_button_press_event().connect([this](GdkEventButton *)
                                                       { search_input->set_text(""); return true; });

    quitSearch->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::OnQuitSearchBtnClick), false);

    openDateSortBtn->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::OnSortBtnClick), isOpenDateSortAsc,
                                                         "create_date asc",
                                                         "create_date desc"));

    createDateSortBtn->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::OnSortBtnClick), isCreateDateSortAsc,
                                                           "create_date asc",
                                                           "create_date desc"));

    alphabetSortBtn->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::OnSortBtnClick), isAlphabetSortAsc,
                                                         "model_name asc",
                                                         "model_name desc"));

    allModelsFilter->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::OnModelsFilterClick),
                                                         MsgTypes::GetModels,
                                                         false));

    favoriteModelsFilter->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::OnModelsFilterClick),
                                                              MsgTypes::GetFavoriteModels,
                                                              true));

    scrolledWindow->add_events(Gdk::POINTER_MOTION_MASK |
                               Gdk::BUTTON_PRESS_MASK |
                               Gdk::BUTTON_RELEASE_MASK |
                               Gdk::SMOOTH_SCROLL_MASK |
                               Gdk::FOCUS_CHANGE_MASK);

    scrolledWindow->signal_motion_notify_event().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::ScrollModelListView), 10));

    scrolledWindow->signal_button_press_event().connect([this](GdkEventButton *theEvent)
                                                        { readyToScroll = true; 
                                                          return true; });

    scrolledWindow->signal_button_release_event().connect([this](GdkEventButton *theEvent)
                                                          { readyToScroll = false; 
                                                            return true; });

    signal_show().connect([this]()
                          { quitSearch->hide(); searchResultsText->hide(); clearInputBtn->hide(); });

    ProcessPanel(uiBuilder, this, "MainScreen");

    add(*mainWindowStack);
}

void MainWindow::SetSearchInputFocus(bool canFocus)
{
    search_input->set_property("can-focus", canFocus);
}

MainWindow::MainWindow()
{
    ProcessWidgets();
    CSSConnection();
}

void MainWindow::OpenWindow()
{
    FillGrid(MsgTypes::GetModels);
    ShowWindow(this);
    SetSearchInputFocus(true);
}