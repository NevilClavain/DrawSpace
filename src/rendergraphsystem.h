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


#ifndef _RENDERGRAPHSYSTEM_H_
#define _RENDERGRAPHSYSTEM_H_

#include "ecs.h"
#include "renderer.h"

namespace DrawSpace
{
class RendergraphSystem : public DrawSpace::ecs::System
{
public:

    using Text = struct
    {
        dsstring        m_text;
        unsigned char   m_r;
        unsigned char   m_g;
        unsigned char   m_b;

        int             m_x;
        int             m_y;
    };

    static const int MakeTextOperation                                  = 0;
    static const int MakeBoolParamOperation                             = 1;
    static const int MakeColorParamOperation                            = 2;
    static const int DrawTextOperation                                  = 3;
    static const int MakeRenderingQueueOnScreenOperation                = 4;
    static const int MakeRenderingQueueOnTargetOperation                = 5;
    static const int RenderingQueueSetTargetClearingColorsOperation     = 6;
    static const int RenderingQueueEnableTargetClearingOperation        = 7;
    static const int RenderingEnableDepthClearingOperation              = 8;
    static const int DrawRenderingQueueOperation                        = 9;

protected:

    DrawSpace::Interface::Renderer* m_renderer;

    virtual void on_entity_visited_action(int p_actionid, ecs::BaseArguments* p_args) const;
    virtual void on_entity_added_action(int p_actionid, ecs::BaseArguments* p_args) const;

public:

    RendergraphSystem( void );
    ~RendergraphSystem( void );

};
}

#endif