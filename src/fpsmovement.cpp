/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#include "fpsmovement.h"
#include "misc_utils.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

FPSMovement::FPSMovement( bool p_ymvt ) : m_ymvt( p_ymvt )
{
    // properties array creation
    m_properties["configname"].AddPropValue<dsstring>( m_configname );

    m_properties["init_pos"].AddProp<Vector>();
    m_properties["init_yaw"].AddPropValue<dsreal>( 0.0 );
    m_properties["init_pitch"].AddPropValue<dsreal>( 0.0 );

}

FPSMovement::~FPSMovement( void )
{

}

void FPSMovement::Init( const Utils::Vector& p_init_pos, dsreal p_initial_yaw, dsreal p_initial_pitch )
{
	m_qyaw.Identity();
	m_qpitch.Identity();
	m_current_res.Identity();

	m_position.Translation( p_init_pos );
	m_orientation.Identity();

	m_ayaw = p_initial_yaw;
	m_apitch = p_initial_pitch;
}

void FPSMovement::InitRot( void )
{
	m_qyaw.Identity();
	m_qpitch.Identity();
	m_current_res.Identity();

	m_orientation.Identity();

	m_ayaw = 0.0;
	m_apitch = 0.0;
}

void FPSMovement::RotateYaw( dsreal p_speed, TimeManager& p_timemanager )
{
    p_timemanager.AngleSpeedInc( &m_ayaw, p_speed );
}

void FPSMovement::RotatePitch( dsreal p_speed, TimeManager& p_timemanager )
{
    p_timemanager.AngleSpeedInc( &m_apitch, p_speed );
}

void FPSMovement::SetSpeed( dsreal p_speed )
{
    m_local_speed[2] = -p_speed;
}

void FPSMovement::Compute( TimeManager& p_timemanager/*, bool p_ymvt */ )
{
	Vector gs;

	Vector yaxis( 0.0, 1.0, 0.0, 1.0 );
	Vector xaxis( 1.0, 0.0, 0.0, 1.0 );

	m_qyaw.RotationAxis( yaxis, m_ayaw );
	m_qpitch.RotationAxis( xaxis, m_apitch );

	m_current_res = m_qpitch * m_qyaw;
	m_current_res.RotationMatFrom( m_orientation );

	m_orientation.Transform( &m_local_speed, &gs );

	p_timemanager.TranslationSpeedInc( &m_position( 3, 0 ), gs[0] );

    
	if( m_ymvt )
	{
		// prendre aussi en compte la composante en Y (la camera peut aussi evoluer "en hauteur")
		p_timemanager.TranslationSpeedInc( &m_position( 3, 1 ), gs[1] );
	}
    

	p_timemanager.TranslationSpeedInc( &m_position( 3, 2 ), gs[2] );

    m_result = m_orientation * m_position;
}

void FPSMovement::Serialize( Utils::Archive& p_archive  )
{
}

bool FPSMovement::Unserialize( Utils::Archive& p_archive )
{
    return true;
}

bool FPSMovement::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    if( "configname" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["configname"].SetPropValue<dsstring>( p_words[1] );
    }
    else if( "init_pos" == p_words[0] )
    {
        if( p_words.size() < 4 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        Vector v;
        for( long i = 0; i < 3; i++ )
        {
            v[i] = StringToReal( p_words[i + 1] );
        }
        v[3] = 1.0;

        m_properties["init_pos"].SetPropValue<Vector>( v );
    }
    else if( "init_yaw" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["init_yaw"].SetPropValue<dsreal>( StringToReal( p_words[1] ) );
    }
    else if( "init_pitch" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["init_pitch"].SetPropValue<dsreal>( StringToReal( p_words[1] ) );
    }
    else
    {
        _PARSER_UNEXPECTED_KEYWORD_
        return false;
    }

    return true;
}

void FPSMovement::DumpProperties( dsstring& p_text )
{
    dsstring text_value;

    p_text = "declare_config ";
    p_text += dsstring( FPSMVT_TEXT_KEYWORD );

    p_text += "\n";

    p_text += "configname ";
    p_text += m_properties["configname"].GetPropValue<dsstring>();
    p_text += "\n";

    p_text += "init_pos ";
    Vector init_pos = m_properties["init_pos"].GetPropValue<Vector>();
    for( long i = 0; i < 4; i++ )
    {
        RealToString( init_pos[i], text_value );

        p_text += text_value;
        p_text += " ";
    }
    p_text += "\n";


    p_text += "init_yaw ";
    RealToString( m_properties["init_yaw"].GetPropValue<dsreal>(), text_value );
    p_text += "\n";

    p_text += "init_pitch ";
    RealToString( m_properties["init_pitch"].GetPropValue<dsreal>(), text_value );
    p_text += "\n";

    p_text += "end_config\n";

}

bool FPSMovement::ParseProperties( const dsstring& p_text )
{
    char seps[] = { 0x09, 0x020, 0x00 };

    return RunOnTextChunk( p_text, seps );
}

void FPSMovement::ApplyProperties( void )
{
    Init( m_properties["init_pos"].GetPropValue<Vector>(),             
            m_properties["init_yaw"].GetPropValue<dsreal>(),
            m_properties["init_pitch"].GetPropValue<dsreal>() );

    m_configname = m_properties["configname"].GetPropValue<dsstring>();
}

FPSMovement::Configurable* Instanciate( void )
{
    return _DRAWSPACE_NEW_( FPSMovement, FPSMovement );
}
