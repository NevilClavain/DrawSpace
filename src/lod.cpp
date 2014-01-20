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

#include "lod.h"

using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

LodStep::LodStep( dsreal p_kinf, dsreal p_ksup, VSphere* p_vsphere ) : 
m_kinf( p_kinf ),
m_ksup( p_ksup ),
m_vsphere( p_vsphere ),
m_in( false ),
m_handler( NULL )
{
}

LodStep::~LodStep( void )
{
}

void LodStep::Run( void )
{
    Vector transformed_point;   
    m_vsphere->GetTransformedPoint( transformed_point );

    if( - transformed_point[2] < m_ksup * m_vsphere->GetRay() &&
        - transformed_point[2] > m_kinf * m_vsphere->GetRay() )
    {
        if( !m_in )
        {
            if( m_handler )
            {
                (*m_handler)( this, IN_LODSTEP );
            }
            m_in = true;
        }
    }
    else
    {
        if( m_in )
        {
            if( m_handler )
            {
                (*m_handler)( this, OUT_LODSTEP );
            }
            m_in = false;
        }
    }
}

void LodStep::RegisterHandler( BaseCallback2<void, LodStep*, Event>* p_handler )
{
    m_handler = p_handler;
}

void LodStep::SetKInf( dsreal p_kinf )
{
    m_kinf = p_kinf;
}

void LodStep::SetKSup( dsreal p_ksup )
{
    m_ksup = p_ksup;
}
