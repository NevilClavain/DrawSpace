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

#pragma warning( disable : 4005 )

#include <unordered_map>
#include <ds_types.h>
#include <d3dx11.h>

void TranslateD3DD11Error( HRESULT p_hRes, dsstring &p_str )
{	
    static const std::unordered_map<HRESULT, dsstring> translate =
    {
        { S_OK, "S_OK"},
        { S_FALSE, "S_FALSE"},
        { E_NOTIMPL, "E_NOTIMPL"},
        { E_OUTOFMEMORY, "E_OUTOFMEMORY"},
        { E_INVALIDARG, "E_INVALIDARG"},
        { E_FAIL, "E_FAIL"},
        { D3DERR_WASSTILLDRAWING, "D3DERR_WASSTILLDRAWING"},
        { DXGI_ERROR_WAS_STILL_DRAWING, "DXGI_ERROR_WAS_STILL_DRAWING"},
        { D3DERR_INVALIDCALL, "D3DERR_INVALIDCALL"},
        { DXGI_ERROR_INVALID_CALL, "DXGI_ERROR_INVALID_CALL"},
        { D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD, "D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD"},
        { D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS, "D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS"},
        { D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS, "D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS"},
        { D3D11_ERROR_FILE_NOT_FOUND, "D3D11_ERROR_FILE_NOT_FOUND"},
    };
    p_str = translate.at(p_hRes);
}
