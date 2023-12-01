#include <iostream>
#include <sstream>
#include <fstream>

#include "global.hxx"
#include "Client.hxx"

#include "LoginWindow.hxx"
#include "MainWindow.hxx"
#include "AdminWindow.hxx"

using namespace std;

void LoginWindow::ProcessQueries()
{
    switch (loginState)
    {
    case LoginState::Server:
    {
        net::message<MsgTypes> iMsg;
        iMsg << global::currentUserName.c_str() << global::currentUserHash.c_str();
        net::message<MsgTypes> oMsg = Client::GetInstance().SendRequestToServer(MsgTypes::PasswordLogin, iMsg);
        
        if (oMsg.header.id == MsgTypes::ServerAccept)
        {
            if(oMsg.size() > 0)
            {
                char req[256];
                oMsg >> req;
                cout<<req<<endl;
            }
            else
            {
                cout<<"Empty respond\n";
            }

            window->hide();
            mainWindow->OpenWindow();
        }
        else if (oMsg.header.id == MsgTypes::ServerDeny)
        {
            loginStatus->set_text("Incorrect login or password");
        }

        break;
    }
    case LoginState::Local:
    {
        std::hash<string> hash;
        unsigned long adminPasswordHashed = 2338358718769850882;
        if (loginInput->get_text() == "Admin" && hash(passwordInput->get_text()) == adminPasswordHashed)
        {
            mainWindow->isServerActive = false;
            window->hide();
            mainWindow->OpenWindow();
        }
        else
        {
            loginStatus->set_text("Incorrect login or password");
        }

        break;
    }

    default:
        break;
    }
}

void LoginWindow::OnLoginClick()
{
    if (passwordInput->get_text() == "" || loginInput->get_text() == "")
    {
        loginStatus->set_text("Incorrect login or password");
        return;
    }

    global::currentUserName = loginInput->get_text();
    std::hash<string> hash;
    global::currentUserHash = to_string(hash(passwordInput->get_text()));

    ProcessQueries();
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

void LoginWindow::OpenWindow(LoginState loginState)
{
    this->loginState = loginState;
    window->show_all();
}
