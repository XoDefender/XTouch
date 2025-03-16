#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "global.hxx"
#include "Client.hxx"

#include "AdminWindow.hxx"

using namespace std;

void AdminWindow::ProcessUser(ProcessUserActions action)
{
    if (loginInput->get_text() != "" && passwordInput->get_text() != "")
    {
        std::hash<std::string> hash;
        std::string passwdHash = to_string(hash(passwordInput->get_text()));
        std::string login = loginInput->get_text();

        net::message<MsgTypes> iMsg;
        iMsg << login.c_str() << passwdHash.c_str();

        MsgTypes type;
        if (action == ProcessUserActions::AddUser) {
            type = MsgTypes::AddUser;
        }
        else {
           type = MsgTypes::RemoveUser;
        }

        net::message<MsgTypes> oMsg = Client::GetInstance().SendRequestToServer(type, iMsg);
        if(oMsg.header.id == MsgTypes::ServerAccept) {
            status->set_text("Operation succeeded");
        }
        else if (oMsg.header.id == MsgTypes::ServerDeny) {
            status->set_text("Error");
        }
    }
}

void AdminWindow::ProcessWidgets()
{
    uiBuilder = Gtk::Builder::create_from_file(global::adminWindowUI);

    uiBuilder->get_widget<Gtk::Entry>("ID", loginInput);
    uiBuilder->get_widget<Gtk::Entry>("Password", passwordInput);
    uiBuilder->get_widget<Gtk::Window>("Window", window);
    uiBuilder->get_widget<Gtk::Button>("AddUserButton", addUserBtn);
    uiBuilder->get_widget<Gtk::Button>("RemoveUserButton", removeUserBtn);
    uiBuilder->get_widget<Gtk::Label>("Status", status);

    addUserBtn->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &AdminWindow::ProcessUser), ProcessUserActions::AddUser));
    removeUserBtn->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &AdminWindow::ProcessUser), ProcessUserActions::RemoveUser));
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