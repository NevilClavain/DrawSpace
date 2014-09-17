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

#ifndef _FACTORY_H_
#define _FACTORY_H_

#include "drawspace_commons.h"
#include "archive.h"
#include "parser.h"
#include "asset.h"

namespace DrawSpace
{
namespace Core
{
class Factory : public DrawSpace::Utils::Parser
{
protected:

    std::map<dsstring, Asset::InstanciateFunc>      m_instanciationfuncs_bytext;
    std::map<long, Asset::InstanciateFunc>          m_instanciationfuncs_byarc;

    std::vector<dsstring>                           m_text_properties;

    bool                                            m_capture_texture_props;
    dsstring                                        m_texture_text_properties;

    static Factory* m_instance;

    Factory( void );

    virtual bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );

public:

    static Factory* GetInstance( void )
    {
        if( NULL == m_instance )
        {
            m_instance = new Factory();
        }

        return m_instance;
    }

    ~Factory( void );

    bool ExecuteFromTextFile( const dsstring& p_path );
    bool ExecuteFromBinaryFile( const dsstring& p_path );

    bool ExecuteFromArchiveChunk( const DrawSpace::Utils::Archive& p_arc );
    bool ExecuteFromTextChunk( const dsstring& p_text );

    void RegisterInstanciationFuncByText( const dsstring& p_keyword, Asset::InstanciateFunc p_func );

};
}
}
#endif