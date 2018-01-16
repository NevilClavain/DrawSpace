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
#include "callback.h"

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
        virtual void Evaluate( void ) = 0;
    };

    struct RootProceduralBloc : public ProceduralBloc
    {
        std::vector<ProceduralBloc*> m_children;

        virtual void Evaluate( void )
        {
            for( size_t i = 0; i < m_children.size(); i++ )
            {
                m_children[i]->Evaluate();
            }
        }
    };

    struct PublishProceduralBloc : public ProceduralBloc
    {
        typedef DrawSpace::Core::BaseCallback<void, const dsstring&> ProceduralPublicationEventHandler;

        dsstring                                        m_id;
        ProceduralBloc*                                 m_toPublish;
        
        std::set<ProceduralPublicationEventHandler*>    m_proc_pub_evt_handlers;

        PublishProceduralBloc( void ) : 
        m_toPublish( NULL )
        {
        }

        virtual void Evaluate( void )
        {
            for( auto it = m_proc_pub_evt_handlers.begin(); it != m_proc_pub_evt_handlers.end(); ++it )
            {
                ( **it )( m_id );
            }
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