#pragma once
#include "panel.h"


class DConfMainDialog : public MainDialog
{
protected:

    virtual void MainDialogOnClose( wxCloseEvent& event );

public:
    DConfMainDialog( wxWindow* parent, const wxString& title );
    virtual ~DConfMainDialog( void );
};

