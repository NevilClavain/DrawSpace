/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

#include "vsphere.h"

using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

VSphere::VSphere( const dsstring& p_name ) : m_name( p_name ), m_ray( 1.0 )
{
}

VSphere::VSphere( const dsstring& p_name, const Vector& p_point, dsreal p_ray ) : m_name( p_name ), m_ray( p_ray ), m_point( p_point )
{
}

VSphere::~VSphere( void )
{
}

void VSphere::Transform( Matrix& p_mat )
{
    m_mutex.WaitInfinite();
    p_mat.Transform( &m_point, &m_transformed_point );
    m_mutex.Release();
}

bool VSphere::Collide( const Vector& p_testpoint )
{
    Vector delta;

    m_mutex.WaitInfinite();
    delta[0] = p_testpoint[0] - m_transformed_point[0];
    delta[1] = p_testpoint[1] - m_transformed_point[1];
    delta[2] = p_testpoint[2] - m_transformed_point[2];
    m_mutex.Release();

    delta[3] = 1.0;
    if( delta.Length() < m_ray )
    {
        return true;
    }
    return false;
}

void VSphere::SetRay( dsreal p_ray )
{
    m_ray = p_ray;
}

dsreal VSphere::GetRay( void )
{
    return m_ray;
}

void VSphere::SetPoint( const Vector& p_point )
{
    m_point = p_point;
}

void VSphere::GetTransformedPoint( Vector& p_point )
{
    p_point = m_transformed_point;
}
