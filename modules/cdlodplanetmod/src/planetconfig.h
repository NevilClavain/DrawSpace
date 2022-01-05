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

    class IntParam : public DrawSpace::Module::KeySink<int>
    {
    public:
	    int			    m_value;

	    IntParam( const dsstring& p_id ) : KeySink( p_id )
        {
        }

	    virtual void OnUpdated( int p_val )
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

    class BoolParam : public DrawSpace::Module::KeySink<bool>
    {
    public:
	    bool		    m_value;

	    BoolParam( const dsstring& p_id ) : KeySink( p_id )
	    {
	    }

	    virtual void OnUpdated( bool p_val )
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

    class AtmoRenderEnableParam : public DrawSpace::Module::KeySink<bool>
    {
    protected:

	    PlanetInstance*	m_owner;

    public:
	    bool			m_value;

	    AtmoRenderEnableParam( const dsstring& p_id ) : KeySink( p_id ),
	    m_owner( NULL )
	    {
	    }

	    virtual void OnUpdated( bool p_val );
	    virtual void SetOwner( PlanetInstance* p_owner )
	    {
		    m_owner = p_owner;
	    }
    };


    class AtmoKrParam : public DrawSpace::Module::KeySink<dsreal>
    {
    protected:

	    PlanetInstance*	m_owner;

    public:
	    dsreal			m_value;

	    AtmoKrParam( const dsstring& p_id ) : KeySink( p_id ),
	    m_owner( NULL )
	    {
	    }

	    virtual void OnUpdated( dsreal p_val );
	    virtual void SetOwner( PlanetInstance* p_owner )
	    {
		    m_owner = p_owner;
	    }
    };

    class GroundFogAltLimitParam : public DrawSpace::Module::KeySink<dsreal>
    {
    protected:

	    PlanetInstance*	m_owner;

    public:
	    dsreal			m_value;

	    GroundFogAltLimitParam( const dsstring& p_id ) : KeySink( p_id ),
	    m_owner( NULL )
	    {
	    }

	    virtual void OnUpdated( dsreal p_val );
	    virtual void SetOwner( PlanetInstance* p_owner )
	    {
		    m_owner = p_owner;
	    }
    };

    class GroundFogDensityParam : public DrawSpace::Module::KeySink<dsreal>
    {
    protected:

	    PlanetInstance*	m_owner;

    public:
	    dsreal			m_value;

	    GroundFogDensityParam( const dsstring& p_id ) : KeySink( p_id ),
	    m_owner( NULL )
	    {
	    }

	    virtual void OnUpdated( dsreal p_val );
	    virtual void SetOwner( PlanetInstance* p_owner )
	    {
		    m_owner = p_owner;
	    }
    };


    class LandscapeBumpFactorParam : public DrawSpace::Module::KeySink<dsreal>
    {
    protected:

	    PlanetInstance*	m_owner;

    public:
	    dsreal			m_value;

	    LandscapeBumpFactorParam( const dsstring& p_id ) : KeySink( p_id ),
	    m_owner( NULL )
	    {
	    }

	    virtual void OnUpdated( dsreal p_val );
	    virtual void SetOwner( PlanetInstance* p_owner )
	    {
		    m_owner = p_owner;
	    }
    };

    class BeachLimitParam : public DrawSpace::Module::KeySink<dsreal>
    {
    protected:

	    PlanetInstance*	m_owner;

    public:
	    dsreal			m_value;

	    BeachLimitParam( const dsstring& p_id ) : KeySink( p_id ),
	    m_owner( NULL )
	    {
	    }

	    virtual void OnUpdated( dsreal p_val );
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
    BeachLimitParam                     m_beachLimit;
    LandscapeBumpFactorParam            m_landscapeBumpFactor;
    RealParam                           m_zbufferActivationRelAlt;
    RealParam                           m_splatTransitionUpRelativeAlt;
    RealParam                           m_splatTransitionDownRelativeAlt;
    IntParam                            m_splatTextureResol;

    IntParam                            m_nbLODFreeCameras;
    IntParam                            m_nbLODInertBodies;

    RealParam                           m_plainsSeed1;
    RealParam                           m_plainsSeed2;
    RealParam                           m_mixSeed1;
    RealParam                           m_mixSeed2;

	StringParam							m_detailsVertexShader;
	StringParam							m_detailsPixelShader;

	GravityEnabledParam					m_gravityEnabled;

    AtmoRenderEnableParam               m_atmoRenderEnable;
    AtmoKrParam                         m_atmoKr;
    GroundFogAltLimitParam              m_groundFogAltLimit;
    GroundFogDensityParam               m_groundFogDensity;

    BoolParam                           m_climatePassEnabled;

    BoolParam                           m_skirtsEnabled;
    BoolParam                           m_landplacePatchEnabled;


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
    m_beachLimit( "beachLimit" ),
    m_landscapeBumpFactor( "landscapeBumpFactor" ),
    m_zbufferActivationRelAlt( "zbufferActivationRelAlt" ),
    m_splatTransitionUpRelativeAlt( "splatTransitionUpRelativeAlt" ),
    m_splatTransitionDownRelativeAlt( "splatTransitionDownRelativeAlt" ),
	m_detailsVertexShader( "detailsVertexShader" ),
	m_detailsPixelShader( "detailsPixelShader" ),
	m_gravityEnabled( "gravityEnabled" ),
    m_nbLODFreeCameras( "nbLODFreeCameras" ),
    m_nbLODInertBodies( "nbLODInertBodies" ),
    m_splatTextureResol( "splatTextureResol" ),
    m_plainsSeed1( "plainsSeed1" ),
    m_plainsSeed2( "plainsSeed2" ),
    m_mixSeed1( "mixSeed1" ),
    m_mixSeed2( "mixSeed1" ),
    m_atmoRenderEnable( "atmoRenderEnable" ),
    m_atmoKr( "atmoKr" ),
    m_groundFogAltLimit( "groundFogAltLimit" ),
    m_groundFogDensity( "groundFogDensty" ),
    m_climatePassEnabled( "climatePassEnabled" ),
    m_skirtsEnabled( "skirtsEnabled" ),
    m_landplacePatchEnabled( "landplacePatchEnabled" )
	{
		m_keylinksTable.RegisterModuleKey( &m_planetName );
		m_keylinksTable.RegisterModuleKey( &m_planetRay );
        m_keylinksTable.RegisterModuleKey( &m_plainsAmplitude );
        m_keylinksTable.RegisterModuleKey( &m_moutainsAmplitude );
        m_keylinksTable.RegisterModuleKey( &m_moutainsOffset );
        m_keylinksTable.RegisterModuleKey( &m_verticalOffset );
        m_keylinksTable.RegisterModuleKey( &m_beachLimit );
        m_keylinksTable.RegisterModuleKey( &m_landscapeBumpFactor );
        m_keylinksTable.RegisterModuleKey( &m_zbufferActivationRelAlt );
        m_keylinksTable.RegisterModuleKey( &m_splatTransitionUpRelativeAlt );
        m_keylinksTable.RegisterModuleKey( &m_splatTransitionDownRelativeAlt );
        m_keylinksTable.RegisterModuleKey( &m_splatTextureResol );
        m_keylinksTable.RegisterModuleKey( &m_nbLODFreeCameras );
        m_keylinksTable.RegisterModuleKey( &m_nbLODInertBodies );
		m_keylinksTable.RegisterModuleKey( &m_detailsVertexShader );
		m_keylinksTable.RegisterModuleKey( &m_detailsPixelShader );
		m_keylinksTable.RegisterModuleKey( &m_gravityEnabled );
        m_keylinksTable.RegisterModuleKey( &m_plainsSeed1 );
        m_keylinksTable.RegisterModuleKey( &m_plainsSeed2 );
        m_keylinksTable.RegisterModuleKey( &m_mixSeed1 );
        m_keylinksTable.RegisterModuleKey( &m_mixSeed2 );
        m_keylinksTable.RegisterModuleKey( &m_atmoRenderEnable );
        m_keylinksTable.RegisterModuleKey( &m_atmoKr );
        m_keylinksTable.RegisterModuleKey( &m_groundFogAltLimit );
        m_keylinksTable.RegisterModuleKey( &m_groundFogDensity );
        m_keylinksTable.RegisterModuleKey( &m_climatePassEnabled );
        m_keylinksTable.RegisterModuleKey( &m_skirtsEnabled );
        m_keylinksTable.RegisterModuleKey( &m_landplacePatchEnabled );
	}

	virtual void SetOwner( PlanetInstance* p_owner )
	{
		m_gravityEnabled.SetOwner( p_owner );
        m_landscapeBumpFactor.SetOwner( p_owner );
        m_beachLimit.SetOwner( p_owner );
        m_atmoRenderEnable.SetOwner( p_owner );
        m_atmoKr.SetOwner( p_owner );
        m_groundFogAltLimit.SetOwner( p_owner );
        m_groundFogDensity.SetOwner( p_owner );
	}
};


#endif
