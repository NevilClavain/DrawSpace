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

#ifndef _RENDERINGNODE_H_
#define _RENDERINGNODE_H_

#include "drawspace_commons.h"
#include "matrix.h"
#include "fx.h"
#include "texture.h"
#include "meshe.h"
#include "callback.h"

namespace DrawSpace
{
namespace Core
{
class RenderingNode
{
public:
    static const int NeutralOrder = 10000;
    static const int NbMaxTextures = 32;

    using ShadersParams = struct
    {
        long						shader_index;
        long						param_register;

        bool						vector; // si true, "param_values" est valide, sinon c'est "mat"
        Utils::Vector				param_values;
        Utils::Matrix				mat;
    };

	using ShadersArrayParam = struct
	{
		long						shader_index;
		long						begin_register;
		std::vector<Utils::Vector>	array;
	};

    dsstring									m_debug_id;

protected:

    Fx*											m_fx;
    Texture*									m_textures[NbMaxTextures]; // 32 textures stages max
    Texture*									m_vertextextures[NbMaxTextures];
    Meshe*										m_meshe;

    std::map<dsstring, ShadersParams*>			m_shader_params;

	std::map<dsstring, ShadersArrayParam*>		m_shaders_array_params;

    long										m_order;

    BaseCallback<void, RenderingNode*>*			m_handler;

    bool										m_drawing_enabled;

public:
    RenderingNode( void );
    virtual ~RenderingNode( void );

    virtual void SetTexture( Texture* p_texture, long p_stage );
    virtual void SetVertexTexture( Texture* p_texture, long p_stage );

    virtual void SetMeshe( Meshe* p_meshe )
    {
        m_meshe = p_meshe;
    }

    virtual Meshe* GetMeshe( void )
    {
        return m_meshe;
    }

    virtual Fx* GetFx( void )
    {
        return m_fx;
    }

    virtual void SetFx( Fx* p_fx )
    {
        m_fx = p_fx;
    }


    static  long GetTextureListSize( void );
    virtual Texture* GetTexture( long p_index );
    virtual Texture* GetVertexTexture( long p_index );

    virtual void OnDraw( void );
    virtual void RegisterHandler( BaseCallback<void, RenderingNode*>* p_handler );
    virtual long GetOrderNumber( void );
    virtual void SetOrderNumber( long p_order );

    virtual void AddShaderParameter( long p_shader_index, const dsstring& p_id, long p_register );
    virtual void SetShaderReal( const dsstring& p_id, dsreal p_value );
    virtual void SetShaderRealVector( const dsstring& p_id, const Utils::Vector& p_value );
	virtual void SetShaderRealInVector(const dsstring& p_id, int p_index_in_vector, dsreal p_value );
    virtual void SetShaderRealMatrix( const dsstring& p_id, const Utils::Matrix& p_value );
    virtual void SetShaderBool( const dsstring& p_id, bool p_value );
    virtual void CleanupShaderParams( void );

	virtual void AddShaderArrayParameter(long p_shader_index, const dsstring& p_id, long p_begin_register);
	virtual void SetShaderArrayParameter(const dsstring& p_id, const std::vector<Utils::Vector>& p_array);
	virtual void CleanupShaderArrayParams(void);

    virtual void UpdateShaderParams( const dsstring& p_id, ShadersParams& p_params );

    virtual void GetShadersParams( std::map<dsstring, ShadersParams*>& p_outlist );

	virtual void GetShadersArrayParams(std::map<dsstring, ShadersArrayParam*>& p_outlist);

    virtual void SetDrawingState( bool p_drawing );

    friend class RenderingQueue;
};
}
}

#endif
