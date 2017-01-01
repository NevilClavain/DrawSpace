/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#pragma once

#include <d3dx9.h>
#include <vector>
#include "panel.h"



class DConfMainDialog : public MainDialog
{
protected:

	typedef struct
	{
		UINT width; 
		UINT height;
		UINT refresh_rate;
		D3DFORMAT format;
		std::string comment;

	} adapter_mode;

	typedef struct
	{
		D3DADAPTER_IDENTIFIER9		infos;
		std::vector<adapter_mode>	modes;

	} adapter_infos;

    LPDIRECT3D9								m_lpd3d;

	bool									m_fullscreen;


	std::vector<adapter_infos>				m_adapters_infos;

    virtual void OnMainDialogClose( wxCloseEvent& event );
    virtual void OnSaveButtonClick( wxCommandEvent& event );

public:
    DConfMainDialog( wxWindow* parent, const wxString& title );
    virtual ~DConfMainDialog( void );
};

