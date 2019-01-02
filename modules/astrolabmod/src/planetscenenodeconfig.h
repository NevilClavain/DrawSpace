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

#ifndef _PLANETSCENENODECONFIG_H_
#define _PLANETSCENENODECONFIG_H_

#include "drawspace.h"

class PlanetSceneNodeConfig
{
public:

	//////////////////////////////////////////////////////////////////

	DrawSpace::Module::KeySource<dsstring>          m_planetName;
	DrawSpace::Module::KeySource<dsreal>            m_planetRay;

    DrawSpace::Module::KeySource<dsreal>            m_plainsAmplitude;
    DrawSpace::Module::KeySource<dsreal>            m_moutainsAmplitude;
    DrawSpace::Module::KeySource<dsreal>            m_moutainsOffset;
    DrawSpace::Module::KeySource<dsreal>            m_verticalOffset;
    DrawSpace::Module::KeySource<dsreal>            m_beachLimit;
    DrawSpace::Module::KeySource<dsreal>            m_landscapeBumpFactor;
    DrawSpace::Module::KeySource<dsreal>            m_zbufferActivationRelAlt;
    DrawSpace::Module::KeySource<dsreal>            m_splatTransitionUpRelativeAlt;
    DrawSpace::Module::KeySource<dsreal>            m_splatTransitionDownRelativeAlt;
    DrawSpace::Module::KeySource<int>               m_nbLODFreeCameras;
    DrawSpace::Module::KeySource<int>               m_nbLODInertBodies;
    DrawSpace::Module::KeySource<int>               m_splatTextureResol;
    DrawSpace::Module::KeySource<dsreal>            m_plainsSeed1;
    DrawSpace::Module::KeySource<dsreal>            m_plainsSeed2;
    DrawSpace::Module::KeySource<dsreal>            m_mixSeed1;
    DrawSpace::Module::KeySource<dsreal>            m_mixSeed2;


	DrawSpace::Module::KeySource<dsstring>			m_detailsVertexShader;
	DrawSpace::Module::KeySource<dsstring>			m_detailsPixelShader;
	DrawSpace::Module::KeySource<bool>				m_gravityEnabled;
    DrawSpace::Module::KeySource<bool>              m_atmoRenderEnable;
    DrawSpace::Module::KeySource<dsreal>            m_atmoKr;
    DrawSpace::Module::KeySource<dsreal>            m_groundFogAltLimit;
    DrawSpace::Module::KeySource<dsreal>            m_groundFogDensity;

    DrawSpace::Module::KeySource<bool>              m_climatePassEnabled;
    DrawSpace::Module::KeySource<bool>              m_skirtsEnabled;
    DrawSpace::Module::KeySource<bool>              m_landplacePatchEnabled;

	//////////////////////////////////////////////////////////////////

	DrawSpace::Module::KeysLinkTable*               m_keylinksTable;

public:

	PlanetSceneNodeConfig( void ) :
		m_planetName( "planetName" ),
		m_planetRay( "planetRay" ),
        m_plainsAmplitude( "plainsAmplitude" ),
        m_moutainsAmplitude( "moutainsAmplitude" ),
        m_moutainsOffset( "moutainsOffset" ),
        m_verticalOffset( "verticalOffset" ),
        m_beachLimit( "beachLimit" ),
        m_landscapeBumpFactor( "landscapeBumpFactor" ),
        m_zbufferActivationRelAlt( "zbufferActivationRelAlt" ),
        m_splatTransitionUpRelativeAlt( "splatTransitionUpRelativeAlt" ),
        m_splatTransitionDownRelativeAlt( "splatTransitionDownRelativeAlt" ),
        m_nbLODFreeCameras( "nbLODFreeCameras" ),
        m_nbLODInertBodies( "nbLODInertBodies" ),
		m_detailsVertexShader( "detailsVertexShader" ),
		m_detailsPixelShader( "detailsPixelShader" ),
		m_gravityEnabled( "gravityEnabled" ),
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
	}
};

#endif
