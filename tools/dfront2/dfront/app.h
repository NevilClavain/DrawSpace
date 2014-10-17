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

#ifndef _APP_H_
#define _APP_H_

#include <wx/wx.h>

#include "console.h"



class DFrontApp : public wxApp
{
protected:

    class Config : public DrawSpace::Utils::Parser
    {
    protected:

        DFrontApp* m_owner;

        bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );

    public:
        Config( DFrontApp* p_owner );
    };


    ////////////////////////////////////////////////

    ConsoleDialog*                          m_console;
    Config                                  m_config;

    std::map<dsstring, dsstring>            m_exeplugins;


public:
	DFrontApp( void );
	virtual ~DFrontApp( void );

	virtual bool OnInit( void );
    virtual int OnExit( void );
};

DECLARE_APP( DFrontApp )




#endif