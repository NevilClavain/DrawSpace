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
        dsstring        text;
        unsigned char   r;
        unsigned char   g;
        unsigned char   b;

        int             x;
        int             y;
    };

    using Color = struct
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };

    static const int MakeTextOperation                                  = 0;
    static const int MakeBoolParamOperation                             = 1;
    static const int MakeColorParamOperation                            = 2;
    static const int MakeRenderingQueueOnScreenOperation                = 3;
    static const int MakeRenderingQueueOnTargetOperation                = 4;

    static const int RenderingQueueSetTargetClearingColorsOperation     = 5;
    static const int RenderingQueueEnableTargetClearingOperation        = 6;
    static const int RenderingEnableDepthClearingOperation              = 7;

    static const int DrawTextOperation                                  = 8;
    static const int DrawRenderingQueueOperation                        = 9;

protected:

    DrawSpace::Interface::Renderer* m_renderer;

    virtual void on_entity_visited_action( int p_actionid, ecs::BaseArguments* p_args, ecs::BaseComponent* p_src, ecs::BaseComponent* p_dst ) const;
    virtual void on_entity_added_action( int p_actionid, ecs::BaseArguments* p_args, ecs::BaseComponent* p_src, ecs::BaseComponent* p_dst ) const;

public:

    RendergraphSystem( void );
    ~RendergraphSystem( void );

};

}

#endif