/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
*                                                                          
* This file is part of DrawSpace.                                          
*                                                                          
*    DrawSpace is free software: you can redistribute it and/or modify     
*    it under the terms of the GNU General Public License as published by  
*    the Free Software Foundation, either version 3 of the License, or     
*    (at your option) any later version.                                   
*                                                                          
*    DrawSpace is distributed in the hope that it will be useful,          
*    but WITHOUT ANY WARRANTY; without even the implied warranty of        
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
*    GNU General Public License for more details.                          
*                                                                          
*    You should have received a copy of the GNU General Public License     
*    along with DrawSpace.  If not, see <http://www.gnu.org/licenses/>.    
*
*/

#include "confapp.h"
#include "DConfMainDialog.h"

// sur laptop, forcer l'utilisation du GPU haute performance, au lieu des chipset graphiques integrés
extern "C" {
	// This is the quickest and easiest way to enable using the nVidia GPU on a Windows laptop with a dedicated nVidia GPU and Optimus tech.
	// enable optimus!
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;

	// AMD have one too!!!
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}



IMPLEMENT_APP( ConfApp )

ConfApp::ConfApp( void )
{
}

ConfApp::~ConfApp( void )
{


}

bool ConfApp::OnInit( void )
{
    DConfMainDialog* dialog = new DConfMainDialog( NULL, "DrawSpace D3D11 Configuration" );

    dialog->Show();   
	return true;
}


int ConfApp::OnExit( void )
{
    return 0;
}