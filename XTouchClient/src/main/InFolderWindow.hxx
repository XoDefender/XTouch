#include "ControlPanel.hxx"
#include "../client/net_modules.hxx"

enum class MsgTypes : uint32_t;

class InFolderWindow : public ControlPanel
{
public:
    Gtk::Label *downloadStatus;

public:
    InFolderWindow();
    void OpenWindow();
    void StopDownloadAnimation();
    std::string ConvertExtensionToXbf(std::string fileName);

private:
    Glib::RefPtr<Gtk::Builder> uiBuilder;
    Gtk::Grid *grid;

    Gtk::Label *fileNameInHeader;
    Gtk::Label *fileName;
    Gtk::Label *categoryName;
    Gtk::Label *dateName;
    Gtk::EventBox *fileBlock;
    Gtk::EventBox *fileTypeIcon;

    Gtk::EventBox *goBackBtn;
    Gtk::Box *inFolderScreen;

    Gtk::Widget *clickedFileBlock;

    Gtk::Revealer* downloadRevealer;
    Gtk::Image* downloadImage;

    Gtk::HeaderBar m_headerBar;

private:
    void CSSConnection();
    void ProcessWidgets();

    void FillGrid(MsgTypes msgType, net::message<MsgTypes> iMsg = Client::GetInstance().GetEmptyMessage());
    void ClearGrid(Gtk::Grid *grid);

    void CreateFileBlockOnGrid(int row, std::string fileName, std::string categoryName, std::string dateName);
    bool OnFileBlockClick(GdkEventButton *widget, Gtk::EventBox *clickedWidget);

    void OnUpdateFilesBtnClick();
    bool OnGoBackBtnClick(GdkEventButton *theEvent);

    bool OpenSelectedFile();

    void StartDownloadAnimation();
    bool ChangeDownloadAnimationImgs();
    void CacheDownloadAnimationFrames(int framesAmount);

private:
    std::string pathToFile;

    std::vector<std::string> fileNames;

    int animationIter = 0;
    sigc::connection downloadAnimationConn;

    std::vector<Glib::RefPtr<Gdk::Pixbuf>> downloadAnimImages;

private:
    class FileBlock
    {
    public:
        FileBlock(Gtk::Widget *widget, std::string fileName, std::string categoryName, std::string dateName);

    public:
        Gtk::Widget *widget;
        std::string fileName;
        std::string categoryName;
        std::string dateName;
    };

    std::vector<FileBlock> files;
};