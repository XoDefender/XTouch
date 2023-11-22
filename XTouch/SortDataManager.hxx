#pragma once

#include "Client.hxx"

class SortDataManager
{
public:
    bool isServerActive = true;

public:
    virtual void FillGrid(MsgTypes msgType, olc::net::message<MsgTypes> iMsg = Client::GetInstance().GetEmptyMessage()) = 0;

    void ClearGrid(Gtk::Grid *grid)
    {
        while (true)
        {
            if (grid->get_child_at(1, 1) != nullptr)
                grid->remove_row(1);
            else
                break;
        }
    };

    void SortData(MsgTypes msgType, bool &isSortAsc, std::string sortAsc, std::string sortDesc)
    {
        olc::net::message<MsgTypes> iMsg;

        if (msgType == MsgTypes::SortFiles)
            iMsg << global::currentModelFolder.c_str();

        if (isSortAsc)
        {
            iMsg << sortAsc.c_str();
            isSortAsc = false;
        }
        else
        {
            iMsg << sortDesc.c_str();
            isSortAsc = true;
        }

        FillGrid(msgType, iMsg);
    }
};