#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "global.hxx"
#include "Client.hxx"

#include "AdminWindow.hxx"

using namespace sql;
using namespace std;

// Нужно добавить обработку на сервере
void AdminWindow::AddUser()
{
    // if (loginInput->get_text() != "" && passwordInput->get_text() != "")
    // {
    //     olc::net::message<MsgTypes> iMsg;
    //     iMsg << loginInput->get_text().c_str() << passwordInput->get_text().c_str();
    //     olc::net::message<MsgTypes> oMsg = Client::GetInstance().SendRequestToServer(MsgTypes::AddUserToTable, iMsg);
    //     cout << oMsg.body.data() << endl;
    // }
}

void AdminWindow::ProcessWidgets()
{
    uiBuilder = Gtk::Builder::create_from_file(global::adminWindowUI);

    uiBuilder->get_widget<Gtk::Entry>("ID", loginInput);
    uiBuilder->get_widget<Gtk::Entry>("Password", passwordInput);
    uiBuilder->get_widget<Gtk::Window>("Window", window);
    uiBuilder->get_widget<Gtk::Button>("AddUserButton", addUserBtn);

    addUserBtn->signal_clicked().connect(sigc::mem_fun(*this, &AdminWindow::AddUser), false);
}

AdminWindow::AdminWindow()
{
    ProcessWidgets();
}

void AdminWindow::OpenWindow()
{
    window->show_all();
    global::app->add_window(*window);
}