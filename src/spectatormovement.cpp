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

#include "spectatormovement.h"
#include "maths.h"
#include "misc_utils.h"
#include "configsbase.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;


SpectatorMovement::SpectatorMovement( void ) :
m_attachedbody( NULL ),
m_linked_to_orbiter( false )
{
    // properties array creation
    //m_properties["configname"].AddPropValue<dsstring>( m_configname );
    m_properties["refbody"].AddPropValue<dsstring>( "" );
    m_properties["scale_pos"].AddPropValue<dsreal>( 1.0 );
    m_properties["period"].AddPropValue<long>( 10 );
    m_properties["orbiter_link"].AddPropValue<bool>( false );


    m_timercb = _DRAWSPACE_NEW_( SpectatorTimer, SpectatorTimer( this, &SpectatorMovement::on_timer ) );
}

SpectatorMovement::~SpectatorMovement( void )
{
    _DRAWSPACE_DELETE_( m_timercb );
}

void SpectatorMovement::compute_pos( void )
{
    Vector body_speed;
    m_attachedbody->GetLinearSpeed( body_speed );

    Vector body_speed_2;

    Vector camera_position;
    Vector direction;

    Matrix body_transf;
    Matrix body_transf_inv;

    if( m_linked_to_orbiter )
    {
        m_attachedbody->GetLastLocalWorldTrans( body_transf );
    }
    else
    {
        m_attachedbody->GetLastWorldTransformation( body_transf );
    }

    body_transf_inv = body_transf;
    body_transf_inv.ClearTranslation();
    body_transf_inv.Inverse();

    body_transf_inv.Transform( &body_speed, &body_speed_2 );



    dsreal ray = m_attachedbody->GetBoundingSphereRay();
    ray *= m_scalepos;

    if( 0.0 == body_speed_2.LengthPow2() )
    {
        direction[0] = 0.5;
        direction[1] = 0.0;
        direction[2] = -1.0;

        direction.Normalize();
        direction.Scale( ray );
    }    
    else
    {
        Vector n_body_speed = body_speed_2;
        n_body_speed.Normalize();

        Vector y_axis( 0.0, 1.0, 0.0, 1.0 );
        Vector z_axis( 0.0, 0.0, 1.0, 1.0 );
        Vector lateral;

        if( y_axis * n_body_speed > 0.5 )
        {
            // vitesse presque colineaire a l'axe y
            lateral = ProdVec( z_axis, n_body_speed );
        }
        else
        {
            // vitesse pas vraiment colineaire a l'axe y
            lateral = ProdVec( y_axis, n_body_speed );                       
        }

        lateral.Normalize();
        lateral.Scale( 0.25 );

        direction[0] = n_body_speed[0] + lateral[0];
        direction[1] = n_body_speed[1] + lateral[1];
        direction[2] = n_body_speed[2] + lateral[2];

        direction.Normalize();
        direction.Scale( ray );

    }



    Matrix translate;

    translate.Translation( direction );
     
    m_result = translate * body_transf;
}

void SpectatorMovement::SetName( const dsstring& p_name )
{
    m_name = p_name;
}

void SpectatorMovement::Init( InertBody* p_attachedbody, dsreal p_scalepos, long p_posperiod, bool p_orbiterlink )
{
    m_linked_to_orbiter = p_orbiterlink;

    m_attachedbody = p_attachedbody;

    m_scalepos = p_scalepos;
    m_posperiod = p_posperiod;

    // pour executer compute_pos() des le 1er appel a SpectatorMovement::Compute(); les appels suivants seront fait periodiquement
    // sur appel timer manager a on_timer()
    m_compute = true;

    
}

void SpectatorMovement::Compute( TimeManager& p_timemanager )
{
    if( m_compute )
    {
        
        dsstring timer_name;

        if( m_name != "" )
        {
            timer_name = m_name + "_spectatormvt";
        }
        else
        {
            _DSEXCEPTION( "spectator mvt name is empty !" );
        }

        p_timemanager.AddTimer( timer_name, m_posperiod, m_timercb );

        p_timemanager.SetTimerState( timer_name, true );

        compute_pos();
        m_compute = false;
    }    
}

void SpectatorMovement::on_timer( const dsstring& p_timername )
{
    m_compute = true;
}

bool SpectatorMovement::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
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
    else*/ if( "refbody" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["refbody"].SetPropValue<dsstring>( p_words[1] );
    }
    else if( "scale_pos" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["scale_pos"].SetPropValue<dsreal>( StringToReal( p_words[1] ) );
    }
    else if( "period" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["period"].SetPropValue<long>( StringToInt( p_words[1] ) );
    }
    else if( "orbiter_link" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }
        m_properties["orbiter_link"].SetPropValue<bool>( ( "true" == p_words[1] ? true : false ) );
    }
    else
    {
        _PARSER_UNEXPECTED_KEYWORD_
        return false;
    }

    return true;
}

void SpectatorMovement::Serialize( Utils::Archive& p_archive  )
{
}

bool SpectatorMovement::Unserialize( Utils::Archive& p_archive )
{
    return true;
}

void SpectatorMovement::DumpProperties( dsstring& p_text )
{
    dsstring text_value;

    p_text = "declare_config ";
    p_text += dsstring( SPECTATORMVT_TEXT_KEYWORD );

    p_text += "\n";
/*
    p_text += "configname ";
    p_text += m_properties["configname"].GetPropValue<dsstring>();
    p_text += "\n";
*/
    p_text += "refbody ";
    p_text += m_properties["refbody"].GetPropValue<dsstring>();
    p_text += "\n";


    p_text += "scale_pos ";
    RealToString( m_properties["scale_pos"].GetPropValue<dsreal>(), text_value );
    p_text += "\n";

    p_text += "period ";
    IntToString( m_properties["period"].GetPropValue<long>(), text_value );
    p_text += "\n";

    p_text += "orbiter_link ";
    p_text += ( true == m_properties["orbiter_link"].GetPropValue<bool>() ? "true" : "false" );
    p_text += "\n";

    p_text += "end_config\n";
}

bool SpectatorMovement::ParseProperties( const dsstring& p_text )
{
    char seps[] = { 0x09, 0x020, 0x00 };

    return RunOnTextChunk( p_text, seps );
}

void SpectatorMovement::ApplyProperties( void )
{
    dsstring refbody = m_properties["refbody"].GetPropValue<dsstring>();

    if( false == ConfigsBase::GetInstance()->ConfigIdExists( refbody ) )
    {
        _DSEXCEPTION( "Config id unknown in ConfigsBase" );
    }

    Configurable* config = ConfigsBase::GetInstance()->GetConfigurable( refbody );

    DrawSpace::Dynamics::InertBody* body = dynamic_cast<DrawSpace::Dynamics::InertBody*>( config );
    if( !body )
    {
        _DSEXCEPTION( "Specified asset is not an Dynamics::InertBody" );
    }

    Init( body, m_properties["scale_pos"].GetPropValue<dsreal>(), 
                m_properties["period"].GetPropValue<long>(),
                m_properties["orbiter_link"].GetPropValue<bool>() );
                
    //m_configname = m_properties["configname"].GetPropValue<dsstring>();
}

Configurable* SpectatorMovement::Instanciate( void )
{
    return _DRAWSPACE_NEW_( SpectatorMovement, SpectatorMovement );
}
