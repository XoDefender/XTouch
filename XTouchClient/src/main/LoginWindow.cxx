#include <iostream>
#include <sstream>
#include <fstream>

#include "global.hxx"
#include "Client.hxx"

#include "LoginWindow.hxx"
#include "MainWindow.hxx"
#include "AdminWindow.hxx"

using namespace std;

void LoginWindow::LoginUser()
{
    net::message<MsgTypes> iMsg;
    iMsg << global::currentUserName.c_str() << global::currentUserHash.c_str();
    net::message<MsgTypes> oMsg = Client::GetInstance().SendRequestToServer(MsgTypes::PasswordLogin, iMsg);

    if (oMsg.header.id == MsgTypes::ServerAccept)
    {
        window->hide();
        mainWindow->OpenWindow();
    }
    else if (oMsg.header.id == MsgTypes::ServerDeny)
    {
        loginStatus->set_text("Incorrect login or password");
    }
}

bool LoginWindow::ProcessLoginInput()
{
    if (passwordInput->get_text() == "" || loginInput->get_text() == "")
    {
        loginStatus->set_text("Incorrect login or password");
        return false;
    }

    global::currentUserName = loginInput->get_text();
    std::hash<std::string> hash;
    global::currentUserHash = to_string(hash(passwordInput->get_text()));

    return true;
}

void LoginWindow::OnLoginClick()
{
    if(ProcessLoginInput())
        LoginUser();
}

void LoginWindow::ProcessWidgets()
{
    uiBuilder = Gtk::Builder::create_from_file(global::loginWindowUI);

    uiBuilder->get_widget<Gtk::Window>("Window", window);
    uiBuilder->get_widget<Gtk::Button>("LoginButton", loginBtn);
    uiBuilder->get_widget<Gtk::Entry>("ID", loginInput);
    uiBuilder->get_widget<Gtk::Entry>("Password", passwordInput);
    uiBuilder->get_widget<Gtk::Label>("LoginStatus", loginStatus);

    loginBtn->signal_clicked().connect(sigc::mem_fun(*this, &LoginWindow::OnLoginClick), false);
}

LoginWindow::LoginWindow()
{
    ProcessWidgets();
}

void LoginWindow::OpenWindow()
{
    window->show_all();
}
