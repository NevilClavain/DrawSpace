/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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

#include <iostream>

#include "folder.h"

#include "substitution_filenames.h"
#include "substitution_filecontent.h"

int main( void )
{    
    const Folder luaext_template_folder("D:\\dev\\DrawSpace\\tools\\dgent\\templates\\luaext");

    Folder luaext_dest_folder{ luaext_template_folder.CloneTo("D:\\dev\\DrawSpace\\lua_extensions\\foo_luaext") };

    const SubstitutionTable substitution_table =
    {
        { "luaext_name", "foo"}        
    };      

    
    luaext_dest_folder >> SubstitutionContainer<FilenamesSubstitution>(substitution_table)
                        >> SubstitutionContainer<FilecontentSubstitution>(substitution_table);
    
    return 0;
}
