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

#ifndef __BasicSceneScriptEditFrame__
#define __BasicSceneScriptEditFrame__

#include "panel.h"
#include "drawspace.h"

class BasicSceneScriptEditFrame : public ScriptEditFrame
{
protected:

    virtual void OnCloseButtonClicked( wxCommandEvent& event );
    virtual void OnApplyButtonClicked( wxCommandEvent& event );
    virtual void OnLoadButtonClicked( wxCommandEvent& event );
    virtual void OnClearButtonClicked( wxCommandEvent& event );

  
    wxFont      m_console_font;

    dsstring*   m_text;
    bool*       m_script_state;


public:

    BasicSceneScriptEditFrame( wxWindow* p_parent, const dsstring& p_title, dsstring* p_text, bool* p_script_state );

    void        Show( void );
	dsstring*   GetTextRef( void );
    bool*       GeScriptStateRef( void );
};

#endif // __BasicSceneScriptEditFrame__
