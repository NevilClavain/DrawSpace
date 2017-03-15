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

#ifndef _PLANETLODDEPENDANTNODEINFOS_H_
#define _PLANETLODDEPENDANTNODEINFOS_H_

#include "drawspace.h"


class PlanetLODDependantNodeInfos
{
public:

    DrawSpace::Module::KeySource<bool>              m_isNodeHot;
    DrawSpace::Module::KeySource<bool>              m_nodeRelativeAltitudeValid;

    DrawSpace::Module::KeySource<dsreal>            m_nodeAltitude;
    DrawSpace::Module::KeySource<dsreal>            m_nodeRelativeAltitude;
    DrawSpace::Module::KeySource<dsreal>            m_groundAlt;

    PlanetLODDependantNodeInfos( void ) :
        m_nodeAltitude( "nodeAltitude" ),
        m_nodeRelativeAltitude( "nodeRelativeAltitude" ),
        m_isNodeHot( "isNodeHot" ),
        m_nodeRelativeAltitudeValid( "nodeRelativeAltitudeValid" ),
        m_groundAlt( "groundAlt" )
    {
    }
};

#endif