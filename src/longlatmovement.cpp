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

#include "longlatmovement.h"
#include "maths.h"
#include "transformation.h"
#include "misc_utils.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;


LongLatMovement::LongLatMovement( void )
{
    // properties array creation
    //m_properties["configname"].AddPropValue<dsstring>( m_configname );

    m_properties["init_longitud_theta"].AddPropValue<dsreal>( 0.0 );
    m_properties["init_longitud_phi"].AddPropValue<dsreal>( 0.0 );
    m_properties["init_alt"].AddPropValue<dsreal>( 0.0 );
    m_properties["init_theta"].AddPropValue<dsreal>( 0.0 );
    m_properties["init_phi"].AddPropValue<dsreal>( 0.0 );    
}

LongLatMovement::~LongLatMovement( void )
{
}

void LongLatMovement::Init( dsreal p_init_longitud_theta, dsreal p_init_latitud_phi, dsreal p_init_alt, dsreal p_init_theta, dsreal p_init_phi )
{
    m_longitud_theta = DrawSpace::Utils::Maths::DegToRad( p_init_longitud_theta );
    m_latitud_phi = DrawSpace::Utils::Maths::DegToRad( p_init_latitud_phi );
    m_alt = p_init_alt;

    SetTheta( p_init_theta );
    SetPhi( p_init_phi );

}

void LongLatMovement::Compute( Utils::TimeManager& p_timemanager )
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

    Matrix init_rot;
    init_rot.Rotation( Vector( 1.0, 0.0, 0.0, 1.0 ), PI / 2.0 );

    Matrix altitude;
    altitude.Translation( 0.0, 0.0, m_alt );

    Matrix longitud;
    longitud.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), m_longitud_theta );

    Matrix latitud;
    latitud.Rotation( Vector( -1.0, 0.0, 0.0, 1.0 ), m_latitud_phi );
    
    transformation.PushMatrix( longitud );
    transformation.PushMatrix( latitud );
    transformation.PushMatrix( altitude );
    transformation.PushMatrix( init_rot );
    transformation.PushMatrix( orientation );
    transformation.BuildResult();
    transformation.GetResult( &m_result );
}

void LongLatMovement::SetTheta( dsreal p_theta )
{
    m_current_theta = DrawSpace::Utils::Maths::DegToRad( p_theta );
}

void LongLatMovement::SetPhi( dsreal p_phi )
{
    m_current_phi = DrawSpace::Utils::Maths::DegToRad( p_phi );
}

bool LongLatMovement::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
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
    else*/ if( "init_longitud_theta" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["init_longitud_theta"].SetPropValue<dsreal>( StringToReal( p_words[1] ) );
    }
    else if( "init_longitud_phi" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["init_longitud_phi"].SetPropValue<dsreal>( StringToReal( p_words[1] ) );
    }
    else if( "init_alt" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["init_alt"].SetPropValue<dsreal>( StringToReal( p_words[1] ) );
    }
    else if( "init_theta" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["init_theta"].SetPropValue<dsreal>( StringToReal( p_words[1] ) );
    }
    else if( "init_phi" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["init_phi"].SetPropValue<dsreal>( StringToReal( p_words[1] ) );
    }
    else
    {
        _PARSER_UNEXPECTED_KEYWORD_
        return false;
    }

    return true;
}

void LongLatMovement::Serialize( Utils::Archive& p_archive  )
{


}

bool LongLatMovement::Unserialize( Utils::Archive& p_archive )
{
    return true;
}

void LongLatMovement::DumpProperties( dsstring& p_text )
{
    dsstring text_value;

    //p_text = "declare_config ";
    //p_text += dsstring( LONGLATMVT_TEXT_KEYWORD );

    //p_text += "\n";

    /*
    p_text += "configname ";
    p_text += m_properties["configname"].GetPropValue<dsstring>();
    p_text += "\n";
*/

    p_text += "init_longitud_theta ";
    RealToString( m_properties["init_longitud_theta"].GetPropValue<dsreal>(), text_value );
    p_text += "\n";

    p_text += "init_longitud_phi ";
    RealToString( m_properties["init_longitud_phi"].GetPropValue<dsreal>(), text_value );
    p_text += "\n";

    p_text += "init_alt ";
    RealToString( m_properties["init_alt"].GetPropValue<dsreal>(), text_value );
    p_text += "\n";

    p_text += "init_theta ";
    RealToString( m_properties["init_theta"].GetPropValue<dsreal>(), text_value );
    p_text += "\n";

    p_text += "init_phi ";
    RealToString( m_properties["init_phi"].GetPropValue<dsreal>(), text_value );
    p_text += "\n";
    //p_text += "end_config\n";
}

bool LongLatMovement::ParseProperties( const dsstring& p_text )
{
    char seps[] = { 0x09, 0x020, 0x00 };

    return RunOnTextChunk( p_text, seps );
}

void LongLatMovement::ApplyProperties( void )
{
    Init( m_properties["init_longitud_theta"].GetPropValue<dsreal>(), 
            m_properties["init_longitud_phi"].GetPropValue<dsreal>(), 
            m_properties["init_alt"].GetPropValue<dsreal>(), 
            m_properties["init_theta"].GetPropValue<dsreal>(), 
            m_properties["init_phi"].GetPropValue<dsreal>()       
        );

    //m_configname = m_properties["configname"].GetPropValue<dsstring>();
}

Configurable* LongLatMovement::Instanciate( void )
{
    return _DRAWSPACE_NEW_( LongLatMovement, LongLatMovement );
}

