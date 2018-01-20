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

#include <random>
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

    struct ProceduralBloc abstract
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

    template<typename T>
    struct ValueProceduralBloc abstract : public ProceduralBloc
    {
    protected:
        T m_value { 0 };
    public:

        virtual T GetValue( void ) const
        {
            return m_value;
        }
    };

    template<typename T>
    struct SimpleValueProceduralBloc : public ValueProceduralBloc<T>
    {        
        virtual void SetValue( T p_val )
        {
            m_value = p_val;
        }

        virtual void Evaluate( void ) {};
    };

    template<typename T>
    struct UniformRandomValueProceduralBloc : public ValueProceduralBloc<T>
    {
        ValueProceduralBloc<int>*               m_seed;
        ValueProceduralBloc<T>*                 m_sup;
        ValueProceduralBloc<T>*                 m_inf;

    protected:
        std::default_random_engine              m_generator;        
        std::uniform_int_distribution<T>*       m_distribution;
        bool                                    m_initialized;

    public:

        UniformRandomValueProceduralBloc( void ):
        m_seed( NULL ),
        m_sup( NULL ),
        m_inf( NULL ),
        m_initialized( false )
        {
        }

        ~UniformRandomValueProceduralBloc( void )
        {
            _DRAWSPACE_DELETE_( m_distribution );
        }

        virtual void Evaluate( void )
        {
            if( !m_initialized )
            {
                m_inf->Evaluate();
                m_sup->Evaluate();
                m_seed->Evaluate();

                T inf = m_inf->GetValue();
                T sup = m_sup->GetValue();
                int seed = m_seed->GetValue();

                m_distribution =  _DRAWSPACE_NEW_( std::uniform_int_distribution<T>, std::uniform_int_distribution<T>( inf, sup ) );
                m_generator.seed( seed );
                m_initialized = true;
            }
            m_value = (*m_distribution)( m_generator );
        }
    };

    template<typename T>
    struct RepeatProceduralBloc : public ProceduralBloc
    {
        ValueProceduralBloc<T>*     m_nbIteration;
        ProceduralBloc*             m_action;

        RepeatProceduralBloc( void ) :
        m_nbIteration( NULL ),
        m_action( NULL )
        {
        }

        virtual void Evaluate( void )
        {
            T nb_ite;

            m_nbIteration->Evaluate();
            
            nb_ite = m_nbIteration->GetValue();

            for( T i = 0; i < nb_ite; i++ )
            {
                m_action->Evaluate();
            }
        }
    };
    
    ////////////////////////////


    class ProceduralBlocsFactory
    {
    protected:
        std::unordered_map<dsstring,std::vector<ProceduralBloc*>>               m_procedurals_blocs;
        std::unordered_map<dsstring, RootProceduralBloc*>                       m_procedurals_tree;

    public:

        ~ProceduralBlocsFactory( void )
        {
            CleanAllTreeBlocs();
        }

        RootProceduralBloc* CreateRootBloc( const dsstring& p_procedural_tree_id )
        {
            ProceduralAspect::RootProceduralBloc* rootpb = _DRAWSPACE_NEW_( RootProceduralBloc, RootProceduralBloc );

            m_procedurals_blocs[p_procedural_tree_id].push_back( rootpb );
            m_procedurals_tree[p_procedural_tree_id] = rootpb;
            return rootpb;
        }

        template<typename B>
        B* CreateBloc( const dsstring& p_procedural_tree_id )
        {
            B* procedural_bloc =  _DRAWSPACE_NEW_( B, B );

            m_procedurals_blocs[p_procedural_tree_id].push_back( procedural_bloc );
            return procedural_bloc;
        }

        void CleanTreeBlocs( const dsstring& p_procedural_tree_id )
        {
            if( m_procedurals_blocs.count( p_procedural_tree_id ) )
            {
                for( auto it2 = m_procedurals_blocs[p_procedural_tree_id].begin(); it2 != m_procedurals_blocs[p_procedural_tree_id].end(); ++it2 )
                {
                    _DRAWSPACE_DELETE_( *it2 );
                }
                m_procedurals_blocs.erase( p_procedural_tree_id );
            }

            if( m_procedurals_tree.count( p_procedural_tree_id ) )
            {
                m_procedurals_tree.erase( p_procedural_tree_id );
            }
        }

        void CleanAllTreeBlocs( void )
        {
            for( auto it = m_procedurals_blocs.begin(); it != m_procedurals_blocs.end(); ++it )
            {
                for( auto it2 = it->second.begin(); it2 != it->second.end(); ++it2 )
                {
                    _DRAWSPACE_DELETE_( *it2 );
                }
            }

            m_procedurals_blocs.clear();
            m_procedurals_tree.clear();
        }
    };

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