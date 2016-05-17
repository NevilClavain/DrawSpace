#include "DConfMainDialog.h"


DConfMainDialog::DConfMainDialog( wxWindow* parent, const wxString& title ) :
MainDialog( parent, wxID_ANY, title, wxDefaultPosition )
{
}


DConfMainDialog::~DConfMainDialog( void )
{
}

void DConfMainDialog::MainDialogOnClose( wxCloseEvent& event )
{
    Destroy();
}