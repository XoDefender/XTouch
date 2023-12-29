#include <iostream>

#include "global.hxx"

#include "MainWindow.hxx"
#include "InFolderWindow.hxx"
#include "../client/Client.hxx"

MainWindow::ModelCard::ModelCard(Gtk::Grid *grid,
                                 int column,
                                 int row,
                                 const char *modelName,
                                 const char *modelFolderPath,
                                 const char *modelDate,
                                 bool isFav) : modelName(modelName), modelFolderPath(modelFolderPath), modelDate(modelDate)
{
    auto uiBuilderModelCard = Gtk::Builder::create_from_file(global::mainWindowUI);

    uiBuilderModelCard->get_widget<Gtk::Label>("ModelName", modelNameLabel);
    uiBuilderModelCard->get_widget<Gtk::Label>("CategoryName", modelFolderPathLabel);
    uiBuilderModelCard->get_widget<Gtk::Label>("DateName", dateNameLabel);
    uiBuilderModelCard->get_widget<Gtk::Button>("FavoriteBtn", favoriteBtn);
    uiBuilderModelCard->get_widget<Gtk::EventBox>("ModelBlock", modelBlock);

    modelNameLabel->set_text(modelName);
    dateNameLabel->set_text(modelDate);
    modelFolderPathLabel->set_text(modelFolderPath);

    modelBlock->set_events(Gdk::BUTTON_PRESS_MASK);
    modelBlock->signal_button_press_event().connect(sigc::bind(sigc::mem_fun(*this, &ModelCard::OpenInFolderWindow), modelBlock));
    favoriteBtn->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &ModelCard::ProcessModelFavoriteState), modelBlock));

    if (isFav)
        MakeFavoriteBtnOn();

    grid->attach(*modelBlock, column, row);
}

void MainWindow::ModelCard::MakeFavoriteBtnOn()
{
    Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
    std::string favButtonStyle = ".StarButton {background: url('../../../XTouchClient/res/Windows/MainWindow/MainScreen/Img/starFav.png') no-repeat center;\
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

void MainWindow::ModelCard::ChangeFavoriteState(const bool &isFavState, const std::string &isFavImg)
{
    std::string action = isFavState ? "add" : "remove";

    net::message<MsgTypes> iMsg;
    iMsg << action.data() << modelName.c_str();
    net::message<MsgTypes> oMsg = Client::GetInstance().SendRequestToServer(MsgTypes::ChangeModelFavState, iMsg);

    Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
    std::string favButtonStyle = ".StarButton {background: url('../../../XTouchClient/res/Windows/MainWindow/MainScreen/Img/" + isFavImg + std::string("') no-repeat center;\
                                                outline: none;\
                                                border: none;\
                                                margin-right: 10px;}");

    css_provider->load_from_data(favButtonStyle);
    favoriteBtn->get_style_context()->add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

    isFavoriteClicked = isFavState;
}

void MainWindow::ModelCard::ProcessModelFavoriteState(Gtk::EventBox *clickedWidget)
{
    for (int i = 0; i < modelCards.size(); i++)
    {
        if (modelCards[i].modelBlock != clickedWidget)
            continue;

        std::string favStateIconName = modelCards[i].isFavoriteClicked ? "starNotFav.png" : "starFav.png";
        modelCards[i].ChangeFavoriteState(!modelCards[i].isFavoriteClicked, favStateIconName);

        for (auto &data : modelData)
        {
            if (data.modelName == modelCards[i].modelName)
                data.isFav = modelCards[i].isFavoriteClicked;
        }
    }
}

bool MainWindow::ModelCard::OpenInFolderWindow(GdkEventButton *eventData, Gtk::EventBox *clickedWidget)
{
    for (int i = 0; i < modelCards.size(); i++)
    {
        if (modelCards[i].modelBlock == (Gtk::Widget *)clickedWidget)
        {
            global::currentModelName = modelCards[i].modelName;
            global::currentModelFolder = modelCards[i].modelFolderPath;
        }
    }

    mainWindow->SetSearchInputFocus(false);
    inFolderWindow->OpenWindow();

    return true;
}

bool MainWindow::OnInputFocusIn(GdkEventFocus *focus)
{
    if (!isInSearch)
    {
        quitSearch->show();
        searchResultsText->show();
        clearInputBtn->show();

        ClearGrid();
    }

    isInSearch = true;

    return true;
}

void MainWindow::OnQuitSearchBtnClick()
{
    quitSearch->hide();
    searchResultsText->hide();
    clearInputBtn->hide();

    search_input->set_text("");

    isInSearch = false;

    FillGrid(MsgTypes::GetModels);
}

void MainWindow::ClearGrid()
{
    while (true)
    {
        if (grid->get_child_at(1, 1) != nullptr)
            grid->remove_row(1);
        else
            break;
    }

    modelCards.clear();
}

void MainWindow::OnInputKeyPress()
{
    if (search_input->get_text() == "")
    {
        ClearGrid();
        return;
    }

    std::vector<ModelData> foundData;
    for (auto data : modelData)
    {
        if (data.modelName.find(search_input->get_text()) != std::string::npos)
        {
            foundData.push_back(data);
        }
    }

    FillGrid(foundData);
}

void MainWindow::FillGrid(std::vector<ModelData> modelData)
{
    ClearGrid();

    int columns = (int)ceil((double)modelData.size() / 2.0);
    int rows = 3;
    int currColumn = 1;
    int currRow = 1;

    grid->insert_column(currColumn);
    grid->insert_row(currRow);

    for (const auto &data : modelData)
    {
        if (currRow > rows)
        {
            currColumn++;
            currRow = 1;
            grid->insert_column(currColumn);
        }

        ModelCard model(grid, currColumn, currRow,
                        data.modelName.c_str(),
                        data.modelPath.c_str(),
                        data.modelCreateDate.c_str(),
                        data.isFav);

        modelCards.push_back(model);

        currRow++;
        grid->insert_row(currRow);
    }
}

void MainWindow::FillGrid(MsgTypes msgType, net::message<MsgTypes> iMsg)
{
    ClearGrid();

    modelData.clear();

    net::message<MsgTypes> oMsg = Client::GetInstance().SendRequestToServer(msgType, iMsg);

    char modelsAmount[1024];
    oMsg >> modelsAmount;

    int rows = 3;
    int currColumn = 1;
    int currRow = 1;

    grid->insert_column(currColumn);
    grid->insert_row(currRow);

    for (int i = 0; i < atoi(modelsAmount); i++)
    {
        if (currRow > rows)
        {
            currColumn++;
            currRow = 1;
            grid->insert_column(currColumn);
        }

        if (oMsg.size() <= 0)
            break;

        char modelName[1024];
        char modelPath[1024];
        char modelDate[1024];
        char isFavorite[1024];

        oMsg >> isFavorite >> modelDate >> modelPath >> modelName;
        bool isFav = !strcmp(isFavorite, "true") ? true : false;

        ModelCard model(grid, currColumn, currRow, modelName, modelPath, modelDate, isFav);
        ModelData data(modelName, modelDate, modelPath, isFav);

        modelCards.push_back(model);
        modelData.push_back(data);

        currRow++;
        grid->insert_row(currRow);
    }
}

void MainWindow::CSSConnection()
{
    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, global::mainWindowCSS.c_str(), NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                              GTK_STYLE_PROVIDER(cssProvider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER);
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
    uiBuilder->get_widget<Gtk::Button>("QuitSearch", quitSearch);
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