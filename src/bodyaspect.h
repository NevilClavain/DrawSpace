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

#ifndef _BODYASPECT_H_
#define _BODYASPECT_H_

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "aspect.h"
#include "bodytransformaspectimpl.h"

#include "matrix.h"
#include "meshe.h"

namespace DrawSpace
{
namespace Aspect
{
class BodyAspect : public Core::Aspect 
{
protected:

    btDefaultMotionState*                                   m_motionState;
    btCollisionShape*                                       m_collisionShape;
    btTriangleMesh*                                         m_mesh;
    btRigidBody*                                            m_rigidBody;

    AspectImplementations::BodyTransformAspectImpl          m_tr_aspectimpl;

    bool                                                    m_body_active;

public:

    struct BoxCollisionShape
    {
        BoxCollisionShape( const Utils::Vector& p_box ) : m_box( p_box ) {};
        Utils::Vector m_box;
    };

    struct SphereCollisionShape
    {
        SphereCollisionShape( dsreal p_ray ) : m_ray( p_ray ) {};
        dsreal m_ray;
    };

    struct MesheCollisionShape
    {
        MesheCollisionShape( const Core::Meshe& p_meshe ) : m_meshe( p_meshe ) {};
        Core::Meshe m_meshe;
    };
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class Force
    {
    public:
        typedef enum
        {
            GLOBAL,
            LOCALE
        } Mode;

    protected:
        DrawSpace::Utils::Vector    m_force_dir;
        dsreal                      m_force_scale;
        Mode                        m_mode;
        bool                        m_enabled;
        
    public:
        Force( const DrawSpace::Utils::Vector& p_dir, Mode p_mode = LOCALE, bool p_enabled = false ) :
        m_force_dir( p_dir ),
        m_mode( p_mode ),
        m_enabled( p_enabled ),
        m_force_scale( 1.0 ) {}

        Force( void ) :
        m_mode( LOCALE ),
        m_enabled( false ),
        m_force_scale( 1.0 ) {}

        inline void UpdateForce( const DrawSpace::Utils::Vector& p_dir ) { m_force_dir = p_dir; }
        inline void UpdateForceScale( dsreal p_scale ) { m_force_scale = p_scale;}
        inline void Enable( void ) { m_enabled = true; }
        inline void Disable( void ) { m_enabled = false; }

        friend class BodyAspect;
    };

    class Torque
    {
    public:
        typedef enum
        {
            GLOBAL,
            LOCALE
        } Mode;

    protected:
        DrawSpace::Utils::Vector    m_torque_axis;
        dsreal                      m_torque_scale;
        Mode                        m_mode;
        bool                        m_enabled;
        
    public:
        Torque( const DrawSpace::Utils::Vector& p_axis, Mode p_mode = LOCALE, bool p_enabled = false ) :
        m_torque_axis( p_axis ),
        m_mode( p_mode ),
        m_enabled( p_enabled ),
        m_torque_scale( 1.0 ) {}

        Torque( void ) :
        m_mode( LOCALE ),
        m_enabled( false ),
        m_torque_scale( 1.0 ) {}

        inline void UpdateForce( const DrawSpace::Utils::Vector& p_axis ) { m_torque_axis = p_axis; }
        inline void UpdateForceScale( dsreal p_scale ) { m_torque_scale = p_scale;}
        inline void Enable( void ) { m_enabled = true; }
        inline void Disable( void ) { m_enabled = false; }

        friend class BodyAspect;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

private:
    void body_state( bool p_enabled );

public:

    BodyAspect( void );
    ~BodyAspect( void );

    btRigidBody* Init( void );
    void Release( void );

    AspectImplementations::BodyTransformAspectImpl* GetTransformAspectImpl( void );

    void Update( void );

    btRigidBody* GetRigidBody( void ) const;

};
}
}

#endif