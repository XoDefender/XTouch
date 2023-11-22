#include "ControlPanel.hxx"
#include "SortDataManager.hxx"

enum class MsgTypes : uint32_t;

class InFolderWindow : public ControlPanel,
                       public SortDataManager
{
public:
    Gtk::Label *downloadStatus;

public:
    InFolderWindow();
    void OpenWindow();
    void ResetFileBlockSelection(std::string downloadStatusText = "");
    std::string ToXbfFileExtension(std::string fileName);

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
    Gtk::Button *openDateSortBtn;
    Gtk::Button *createDateSortBtn;
    Gtk::Button *alphabetSortBtn;
    Gtk::Button *updateFilesBtn;
    Gtk::Box *inFolderScreen;

    Gtk::Widget *clickedFileBlock;

    Gtk::Revealer* downloadRevealer;
    Gtk::Image* downloadImage;

    Gtk::HeaderBar m_headerBar;

private:
    void CSSConnection();
    void ProcessWidgets();

    void FillGrid(MsgTypes msgType, olc::net::message<MsgTypes> iMsg = Client::GetInstance().GetEmptyMessage()) override;

    void CreateFileBlockOnGrid(int row, std::string fileName, std::string categoryName, std::string dateName);
    bool OnFileBlockClick(GdkEventButton *widget, Gtk::EventBox *clickedWidget);

    void OnSortBtnClick(bool &isSortAsc,
                        std::string sortAsc,
                        std::string sortDesc);

    void OnUpdateFilesBtnClick();
    bool OnGoBackBtnClick(GdkEventButton *theEvent);

    bool OpenSelectedFile();

    void StartDownloadAnimation();
    void CacheDownloadAnimationFrames(int framesAmount);

private:
    std::string pathToFile;

    std::vector<std::string> fileNames;

    bool isOpenDateSortAsc = true;
    bool isCreateDateSortAsc = true;
    bool isAlphabetSortAsc = true;

    bool isAnyFileBlockClicked = false;
    bool isAnySortButtonClicked = false;

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