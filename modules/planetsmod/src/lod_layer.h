/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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
/* -*-LIC_END-*- */

#pragma once

#include "lod_patch.h"

namespace LOD
{
struct Config;
class Body;

class Layer
{
private:

    using PatchUpdateCb = DrawSpace::Core::CallBack2<Layer, void, Patch*, int>;

public:

    using SubPassCreationHandler = DrawSpace::Core::BaseCallback2<SubPass::EntryInfos, SubPass*, SubPass::Destination>;

private:

    Config*                             m_config;
    Body*                               m_body;
    Layer::SubPassCreationHandler*      m_handler;
    bool                                m_hot;
    int                                 m_current_lod;

    dsreal                              m_planetray;

    PatchUpdateCb                       m_patch_update_cb;

    void on_patchupdate(Patch* p_patch, int p_patch_lod);

public:
    Layer( Config* p_config, Body* p_body, Layer::SubPassCreationHandler* p_handler, int p_index );

    Body* GetBody(void) const;
    bool  GetHostState(void) const;
    Layer::SubPassCreationHandler* GetSubPassCreationHandler(void) const;
    int GetCurrentLOD(void) const;
    
    void SetHotState(bool p_hotstate);
    void UpdateRelativeAlt(dsreal p_alt);
    void UpdateInvariantViewerPos(const DrawSpace::Utils::Vector& p_pos);
    void UpdateHotPoint( const DrawSpace::Utils::Vector& p_vector );
    void Compute( void );
};
}
