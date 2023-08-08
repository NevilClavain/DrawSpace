/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

#include "renderstate.h"

namespace DrawSpace
{
namespace Core
{

class RenderStatesSet
{                                                                            
public:

    RenderStatesSet( void );  
    ~RenderStatesSet( void );

    void            SetRenderStateUniqueQueueID(const dsstring& p_id);

    void            AddRenderStateIn( const RenderState& p_renderstate );
    void            AddRenderStateOut( const RenderState& p_renderstate );

    void            UpdateRenderStateIn( int p_index, const RenderState& p_renderstate );
    void            UpdateRenderStateOut( int p_index, const RenderState& p_renderstate );

    void            UpdateRenderStateInExtendedArgs(int p_index, const std::vector<dsstring>& p_args);
    void            UpdateRenderStateOutExtendedArgs(int p_index, const std::vector<dsstring>& p_args);

    void            ClearRenderStateInExtendedArgs(int p_index);
    void            ClearRenderStateOutExtendedArgs(int p_index);

    void            PushRenderStateInExtendedArgs(int p_index, const dsstring& p_arg);
    void            PushRenderStateOutExtendedArgs(int p_index, const dsstring& p_arg);

    RenderState     GetRenderStateIn( long p_index ) const;
    RenderState     GetRenderStateOut( long p_index ) const;
    size_t          GetRenderStatesInListSize( void ) const;
    size_t          GetRenderStatesOutListSize( void ) const;   
    void            GetRenderStatesSetMD5( dsstring& p_md5 ) const;

private:


    std::vector<RenderState>        m_renderstates_in;
    std::vector<RenderState>        m_renderstates_out;

    dsstring                        m_renderstate_unique_queue_id; //si non vide, permet de forcer la rendering queue a garder
                                                                                // une entree setrenderstates specialement dediee au node associee
                                                                                // au lieu d'utiliser celle issue du regroupement
                                                                                // Utile pour le cas ou on doit updater un ou plusieurs renderstate
                                                                                // pour ce node pendant le rendu   
};
}
}
