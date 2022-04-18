/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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

#include "??extension_type??aspectimpl.h"

class ??class_name????extension_type??AspectImpl : public DrawSpace::Interface::AspectImplementations::??extension_type??AspectImpl
{
private:
    
public:
    ??class_name????extension_type??AspectImpl( void );

    bool Init( DrawSpace::Core::Entity* p_entity, DrawSpace::Utils::TimeManager* p_timemanager) { return true; };
    void Release( void ) {};
    void Run( DrawSpace::Core::Entity* p_entity );
    void SetEntityNodeGraph(DrawSpace::EntityGraph::EntityNodeGraph* p_entitynodegraph) {};
};