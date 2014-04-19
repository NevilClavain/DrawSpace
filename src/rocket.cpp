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

#include "rocket.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;

Rocket::Rocket( World* p_world, TransformNode* p_drawable, const Body::Parameters& p_parameters ) : InertBody( p_world, p_drawable, p_parameters )
{
    m_fwd[0] = 0.0;
    m_fwd[1] = 0.0;
    m_fwd[2] = -1.0;

    m_rev[0] = 0.0;
    m_rev[1] = 0.0;
    m_rev[2] = 1.0;

    m_right[0] = 1.0;
    m_right[1] = 0.0;
    m_right[2] = 0.0;

    m_left[0] = -1.0;
    m_left[1] = 0.0;
    m_left[2] = 0.0;

    m_up[0] = 0.0;
    m_up[1] = 1.0;
    m_up[2] = 0.0;

    m_down[0] = 0.0;
    m_down[1] = -1.0;
    m_down[2] = 0.0;

    m_transformed_fwd = m_fwd;
    m_transformed_left = m_left;
    m_transformed_right = m_right;
    m_transformed_up = m_up;
    m_transformed_down = m_down;
}

Rocket::~Rocket( void )
{
}

void Rocket::Update( void )
{
    InertBody::Update();

    Matrix local_rot = m_lastlocalworldtrans;
    local_rot.ClearTranslation();
    local_rot.Transform( &m_fwd, &m_transformed_fwd );
    local_rot.Transform( &m_rev, &m_transformed_rev );
    local_rot.Transform( &m_left, &m_transformed_left );
    local_rot.Transform( &m_right, &m_transformed_right );
    local_rot.Transform( &m_up, &m_transformed_up );
    local_rot.Transform( &m_down, &m_transformed_down );
}

void Rocket::ApplyFwdForce( dsreal p_norm )
{
    dsreal world_scale = World::m_scale;

    m_rigidBody->applyForce( btVector3( m_transformed_fwd[0] * p_norm, 
                                m_transformed_fwd[1] * p_norm, m_transformed_fwd[2] * p_norm ), 
                                btVector3( 0.0, 0.0, 0.0 ) );
}

void Rocket::ApplyRevForce( dsreal p_norm )
{
    dsreal world_scale = World::m_scale;

    m_rigidBody->applyForce( btVector3( m_rev[0] * p_norm * world_scale, 
                                m_rev[1] * p_norm * world_scale, m_rev[2] * p_norm * world_scale ), 
                                btVector3( 0.0, 0.0, 0.0 ) );
}

void Rocket::ApplyLeftYaw( dsreal p_norm )
{
    dsreal world_scale = World::m_scale;
    dsreal world_scale_2 = world_scale * world_scale;

    m_rigidBody->applyTorque( btVector3( m_transformed_up[0] * p_norm * world_scale_2, m_transformed_up[1] * p_norm * world_scale_2, m_transformed_up[2] * p_norm * world_scale_2 ) );
}

void Rocket::ApplyRightYaw( dsreal p_norm )
{
    dsreal world_scale = World::m_scale;
    dsreal world_scale_2 = world_scale * world_scale;

    m_rigidBody->applyTorque( btVector3( m_transformed_down[0] * p_norm * world_scale_2, m_transformed_down[1] * p_norm * world_scale_2, m_transformed_down[2] * p_norm * world_scale_2 ) );
}

void Rocket::ApplyUpPitch( dsreal p_norm )
{
    dsreal world_scale = World::m_scale;
    dsreal world_scale_2 = world_scale * world_scale;

    m_rigidBody->applyTorque( btVector3( m_transformed_left[0] * p_norm * world_scale_2, m_transformed_left[1] * p_norm * world_scale_2, m_transformed_left[2] * p_norm * world_scale_2 ) );
}

void Rocket::ApplyDownPitch( dsreal p_norm )
{
    dsreal world_scale = World::m_scale;
    dsreal world_scale_2 = world_scale * world_scale;

    m_rigidBody->applyTorque( btVector3( m_transformed_right[0] * p_norm * world_scale_2, m_transformed_right[1] * p_norm * world_scale_2, m_transformed_right[2] * p_norm * world_scale_2 ) );
}

void Rocket::ApplyLeftRoll( dsreal p_norm )
{
    dsreal world_scale = World::m_scale;
    dsreal world_scale_2 = world_scale * world_scale;

    m_rigidBody->applyTorque( btVector3( m_transformed_rev[0] * p_norm * world_scale_2, m_transformed_rev[1] * p_norm * world_scale_2, m_transformed_rev[2] * p_norm * world_scale_2 ) );   
}

void Rocket::ApplyRightRoll( dsreal p_norm )
{
    dsreal world_scale = World::m_scale;
    dsreal world_scale_2 = world_scale * world_scale;

    m_rigidBody->applyTorque( btVector3( m_transformed_fwd[0] * p_norm * world_scale_2, m_transformed_fwd[1] * p_norm * world_scale_2, m_transformed_fwd[2] * p_norm * world_scale_2 ) );
}

void Rocket::ZeroSpeed( void )
{
    m_rigidBody->setAngularVelocity( btVector3( 0.0, 0.0, 0.0 ) );
    m_rigidBody->setLinearVelocity( btVector3( 0.0, 0.0, 0.0 ) );
}