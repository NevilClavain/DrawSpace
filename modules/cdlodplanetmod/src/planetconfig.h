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

#ifndef _PLANETCONFIG_H_
#define _PLANETCONFIG_H_

#include "drawspace.h"

class PlanetInstance; // fwd declaration


///////////////////////////////////////////////////////////////////////////////////////////////

class PlanetSceneNodeConfig
{
public:

    class RealParam : public DrawSpace::Module::KeySink<dsreal>
    {
    public:
	    dsreal			m_value;

	    RealParam( const dsstring& p_id ) : KeySink( p_id )
        {
        }

	    virtual void OnUpdated( dsreal p_val )
        {
            m_value = p_val;
        }
    };

    class StringParam : public DrawSpace::Module::KeySink<dsstring>
    {
    public:
	    dsstring		m_value;

	    StringParam( const dsstring& p_id ) : KeySink( p_id )
	    {
	    }

	    virtual void OnUpdated( dsstring p_val )
	    {
		    m_value = p_val;
	    }
    };

    class GravityEnabledParam : public DrawSpace::Module::KeySink<bool>
    {
    protected:

	    PlanetInstance*	m_owner;

    public:
	    bool			m_value;

	    GravityEnabledParam( const dsstring& p_id ) : KeySink( p_id ),
	    m_owner( NULL )
	    {
	    }

	    virtual void OnUpdated( bool p_val );
	    virtual void SetOwner( PlanetInstance* p_owner )
	    {
		    m_owner = p_owner;
	    }
    };


	/////////////////--*- ENSEMBLE DES PARAMETRES PLANETE -*-- ////////////////////

	StringParam							m_planetName;

	RealParam                           m_planetRay;
    RealParam                           m_plainsAmplitude;
    RealParam                           m_moutainsAmplitude;
    RealParam                           m_moutainsOffset;
    RealParam                           m_verticalOffset;

	StringParam							m_detailsVertexShader;
	StringParam							m_detailsPixelShader;

	GravityEnabledParam					m_gravityEnabled;

	////////////////////////////////////////////////////////////////////////////////

	DrawSpace::Module::KeysLinkTable    m_keylinksTable;

public:

	PlanetSceneNodeConfig( void ) :
	m_planetName( "planetName" ),
	m_planetRay("planetRay"),
    m_plainsAmplitude( "plainsAmplitude" ),
    m_moutainsAmplitude( "moutainsAmplitude" ),
    m_moutainsOffset( "moutainsOffset" ),
    m_verticalOffset( "verticalOffset" ),
	m_detailsVertexShader( "detailsVertexShader" ),
	m_detailsPixelShader( "detailsPixelShader" ),
	m_gravityEnabled( "gravityEnabled" )
	{
		m_keylinksTable.RegisterModuleKey( &m_planetName );
		m_keylinksTable.RegisterModuleKey( &m_planetRay );
        m_keylinksTable.RegisterModuleKey( &m_plainsAmplitude );
        m_keylinksTable.RegisterModuleKey( &m_moutainsAmplitude );
        m_keylinksTable.RegisterModuleKey( &m_moutainsOffset );
        m_keylinksTable.RegisterModuleKey( &m_verticalOffset );
		m_keylinksTable.RegisterModuleKey( &m_detailsVertexShader );
		m_keylinksTable.RegisterModuleKey( &m_detailsPixelShader );
		m_keylinksTable.RegisterModuleKey( &m_gravityEnabled );
	}

	virtual void SetOwner( PlanetInstance* p_owner )
	{
		m_gravityEnabled.SetOwner( p_owner );
	}
};


#endif