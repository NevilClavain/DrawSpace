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

#include "linearmovement.h"
#include "maths.h"
#include "misc_utils.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;


LinearMovement::LinearMovement( void ) :
m_speed( 0.0 )
{
    // properties array creation
    //m_properties["configname"].AddPropValue<dsstring>( m_configname );

    m_properties["init_pos"].AddProp<Vector>();
    m_properties["direction"].AddProp<Vector>();

    m_properties["theta"].AddPropValue<dsreal>( 0.0 );
    m_properties["phi"].AddPropValue<dsreal>( 0.0 );
}

LinearMovement::~LinearMovement( void )
{

}

void LinearMovement::Init( const Utils::Vector& p_init_pos, const Utils::Vector& p_direction, dsreal p_theta, dsreal p_phi )
{
    m_direction = p_direction;
    m_direction.Normalize();

    m_init_pos = p_init_pos;

    m_qyaw.Identity();
	m_qpitch.Identity();
	m_rot_res.Identity();

    Reset();
    SetTheta( p_theta );
    SetPhi( p_phi );
}

void LinearMovement::Compute( Utils::TimeManager& p_timemanager )
{
    /////////////////////////////////////////////////

    Matrix orientation;

	Vector yaxis( 0.0, 1.0, 0.0, 1.0 );
	Vector xaxis( 1.0, 0.0, 0.0, 1.0 );

	m_qyaw.RotationAxis( yaxis, m_current_theta );
	m_qpitch.RotationAxis( xaxis, m_current_phi );

	m_rot_res = m_qpitch * m_qyaw;
	m_rot_res.RotationMatFrom( orientation );

    /////////////////////////////////////////////////

    Matrix translation;

    dsreal delta_x = m_current_pos[0];
    dsreal delta_y = m_current_pos[1];
    dsreal delta_z = m_current_pos[2];

    p_timemanager.TranslationSpeedInc( &delta_x, m_speed * m_direction[0] );
    p_timemanager.TranslationSpeedInc( &delta_y, m_speed * m_direction[1] );
    p_timemanager.TranslationSpeedInc( &delta_z, m_speed * m_direction[2] );
    
    m_current_pos[0] = delta_x;
    m_current_pos[1] = delta_y;
    m_current_pos[2] = delta_z;

    translation.Translation( m_current_pos[0] + m_init_pos[0], m_current_pos[1] + m_init_pos[1], m_current_pos[2] + m_init_pos[2] );

    /////////////////////////////////////////////////

    m_result = orientation * translation;
}

void LinearMovement::SetSpeed( dsreal p_speed )
{
    m_speed = p_speed;
}

void LinearMovement::Reset( void )
{
    m_current_pos[0] = 0.0;
    m_current_pos[1] = 0.0;
    m_current_pos[2] = 0.0;
}

void LinearMovement::SetTheta( dsreal p_theta )
{
    m_current_theta = DrawSpace::Utils::Maths::DegToRad( p_theta );
}

void LinearMovement::SetPhi( dsreal p_phi )
{
    m_current_phi = DrawSpace::Utils::Maths::DegToRad( p_phi );
}

dsreal LinearMovement::GetTranslationLength( void )
{
    return m_current_pos.Length();
}

bool LinearMovement::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    /*
    if( "configname" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["configname"].SetPropValue<dsstring>( p_words[1] );
    }
    else*/ if( "init_pos" == p_words[0] )
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
    else if( "direction" == p_words[0] )
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

        m_properties["direction"].SetPropValue<Vector>( v );
    }
    else if( "theta" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["theta"].SetPropValue<dsreal>( StringToReal( p_words[1] ) );
    }
    else if( "phi" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["phi"].SetPropValue<dsreal>( StringToReal( p_words[1] ) );
    }
    else
    {
        _PARSER_UNEXPECTED_KEYWORD_
        return false;
    }

    return true;
}

void LinearMovement::Serialize( Utils::Archive& p_archive  )
{
}

bool LinearMovement::Unserialize( Utils::Archive& p_archive )
{
    return true;
}

void LinearMovement::DumpProperties( dsstring& p_text )
{
    dsstring text_value;

    //p_text = "declare_config ";
    //p_text += dsstring( LINEARMVT_TEXT_KEYWORD );

    //p_text += "\n";
/*
    p_text += "configname ";
    p_text += m_properties["configname"].GetPropValue<dsstring>();
    p_text += "\n";
*/
    p_text += "init_pos ";
    Vector init_pos = m_properties["init_pos"].GetPropValue<Vector>();
    for( long i = 0; i < 4; i++ )
    {
        RealToString( init_pos[i], text_value );

        p_text += text_value;
        p_text += " ";
    }
    p_text += "\n";

    p_text += "direction ";
    Vector direction = m_properties["direction"].GetPropValue<Vector>();
    for( long i = 0; i < 4; i++ )
    {
        RealToString( direction[i], text_value );

        p_text += text_value;
        p_text += " ";
    }
    p_text += "\n";

    p_text += "theta ";
    RealToString( m_properties["theta"].GetPropValue<dsreal>(), text_value );
    p_text += "\n";

    p_text += "phi ";
    RealToString( m_properties["phi"].GetPropValue<dsreal>(), text_value );
    p_text += "\n";

    //p_text += "end_config\n";
}

bool LinearMovement::ParseProperties( const dsstring& p_text )
{
    char seps[] = { 0x09, 0x020, 0x00 };

    return RunOnTextChunk( p_text, seps );
}


void LinearMovement::ApplyProperties( void )
{      
    Init( m_properties["init_pos"].GetPropValue<Vector>(),             
            m_properties["direction"].GetPropValue<Vector>(),            
            m_properties["theta"].GetPropValue<dsreal>(),
            m_properties["phi"].GetPropValue<dsreal>() );

    //m_configname = m_properties["configname"].GetPropValue<dsstring>();
}

Configurable* LinearMovement::Instanciate( void )
{
    return _DRAWSPACE_NEW_( LinearMovement, LinearMovement );
}
