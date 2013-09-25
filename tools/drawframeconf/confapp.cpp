
#include "confapp.h"
#include "confframe.h"

IMPLEMENT_APP( ConfApp )

ConfApp::ConfApp( void )
{
}

ConfApp::~ConfApp( void )
{


}

bool ConfApp::OnInit( void )
{
	ConfFrame* frame = new ConfFrame( _( "DrawSpace 3D Engine Config" ) );
	frame->Show();
    
	return true;
}