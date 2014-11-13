/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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


#include "app.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface;

IMPLEMENT_APP( DFrontApp )

DFrontApp::DFrontApp( void ):
m_config( this )
{
}

DFrontApp::~DFrontApp( void )
{


}

bool DFrontApp::OnInit( void )
{

    DrawSpace::Initialize();

    bool parser_status = m_config.Run( "drawfront.cfg", "    " );

    m_console = new ConsoleDialog( NULL, "DrawFRONT", m_exeplugins );

    m_console->Show();
	return true;
}

int DFrontApp::OnExit( void )
{
    return 0;
}

DFrontApp::Config::Config( DFrontApp* p_owner ) :
m_owner( p_owner )
{
}

bool DFrontApp::Config::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    if( "exe_plugin" == p_words[0] && 3 == p_words.size() )
    {
        m_owner->m_exeplugins[p_words[1]] = p_words[2];
    }
    return true;
}