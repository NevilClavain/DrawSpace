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

#pragma once

#include <mutex>
#include "ds_types.h"
#include "logoutput.h"
#include "file.h"

namespace DrawSpace
{
    namespace Logger
    {
        class OutputFile : public Output
        {
        public:

            OutputFile( const dsstring& p_filename );

            ~OutputFile(void) = default;

            void logIt( const dsstring& p_trace );
            void flush( void );
            void setFlushPeriod( long p_period );    

        private:

            std::unique_ptr<DrawSpace::Utils::File>     m_file;
            long                                        m_flush_period{ 0 };
            long                                        m_period_count{ 0 };
            std::mutex	                                m_mutex;   
        };
    }
}

