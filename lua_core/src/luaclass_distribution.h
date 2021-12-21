/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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

#pragma once


#include <random>
#include "luna.h"

class DistributionWrapper abstract {};

template<typename C>
class DistributionWrapperImpl : public DistributionWrapper
{
private:
    std::unique_ptr<C> m_distribution;

public:
    template<class... Args>
    DistributionWrapperImpl(Args&&... p_args)
    {
        m_distribution = std::make_unique<C>((std::forward<Args>(p_args))...);
    }
    
    template<typename T>
    T Generate( std::default_random_engine& p_generator )
    {        
        return (*m_distribution)(p_generator);
    }    
};

class LuaClass_Distribution
{
private:
    std::unique_ptr<DistributionWrapper>    m_distribution;
    dsstring                                m_distribution_type;

public:
    LuaClass_Distribution(lua_State* p_L);
    ~LuaClass_Distribution(void);

    int LUA_generate(lua_State* p_L);

    static const char className[];
    static const typename Luna<LuaClass_Distribution>::RegType methods[];
};
