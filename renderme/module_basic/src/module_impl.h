
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

#include "entity.h"
#include "entitygraph.h"
#include "renderingqueue.h"
#include "texture.h"

class ModuleImpl : public renderMe::interfaces::ModuleRoot
{
public:
    ModuleImpl();
    ~ModuleImpl() = default;

    ModuleImpl(const ModuleImpl&) = delete;
    ModuleImpl(ModuleImpl&&) = delete;
    ModuleImpl& operator=(const ModuleImpl& t) = delete;


    std::string                     getModuleName() const;
    std::string                     getModuleDescr() const;

    renderMe::core::Entitygraph*    entitygraph();

    void                            onKeyPress(long p_key);
    void                            onEndKeyPress(long p_key);
    void                            onKeyPulse(long p_key);
    void                            onChar(long p_char, long p_scan);
    void                            onMouseMove(long p_xm, long p_ym, long p_dx, long p_dy);
    void                            onMouseWheel(long p_delta);
    void                            onMouseLeftButtonDown(long p_xm, long p_ym);
    void                            onMouseLeftButtonUp(long p_xm, long p_ym);
    void                            onMouseRightButtonDown(long p_xm, long p_ym);
    void                            onMouseRightButtonUp(long p_xm, long p_ym);
    void                            onAppEvent(WPARAM p_wParam, LPARAM p_lParam);

    void                            init(const std::string p_appWindowsEntityName);
    void                            run(void);
    void                            close(void);

    void                            createEntities(const std::string p_appWindowsEntityName);

    void                            resource_system_events();
    void                            d3d11_system_events();


    //override
    void                            registerSubscriber(const Callback& p_callback);

private:

    static constexpr int                    timeSystemSlot{ 0 };
    static constexpr int                    d3d11SystemSlot{ 1 };
    static constexpr int                    resourceSystemSlot{ 2 };
    static constexpr int                    worldSystemSlot{ 3 };
    static constexpr int                    renderingQueueSystemSlot{ 4 };
    static constexpr int                    dataPrintSystemSlot{ 5 };

    bool                                    m_show_mouse_cursor{ true };
    bool                                    m_mouse_circular_mode{ false };

    renderMe::core::Entitygraph             m_entitygraph;

    renderMe::rendering::Queue*             m_windowRenderingQueue{ nullptr };

    std::string                             m_resources_event{ "..." };

    renderMe::Texture*                      m_rendering_quad_texture{ nullptr };

};




