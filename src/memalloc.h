/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

#ifndef _MEMALLOC_H_
#define _MEMALLOC_H_

#include "drawspace_commons.h"
#include "tracedefs.h"

#define _DRAWSPACE_NEW_( __type__, __item__ ) DrawSpace::Utils::MemAlloc::GetInstance()->Register<__type__>( new __item__, sizeof( __type__ ), #__item__, __FUNCTION__, __LINE__, __FILE__ )
#define _DRAWSPACE_NEW_EXPLICIT_SIZE_( __type__, __item__, __size__ ) DrawSpace::Utils::MemAlloc::GetInstance()->Register<__type__>( new __item__, __size__, #__item__, __FUNCTION__, __LINE__, __FILE__ )
#define _DRAWSPACE_NEW_EXPLICIT_SIZE_WITH_COMMENT( __type__, __item__, __size__, __comment__ ) DrawSpace::Utils::MemAlloc::GetInstance()->Register<__type__>( new __item__, __size__, #__item__, __FUNCTION__, __LINE__, __FILE__, __comment__ )

#define _DRAWSPACE_NEW_FROM_MEMMGR_( __mgr__, __type__, __item__ ) __mgr__->Register<__type__>( new __item__, sizeof( __type__ ), #__item__, __FUNCTION__, __LINE__, __FILE__ )

#define _DRAWSPACE_NEW_WITH_COMMENT( __type__, __item__, __comment__ ) DrawSpace::Utils::MemAlloc::GetInstance()->Register<__type__>( new __item__, sizeof( __type__ ), #__item__, __FUNCTION__, __LINE__, __FILE__, __comment__ )

#define _DRAWSPACE_DELETE_( __ptr__ ) delete __ptr__; DrawSpace::Utils::MemAlloc::GetInstance()->Unregister( __ptr__ ); __ptr__ = nullptr
#define _DRAWSPACE_DELETE_N_( __ptr__ ) delete[] __ptr__; DrawSpace::Utils::MemAlloc::GetInstance()->Unregister( __ptr__ ); __ptr__ = nullptr
#define _DRAWSPACE_DELETE_FROM_MEMMGR_( __mgr__, __ptr__ ) delete __ptr__; __mgr__->Unregister( __ptr__ ); __ptr__ = nullptr

namespace DrawSpace
{
namespace Utils
{
class MemAlloc
{
protected:

    typedef struct
    {
        size_t      size;
        dsstring    item;
        dsstring    func;
        long        linenum;
        dsstring    file;
        dsstring    comment;

    } entry;

    static MemAlloc*        m_instance;
    std::map<void*, entry>  m_blocs;
    size_t                  m_totalsize;

    MemAlloc( void );

    void register_bloc( void* p_ptr, size_t p_size, const std::string& p_item, const std::string& p_funcname, long p_line, const std::string& p_filename, const std::string& p_comment );

public:
    
    ~MemAlloc( void );

    static MemAlloc* GetInstance( void );

    void DumpContent( void );

    template <typename base>
    base* Register( base* p_ptr, size_t p_size, const std::string& p_item, const std::string& p_funcname, long p_line, const std::string& p_filename, const std::string& p_comment = "")
    {
        base* t = p_ptr;
        register_bloc( t, p_size, p_item, p_funcname, p_line, p_filename, p_comment);

        return t;
    };

    void Unregister( void* p_ptr );

    size_t GetTotalSize( void ) const;

    static DrawSpace::Logger::Sink* GetLogSink( void );
};
}
}

#endif
