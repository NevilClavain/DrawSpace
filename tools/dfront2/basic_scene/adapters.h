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

#ifndef _ADAPTERS_H_
#define _ADAPTERS_H_

#include <wx/listctrl.h>
#include <wx/propgrid/propgrid.h>
#include "drawspace.h"
#include "BasicSceneObjectPropertiesDialog.h"
#include "BasicSceneMainFrame.h"

class wxWidgetAdapter
{
protected:

    static wxWidgetAdapter* m_instance;
   
    DrawSpace::Core::CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>* m_applypassshadervalues_callback;

    


    DrawSpace::Core::CallBack<wxWidgetAdapter, void, BasicSceneObjectPropertiesDialog*>* m_applytransfonodevalues_callback;

    

    long m_pass_slot_index;

    


    void on_applypassshadervalues( BasicSceneObjectPropertiesDialog* p_dialog );
    




    wxWidgetAdapter( void );


public:

    long m_register_index;

    ~wxWidgetAdapter( void );

    static wxWidgetAdapter* GetInstance( void )
    {
        if( !m_instance )
        {
            m_instance = new wxWidgetAdapter;
        }
        return m_instance;
    }

    void AdaptAssetsList( wxListCtrl* p_listctrl );
    void AdaptConfigsList( wxListCtrl* p_listctrl );
    void AdaptPassesList( wxListCtrl* p_listctrl );
    void AdaptPassesShaderParamsList( DrawSpace::Pass* p_pass, wxListCtrl* p_listctrl );
    void AdaptPassShaderValuesPropsModification( DrawSpace::Pass* p_pass, char* p_param_id, BasicSceneObjectPropertiesDialog* p_dialog );
    
};


#endif
