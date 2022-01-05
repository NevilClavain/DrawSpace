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

#include "planetconfig.h"
#include "planetinstance.h"

void PlanetSceneNodeConfig::GravityEnabledParam::OnUpdated( bool p_val )
{
	m_value = p_val;
	if( m_owner )
	{
		m_owner->OnGravityEnabledUpdate( m_value );
	}
}

void PlanetSceneNodeConfig::LandscapeBumpFactorParam::OnUpdated( dsreal p_val )
{
	m_value = p_val;
	if( m_owner )
	{
		m_owner->OnLandscapeBumpFactorUpdate( m_value );
	}
}

void PlanetSceneNodeConfig::BeachLimitParam::OnUpdated( dsreal p_val )
{
	m_value = p_val;
	if( m_owner )
	{
		m_owner->OnBeachLimitUpdate( m_value );
	}
}

void PlanetSceneNodeConfig::AtmoRenderEnableParam::OnUpdated( bool p_val )
{
	m_value = p_val;
	if( m_owner )
	{
		m_owner->OnAtmoRenderEnableUpdate( m_value );
	}
}

void PlanetSceneNodeConfig::AtmoKrParam::OnUpdated( dsreal p_val )
{
	m_value = p_val;
	if( m_owner )
	{
		m_owner->OnAtmoKrUpdate( p_val );
	}
}

void PlanetSceneNodeConfig::GroundFogAltLimitParam::OnUpdated( dsreal p_val )
{
	m_value = p_val;
	if( m_owner )
	{
		m_owner->OnGroundFogAltLimit( p_val );
	}
}

void PlanetSceneNodeConfig::GroundFogDensityParam::OnUpdated( dsreal p_val )
{
	m_value = p_val;
	if( m_owner )
	{
		m_owner->OnGroundFogDensity( p_val );
	}
}
