/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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

#ifndef _NODEPLANETINFOS_H_
#define _NODEPLANETINFOS_H_

#include "drawspace.h"


///////////////////////////////////////////////////////////////////////////////////////////////

// infos de "nature planetaire" relative a un node scenegraph : altitude, altitude relative, hot ou non, etc...

class NodePlanetInfos
{
public:

    class RealInfo : public DrawSpace::Module::KeySink<dsreal>
    {
    public:
	    dsreal			m_value;

	    RealInfo( const dsstring& p_id ) : KeySink( p_id )
        {
        }

	    virtual void OnUpdated( dsreal p_val )
        {
            m_value = p_val;
        }
    };

    class IntInfo : public DrawSpace::Module::KeySink<int>
    {
    public:
	    int			m_value;

	    IntInfo( const dsstring& p_id ) : KeySink( p_id )
        {
        }

	    virtual void OnUpdated( int p_val )
        {
            m_value = p_val;
        }
    };

    class BoolInfo : public DrawSpace::Module::KeySink<bool>
    {
    public:
	    bool			m_value;

	    BoolInfo( const dsstring& p_id ) : KeySink( p_id )
        {
        }

	    virtual void OnUpdated( bool p_val )
        {
            m_value = p_val;
        }
    };

    ///////////////////////////////////////////////////////////////////////////////

    BoolInfo                            m_isCamera;

    IntInfo                             m_cameraType;

    BoolInfo                            m_isNodeHot;

    RealInfo                            m_nodeAltitude;

    BoolInfo                            m_nodeRelativeAltitudeValid;
    RealInfo                            m_nodeRelativeAltitude;

    BoolInfo                            m_groundAltValid;
    RealInfo                            m_groundAlt;

    IntInfo                             m_nbSubPasses;

	////////////////////////////////////////////////////////////////////////////////

	DrawSpace::Module::KeysLinkTable    m_keylinksTable;

public:

	NodePlanetInfos( void ) :
    m_isCamera( "isCamera" ),
    m_cameraType( "cameraType"),
    m_isNodeHot( "isNodeHot" ),
    m_nodeAltitude( "nodeAltitude" ),
    m_nodeRelativeAltitudeValid( "nodeRelativeAltitudeValid" ),
    m_nodeRelativeAltitude( "nodeRelativeAltitude" ),
    m_groundAlt( "groundAlt" ),
    m_groundAltValid( "groundAltValid" ),
    m_nbSubPasses( "nbSubPasses" )
	{
		m_keylinksTable.RegisterModuleKey( &m_isCamera );
        m_keylinksTable.RegisterModuleKey( &m_cameraType );
        m_keylinksTable.RegisterModuleKey( &m_isNodeHot );
        m_keylinksTable.RegisterModuleKey( &m_nodeAltitude );
        m_keylinksTable.RegisterModuleKey( &m_nodeRelativeAltitudeValid );
        m_keylinksTable.RegisterModuleKey( &m_nodeRelativeAltitude );
        m_keylinksTable.RegisterModuleKey( &m_groundAlt );
        m_keylinksTable.RegisterModuleKey( &m_groundAltValid );
        m_keylinksTable.RegisterModuleKey( &m_nbSubPasses );
	}
};


#endif
