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

#ifndef _DRAW_COMPONENT_H_
#define _DRAW_COMPONENT_H_

#include "component.h"
#include "renderingnode.h"
#include "components_ids.h"

#include "plugin.h"
#include "renderer.h"
#include "vector.h"

namespace DrawSpace
{
struct DrawComponent : public ComponentBase
{
public:
    using Callback = DrawSpace::Core::CallBack<DrawComponent, void, DrawSpace::Core::RenderingNode*>;

private:

    DrawSpace::Interface::Renderer* m_renderer;

public:
    DrawComponent( void )
    {
        m_type = DrawComponentType;
        m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    }

    void on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node )
    {        
        //m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
        DrawSpace::Interface::Renderer::Characteristics renderer_characteristics;
        m_renderer->GetRenderCharacteristics( renderer_characteristics );

        DrawSpace::Utils::Matrix view;
        DrawSpace::Utils::Matrix proj;
        DrawSpace::Utils::Matrix world;

        // TEMPORAIRE : TODO : prendre la mat view de la camera courante du scenegraph
        view.Identity();

        // TEMPORAIRE : TODO : prendre la mat proj de la camera courante du scenegraph
        proj.Perspective( renderer_characteristics.width_viewport, renderer_characteristics.height_viewport, 1.0, 100000000000.0 );


        // TEMPORAIRE : prendre le global_word d'un cmposant transfo dans la meme entitee
        world.Translation( Utils::Vector( 0.0, 0.0, -5.0, 1.0 ) );
             
        m_renderer->DrawMeshe( world, view, proj );
    }
};
}

#endif