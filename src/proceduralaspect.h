/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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

#ifndef _PROCEDURALASPECT_H_
#define _PROCEDURALASPECT_H_

#include "aspect.h"
#include "entity.h"

#define PROCEDURALBLOCID( __type__) typeid(__type__).hash_code()

namespace DrawSpace
{
namespace Aspect
{
class ProceduralAspect : public Core::Aspect 
{
public:

    typedef enum
    {
        ARG0,
        ARG1,
        ARG2,
        ARG3,
        ARG4
    
    } Args;

    struct ProceduralBloc
    {
        std::vector<ProceduralBloc*> m_children;

        virtual void Evaluate( void ) = 0;
    };

    struct RootProceduralBloc : public ProceduralBloc
    {
        virtual void Evaluate( void )
        {
        }
    };

    struct PublishProceduralBloc : public ProceduralBloc
    {
        dsstring        m_id;
        ProceduralBloc* m_toPublish;

        PublishProceduralBloc( void ) : 
        m_toPublish( NULL )
        {
        }

        virtual void Evaluate( void )
        {
        }
    };

    struct RepeatProceduralBloc : public ProceduralBloc
    {

        ProceduralBloc* m_nbIteration;
        ProceduralBloc* m_action;

        RepeatProceduralBloc( void ) :
        m_nbIteration( NULL ),
        m_action( NULL )
        {
        }

        virtual void Evaluate( void )
        {
        }
    };

    template<typename T>
    struct ValueProceduralBloc : public ProceduralBloc
    {
        T m_value;

        virtual void Evaluate( void ) {};
        T GetValue( void )
        {
            return m_value;
        }
    };


    ////////////////////////////


protected:
    
    bool m_to_update;
    
    void update( void );

public:
    ProceduralAspect( void );

    void SetToUpdate( bool p_state );
    bool GetToUpdate( void ) const;

    void Run( Core::Entity* p_parent, Core::Entity* p_entity  );
};



}
}

#endif