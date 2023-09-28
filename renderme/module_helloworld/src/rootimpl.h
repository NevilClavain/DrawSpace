
/* -*-LIC_BEGIN-*- */
/*
*
* renderMe grafx framework
* Emmanuel Chaumont Copyright (c) 2013-2023
*
* This file is part of renderMe.
*
*    renderMe is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    renderMe is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with renderMe.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#pragma once

#include "module_root.h"
#include "componentcontainer.h"


class RootImpl : public renderMe::interface::ModuleRoot
{
public:
    RootImpl() = default;
    ~RootImpl() = default;

    RootImpl(const RootImpl&) = delete;
    RootImpl(RootImpl&&) = delete;
    RootImpl& operator=(const RootImpl& t) = delete;


    std::string                                     getModuleName() const;
    std::string                                     getModuleDescr() const;
    const renderMe::core::ComponentContainer&       getComponentContainer() const;

private:

    renderMe::core::ComponentContainer m_component_container;
};




