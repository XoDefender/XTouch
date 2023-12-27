#include <iostream>
#include <fstream>

#include "global.hxx"

#include "MainWindow.hxx"
#include "InFolderWindow.hxx"
#include "ModelFileManager.hxx"

using namespace std;

InFolderWindow::FileBlock::FileBlock(Gtk::Widget *widget, std::string fileName, std::string categoryName, std::string dateName)
{
    this->widget = widget;
    this->fileName = fileName;
    this->categoryName = categoryName;
}

void InFolderWindow::CSSConnection()
{
    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, global::inFolderWindowCSS.c_str(), NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                              GTK_STYLE_PROVIDER(cssProvider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void InFolderWindow::CreateFileBlockOnGrid(int row, std::string fileName, std::string categoryName, std::string dateName)
{
    auto ui_builder_temp = Gtk::Builder::create_from_file(global::mainWindowUI);

    ui_builder_temp->get_widget<Gtk::EventBox>("FileBlock", fileBlock);
    ui_builder_temp->get_widget<Gtk::EventBox>("FileTypeIcon", fileTypeIcon);
    ui_builder_temp->get_widget<Gtk::Label>("FileName", this->fileName);
    ui_builder_temp->get_widget<Gtk::Label>("FileCategoryName", this->categoryName);
    ui_builder_temp->get_widget<Gtk::Label>("FileDateName", this->dateName);

    if (!(fileName.find(".txt") != string::npos))
    {
        Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
        string favButtonStyle = ".FileTypeIcon {background: url('../src/Windows/MainWindow/InFolderScreen/Img/fileIcon3D.jpeg') no-repeat center;}";

        css_provider->load_from_data(favButtonStyle);
        fileTypeIcon->get_style_context()
            ->add_provider(
                css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

        categoryName = "3D Модель";
    }
    else
    {
        Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
        string favButtonStyle = ".FileTypeIcon {background: url('../src/Windows/MainWindow/InFolderScreen/Img/textIcon.png') no-repeat center;}";

        css_provider->load_from_data(favButtonStyle);
        fileTypeIcon->get_style_context()
            ->add_provider(
                css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

        categoryName = "Текст";
    }

    this->fileName->set_text(fileName);
    this->categoryName->set_text(categoryName);
    // this->dateName->set_text(dateName);

    fileBlock->set_events(Gdk::BUTTON_PRESS_MASK);
    fileBlock->signal_button_press_event().connect(sigc::bind(sigc::mem_fun(*this, &InFolderWindow::OnFileBlockClick), fileBlock));

    FileBlock file(fileBlock, fileName, categoryName, dateName);

    files.push_back(file);

    grid->attach(*fileBlock, 1, row, 1, 1);
}

bool InFolderWindow::OpenSelectedFile()
{
    modelFileManager->ProcessFile();
    return false;
}

void InFolderWindow::StartDownloadAnimation()
{
    downloadRevealer->set_reveal_child(true);
    animationIter = 0;

    downloadAnimationConn = Glib::signal_timeout().connect([this]()
                                                           {
        if(animationIter == downloadAnimImages.size() - 1)
            animationIter = 0;

        downloadImage->set(downloadAnimImages[animationIter]);

        animationIter++;

        return true; },
                                                           50);
}

bool InFolderWindow::OnFileBlockClick(GdkEventButton *widget, Gtk::EventBox *clickedWidget)
{
    StartDownloadAnimation();

    for (int i = 0; i < files.size(); i++)
    {
        if (files[i].widget == (Gtk::Widget *)clickedWidget)
        {
            global::currentFileName = files[i].fileName;

            files[i].widget->set_sensitive(false);

            clickedFileBlock = files[i].widget;

            Glib::RefPtr<Gtk::StyleContext> context;
            context = clickedFileBlock->get_style_context();
            context->add_class("ClickedFileBlock");

            Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
            css_provider->load_from_data(".ClickedFileBlock {border: 5px solid red;}");
            context->add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

            Glib::signal_timeout().connect(sigc::mem_fun(*this, &InFolderWindow::OpenSelectedFile), 2700);
        }
    }

    ifstream file;

    if (global::testModelFolder != "nullptr")
    {
        file.open(global::testModelFolder);
    }
    else
    {
        ostringstream relativeFilePathStructure;

        if (!(global::currentFileName.find(".txt") != string::npos))
            relativeFilePathStructure << global::saveModelPath << ToXbfFileExtension(global::currentFileName) << "";
        else
            relativeFilePathStructure << global::saveModelPath << global::currentFileName << "";

        string relativeFilePath = relativeFilePathStructure.str();
        file.open(relativeFilePath);
    }

    if (!file)
        downloadStatus->set_text("Файл загружается...");
    else
        downloadStatus->set_text("Файл открывается...");

    return true;
}

void InFolderWindow::ResetFileBlockSelection(string downloadStatusText)
{
    Glib::RefPtr<Gtk::StyleContext> context;
    context = clickedFileBlock->get_style_context();
    context->add_class("ClickedFileBlock");

    Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
    css_provider->load_from_data(".ClickedFileBlock {border: 0px solid red;}");
    context->add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

    downloadStatus->set_text(downloadStatusText);

    clickedFileBlock->set_sensitive(true);

    downloadRevealer->set_reveal_child(false);
    downloadAnimationConn.disconnect();
}

void InFolderWindow::FillGrid(MsgTypes msgType, net::message<MsgTypes> iMsg)
{
    fileNames.clear();
    ClearGrid(grid);

    net::message<MsgTypes> oMsg;
    oMsg = Client::GetInstance().SendRequestToServer(msgType, iMsg);

    char rowAmountString[1024];
    oMsg >> rowAmountString;
    int rowAmountInt = atoi(rowAmountString);

    for (int row = 1; row < rowAmountInt + 1; row++)
    {
        if (oMsg.size() <= 0)
            break;

        grid->insert_row(row);

        char fileName[1024];
        char categoryName[1024];
        char dateName[1024];
        char isFavorite[1024];

        if (msgType != MsgTypes::GetModelFiles)
        {
            if (oMsg.size() <= 0)
                break;

            oMsg >> isFavorite >> dateName >> categoryName >> fileName;

            CreateFileBlockOnGrid(row, fileName, categoryName, dateName);
        }
        else
        {
            oMsg >> fileName;

            fileNames.push_back(fileName);
            CreateFileBlockOnGrid(row, fileName, categoryName, dateName);
        }
    }
}

string InFolderWindow::ToXbfFileExtension(string fileName)
{
    string fileNameWithoutExtension = fileName.substr(0, fileName.find("."));
    return (fileNameWithoutExtension + ".xbf");
}

bool InFolderWindow::OnGoBackBtnClick(GdkEventButton *theEvent)
{
    mainWindow->GetWindowStack()->set_visible_child("MainScreen");
    mainWindow->SetSearchInputFocus(true);

    return true;
}

void InFolderWindow::ProcessWidgets()
{
    uiBuilder = Gtk::Builder::create_from_file(global::mainWindowUI);

    uiBuilder->get_widget<Gtk::Box>("InFolderScreen", inFolderScreen);
    uiBuilder->get_widget<Gtk::Grid>("GridForFiles", grid);
    uiBuilder->get_widget<Gtk::Label>("MainFileName", fileNameInHeader);
    uiBuilder->get_widget<Gtk::Label>("DownloadStatus", downloadStatus);
    uiBuilder->get_widget<Gtk::EventBox>("BackBtn", goBackBtn);
    uiBuilder->get_widget<Gtk::Revealer>("DownloadRevealer", downloadRevealer);
    uiBuilder->get_widget<Gtk::Image>("DownloadImage", downloadImage);

    goBackBtn->signal_button_press_event().connect(sigc::mem_fun(*this, &InFolderWindow::OnGoBackBtnClick), false);

    ProcessPanel(uiBuilder, mainWindow, "InFolderScreen");
}

void InFolderWindow::CacheDownloadAnimationFrames(int framesAmount)
{
    for (int i = 1; i < framesAmount + 1; i++)
    {
        string imgName = to_string(i) + ".png";
        string path = "../../../XTouchClient/res/Windows/MainWindow/InFolderScreen/Img/DownloadAnimation/" + imgName;
        Glib::RefPtr<Gdk::Pixbuf> img = Gdk::Pixbuf::create_from_file(path);
        downloadAnimImages.push_back(img);
    }
}

// после срабатывания данного конструктора, можно снова нажать на файл и открыть новое окно
InFolderWindow::InFolderWindow()
{
    ProcessWidgets();
    CSSConnection();

    mainWindow->GetWindowStack()->add(*inFolderScreen, "InFolderScreen");
    m_headerBar.set_show_close_button(true);
    mainWindow->set_titlebar(m_headerBar);

    CacheDownloadAnimationFrames(60);
}

void InFolderWindow::OpenWindow()
{
    net::message<MsgTypes> iMsg;
    iMsg << global::currentModelName.c_str();

    FillGrid(MsgTypes::GetModelFiles, iMsg);

    downloadStatus->set_text("");
    fileNameInHeader->set_text(global::currentModelName);
    mainWindow->GetWindowStack()->set_visible_child("InFolderScreen");
}
