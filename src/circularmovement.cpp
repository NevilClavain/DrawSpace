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

#include "circularmovement.h"
#include "maths.h"
#include "transformation.h"
#include "misc_utils.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

CircularMovement::CircularMovement( void ) :
m_angular_speed( 0.0 )
{
    // properties array creation
    //m_properties["configname"].AddPropValue<dsstring>( m_configname );

    m_properties["center_pos"].AddProp<Vector>();
    m_properties["delta_center"].AddProp<Vector>();
    m_properties["rot_axis"].AddProp<Vector>();

    m_properties["init_angle"].AddPropValue<dsreal>( 0.0 );
    m_properties["theta"].AddPropValue<dsreal>( 0.0 );
    m_properties["phi"].AddPropValue<dsreal>( 0.0 );

}

CircularMovement::~CircularMovement( void )
{
}

void CircularMovement::Init( const Vector& p_center_pos, const Vector& p_delta_center, const Vector& p_rotaxis, dsreal p_init_angle, dsreal p_theta, dsreal p_phi )
{
    m_rotaxis = p_rotaxis;
    m_center_pos = p_center_pos;
    m_delta_center = p_delta_center;

    m_qyaw.Identity();
	m_qpitch.Identity();
	m_rot_res.Identity();

    m_init_angle = p_init_angle;

    Reset();
    SetTheta( p_theta );
    SetPhi( p_phi );
}

void CircularMovement::Compute( Utils::TimeManager& p_timemanager )
{
    Transformation transformation;

    /////////////////////////////////////////////////

    Matrix orientation;

	Vector yaxis( 0.0, 1.0, 0.0, 1.0 );
	Vector xaxis( 1.0, 0.0, 0.0, 1.0 );

	m_qyaw.RotationAxis( yaxis, m_current_theta );
	m_qpitch.RotationAxis( xaxis, m_current_phi );

	m_rot_res = m_qpitch * m_qyaw;
	m_rot_res.RotationMatFrom( orientation );

    /////////////////////////////////////////////////

    p_timemanager.AngleSpeedInc( &m_current_angle, m_angular_speed );

    dsreal final_angle = m_current_angle + m_init_angle;
    if( final_angle >= 360.0 )
    {
        final_angle -= 360.0;
    }
    else if( final_angle < 0.0 )
    {
        final_angle = 360.0 + final_angle;
    }


    Matrix delta_center;
    delta_center.Translation( m_delta_center );

    Matrix rotation;
    rotation.Rotation( m_rotaxis, DrawSpace::Utils::Maths::DegToRad( final_angle ) );

    Matrix position;
    position.Translation( m_center_pos );



    transformation.PushMatrix( position );
    transformation.PushMatrix( rotation );
    transformation.PushMatrix( delta_center );
    transformation.PushMatrix( orientation );
    transformation.BuildResult();
    transformation.GetResult( &m_result );    
}

void CircularMovement::SetAngularSpeed( dsreal p_angular_speed )
{
    m_angular_speed = p_angular_speed;
}

void CircularMovement::SetTheta( dsreal p_theta )
{
    m_current_theta = DrawSpace::Utils::Maths::DegToRad( p_theta );
}

void CircularMovement::SetPhi( dsreal p_phi )
{
    m_current_phi = DrawSpace::Utils::Maths::DegToRad( p_phi );
}

void CircularMovement::Reset( void )
{
    m_current_angle = 0.0;
}

void CircularMovement::Serialize( Utils::Archive& p_archive  )
{

}

bool CircularMovement::Unserialize( Utils::Archive& p_archive )
{
    return true;
}

bool CircularMovement::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
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
    else*/ if( "center_pos" == p_words[0] )
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

        m_properties["center_pos"].SetPropValue<Vector>( v );
    }
    else if( "delta_center" == p_words[0] )
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

        m_properties["delta_center"].SetPropValue<Vector>( v );
    }
    else if( "rot_axis" == p_words[0] )
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

        m_properties["rot_axis"].SetPropValue<Vector>( v );
    }
    else if( "init_angle" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["init_angle"].SetPropValue<dsreal>( StringToReal( p_words[1] ) );
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

void CircularMovement::DumpProperties( dsstring& p_text )
{
    dsstring text_value;

    //p_text = "declare_config ";
    //p_text += dsstring( CIRCULARMVT_TEXT_KEYWORD );

    //p_text += "\n";
/*
    p_text += "configname ";
    p_text += m_properties["configname"].GetPropValue<dsstring>();
    p_text += "\n";
*/
    p_text += "center_pos ";
    Vector center_pos = m_properties["center_pos"].GetPropValue<Vector>();
    for( long i = 0; i < 4; i++ )
    {
        RealToString( center_pos[i], text_value );

        p_text += text_value;
        p_text += " ";
    }
    p_text += "\r\n";

    p_text += "delta_center ";
    Vector delta_center = m_properties["delta_center"].GetPropValue<Vector>();
    for( long i = 0; i < 4; i++ )
    {
        RealToString( delta_center[i], text_value );

        p_text += text_value;
        p_text += " ";
    }
    p_text += "\r\n";

    p_text += "rot_axis ";
    Vector rot_axis = m_properties["rot_axis"].GetPropValue<Vector>();
    for( long i = 0; i < 4; i++ )
    {
        RealToString( rot_axis[i], text_value );

        p_text += text_value;
        p_text += " ";
    }
    p_text += "\r\n";

    p_text += "init_angle ";
    RealToString( m_properties["init_angle"].GetPropValue<dsreal>(), text_value );
    p_text += "\r\n";

    p_text += "theta ";
    RealToString( m_properties["theta"].GetPropValue<dsreal>(), text_value );
    p_text += "\r\n";

    p_text += "phi ";
    RealToString( m_properties["phi"].GetPropValue<dsreal>(), text_value );
    p_text += "\r\n";

    //p_text += "end_config\n";
}

bool CircularMovement::ParseProperties( const dsstring& p_text )
{
    char seps[] = { 0x09, 0x020, 0x00 };

    return RunOnTextChunk( p_text, seps );
}

void CircularMovement::ApplyProperties( void )
{
    Init( m_properties["center_pos"].GetPropValue<Vector>(),             
            m_properties["delta_center"].GetPropValue<Vector>(),
            m_properties["rot_axis"].GetPropValue<Vector>(),
            m_properties["init_angle"].GetPropValue<dsreal>(),
            m_properties["theta"].GetPropValue<dsreal>(),
            m_properties["phi"].GetPropValue<dsreal>() );

    //m_configname = m_properties["configname"].GetPropValue<dsstring>();
}

Configurable* CircularMovement::Instanciate( void )
{
    return _DRAWSPACE_NEW_( CircularMovement, CircularMovement );
}

void CircularMovement::GetKeyword( dsstring& p_outkeyword )
{
    p_outkeyword = CIRCULARMVT_TEXT_KEYWORD;
}