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

namespace DrawSpace
{
namespace Aspect
{
class ProceduralAspect : public Core::Aspect 
{
public:
    struct ProceduralBloc abstract
    {
        std::vector<ProceduralBloc*> m_args;        
        virtual void Evaluate( void ) = 0;
        virtual void Reset( void ) 
        {
            for(size_t i = 0; i < m_args.size(); i++ )
            {
                m_args[i]->Reset();
            }
        };
        
        virtual void Terminate( void ) 
        {
            for(size_t i = 0; i < m_args.size(); i++ )
            {
                m_args[i]->Terminate();
            }
        };
    };

    struct RootProceduralBloc : public ProceduralBloc
    {
        virtual void Evaluate( void )
        {
            for( size_t i = 0; i < m_args.size(); i++ )
            {
                m_args[i]->Evaluate();
            }
        }
    };

    template<typename T>
    struct ValueProceduralBloc abstract : public ProceduralBloc
    {
    protected:
        T m_value;// { 0 };
    public:

        virtual T GetValue( void ) const
        {
            return m_value;
        }
    };

    template<typename T>
    struct SimpleValueProceduralBloc : public ValueProceduralBloc<T>
    {
        SimpleValueProceduralBloc( void )
        {
        }

        SimpleValueProceduralBloc( T p_val )
        {
            m_value = p_val;
        }

        virtual void Evaluate( void ) {};
    };

    template<typename T>
    struct UniformRandomValueProceduralBloc : public ValueProceduralBloc<T>
    {
    protected:
        std::default_random_engine              m_generator;        
        std::uniform_int_distribution<T>*       m_distribution;

    public:

        UniformRandomValueProceduralBloc( void ) :
        m_distribution( NULL )
        {
        }

        ~UniformRandomValueProceduralBloc( void )
        {
            Terminate();
        }

        virtual void Reset( void ) 
        {
            ProceduralBloc::Reset();

            ValueProceduralBloc<int>* seed_a = static_cast<ValueProceduralBloc<int>*>( m_args[0] );
            ValueProceduralBloc<T>* sup_a = static_cast<ValueProceduralBloc<T>*>( m_args[1] );
            ValueProceduralBloc<T>* inf_a = static_cast<ValueProceduralBloc<T>*>( m_args[2] );

            inf_a->Evaluate();
            sup_a->Evaluate();
            seed_a->Evaluate();

            T inf = inf_a->GetValue();
            T sup = sup_a->GetValue();
            int seed = seed_a->GetValue();

            m_distribution = _DRAWSPACE_NEW_( std::uniform_int_distribution<T>, std::uniform_int_distribution<T>( inf, sup ) );
            m_generator.seed( seed );
        };
        
        virtual void Terminate( void ) 
        {
            ProceduralBloc::Terminate();

            if( m_distribution )
            {
                 _DRAWSPACE_DELETE_( m_distribution );
                 m_distribution = NULL;            
            }            
        };

        virtual void Evaluate( void )
        {
            m_value = (*m_distribution)( m_generator );
        }
    };

    template<typename T>
    struct ArrayProceduralBloc : public ValueProceduralBloc<T>
    {
    protected:
        std::vector<T>  m_array;
        
    public:

        void PushValue( T p_value )
        {
            m_array.push_back( p_value );
        }

        virtual void Evaluate( void ) 
        {
            ValueProceduralBloc<int>* index_a = static_cast<ValueProceduralBloc<int>*>( m_args[0] );

            index_a->Evaluate();
            int index = index_a->GetValue();
            m_value = m_array[index];
        };
    };
    
    struct SeedSourceProceduralBloc : public ValueProceduralBloc<int>
    { 
    public:
        static int m_seed;

    protected:
        static const int SeedMin = 1;
        static const int SeedMax= 999999;

        std::default_random_engine              m_generator;        
        std::uniform_int_distribution<int>*     m_distribution;

    public:

        SeedSourceProceduralBloc( void ) :
        m_distribution( NULL )
        {
        }

        ~SeedSourceProceduralBloc( void )
        {
            Terminate();
        }

        virtual void Reset( void ) 
        {
            ProceduralBloc::Reset();
            m_distribution = _DRAWSPACE_NEW_( std::uniform_int_distribution<int>, std::uniform_int_distribution<int>( SeedMin, SeedMax ) );
            m_generator.seed( m_seed );
        };
        
        virtual void Terminate( void ) 
        {
            ProceduralBloc::Terminate();

            if( m_distribution )
            {
                 _DRAWSPACE_DELETE_( m_distribution );
                 m_distribution = NULL;            
            }            
        };

        virtual void Evaluate( void )
        {
            m_value = (*m_distribution)( m_generator );
        }
    };


    template<typename T>
    struct RepeatProceduralBloc : public ProceduralBloc
    {
        RepeatProceduralBloc( void )
        {
        }

        virtual void Evaluate( void )
        {
            ValueProceduralBloc<T>* nb_iteration_a = static_cast<ValueProceduralBloc<T>*>( m_args[0] );
            std::vector<ProceduralBloc*> actions_a;

            for( size_t i = 1; i < m_args.size(); i++ )
            {
                actions_a.push_back( m_args[i] );
            }

            T nb_ite;

            nb_iteration_a->Evaluate();            
            nb_ite = nb_iteration_a->GetValue();

            for( T i = 0; i < nb_ite; i++ )
            {
                for( size_t i2 = 0; i2 < actions_a.size(); ++i2 )
                {
                    actions_a[i2]->Evaluate();
                }
            }
        }
    };

    struct PublishProceduralBloc : public ProceduralBloc
    {
        typedef DrawSpace::Core::BaseCallback2<void, const dsstring&, ProceduralBloc*> ProceduralPublicationEventHandler;

        dsstring                                        m_id;
        
        std::set<ProceduralPublicationEventHandler*>    m_proc_pub_evt_handlers;

        PublishProceduralBloc( void )
        {
        }

        PublishProceduralBloc( const dsstring& p_id ) : m_id( p_id )
        {
        }

        virtual void Evaluate( void )
        {
            ProceduralBloc* arg = NULL;

            if( m_args.size() > 0 )
            {
                arg = m_args[0];
                arg->Evaluate();
            }

            for( auto it = m_proc_pub_evt_handlers.begin(); it != m_proc_pub_evt_handlers.end(); ++it )
            {
                ( **it )( m_id, arg );
            }
        }
    };

    
    ////////////////////////////


    class ProceduralBlocsFactory
    {
    protected:
        std::unordered_map<dsstring,std::vector<ProceduralBloc*>>               m_procedurals_blocs;
       
    public:

        ~ProceduralBlocsFactory( void )
        {
            CleanAllTreeBlocs();
        }

        RootProceduralBloc* CreateRootBloc( const dsstring& p_procedural_tree_id )
        {
            ProceduralAspect::RootProceduralBloc* rootpb = _DRAWSPACE_NEW_( RootProceduralBloc, RootProceduralBloc );

            m_procedurals_blocs[p_procedural_tree_id].push_back( rootpb );
            return rootpb;
        }

        template<typename B, class... Args>
        B* CreateBloc( const dsstring& p_procedural_tree_id, Args&&... p_args )
        {
            B* procedural_bloc =  _DRAWSPACE_NEW_( B, B(p_args...) );

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
        }
    };

protected:   
    bool m_to_update;
    
public:
    ProceduralAspect( void );

    void SetToUpdate( bool p_state );
    bool GetToUpdate( void ) const;

};



}
}

#endif