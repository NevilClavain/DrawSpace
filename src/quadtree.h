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

#include "ds_types.h"
#include "memalloc.h"
#include "callback.h"

namespace DrawSpace
{
    namespace Commons
    {
        class BaseQuadtreeNode
        {
        public:

            static constexpr int    NorthWestNode = 0;
            static constexpr int    NorthEastNode = 1;
            static constexpr int    SouthEastNode = 2;
            static constexpr int    SouthWestNode = 3;
            static constexpr int    RootNode = 4;

            BaseQuadtreeNode(BaseQuadtreeNode* p_parent, int p_id) :
                m_parent(p_parent),
                m_id(p_id)
            {
            }

            virtual ~BaseQuadtreeNode(void) = default;

            virtual void                setParent(BaseQuadtreeNode* p_parent)
            {
                m_parent = p_parent;
            }

            virtual BaseQuadtreeNode* getParent(void) const
            {
                return m_parent;
            }

            virtual bool                hasChildren(void) const
            {
                return m_splitted;
            }

            virtual BaseQuadtreeNode* getChild(int p_id) const
            {
                return m_children[p_id];
            }

            virtual int                 getId(void) const
            {
                return m_id;
            }

            virtual void                split(void) = 0;
            virtual void                merge(void) = 0;
            virtual void                recursiveMerge(void) = 0;

        protected:

            int                 m_id;
            BaseQuadtreeNode*   m_children[4]{ nullptr, nullptr, nullptr, nullptr };
            BaseQuadtreeNode*   m_parent{ nullptr };
            bool                m_splitted{ false };

        };

        template <typename Base>
        class QuadtreeNode : public BaseQuadtreeNode
        {
        public:

            using InstanciationHandler = Core::BaseCallback<void, BaseQuadtreeNode*>;
            using DeletionHandler = Core::BaseCallback<void, BaseQuadtreeNode*>;
            using SplitHandler = Core::BaseCallback<void, BaseQuadtreeNode*>;
            using MergeHandler = Core::BaseCallback<void, BaseQuadtreeNode*>;

            QuadtreeNode(InstanciationHandler* p_insthandler, DeletionHandler* p_delhandler, SplitHandler* p_splithandler, MergeHandler* p_mergehandler) : BaseQuadtreeNode(NULL, RootNode),
                m_insthandler(p_insthandler),
                m_delhandler(p_delhandler),
                m_splithandler(p_splithandler),
                m_mergehandler(p_mergehandler)
            {
                (*m_insthandler)(this);
            }

            ~QuadtreeNode(void)
            {
                (*m_delhandler)(this);
            }

            Base* getContent(void) const
            {
                return m_content;
            }

            void setContent(Base* p_content)
            {
                m_content = p_content;
            }

            void split(void)
            {
                if (m_splitted)
                {
                    // deja splitte
                    return;
                }

                m_children[NorthWestNode] = _DRAWSPACE_NEW_(QuadtreeNode<Base>, QuadtreeNode<Base>(m_insthandler, m_delhandler, m_splithandler, m_mergehandler, this, NorthWestNode, m_depth_level + 1));
                m_children[NorthEastNode] = _DRAWSPACE_NEW_(QuadtreeNode<Base>, QuadtreeNode<Base>(m_insthandler, m_delhandler, m_splithandler, m_mergehandler, this, NorthEastNode, m_depth_level + 1));
                m_children[SouthEastNode] = _DRAWSPACE_NEW_(QuadtreeNode<Base>, QuadtreeNode<Base>(m_insthandler, m_delhandler, m_splithandler, m_mergehandler, this, SouthEastNode, m_depth_level + 1));
                m_children[SouthWestNode] = _DRAWSPACE_NEW_(QuadtreeNode<Base>, QuadtreeNode<Base>(m_insthandler, m_delhandler, m_splithandler, m_mergehandler, this, SouthWestNode, m_depth_level + 1));

                m_splitted = true;

                (*m_splithandler)(this);
            }

            void merge(void)
            {
                if (!m_splitted)
                {
                    // deja merge
                    return;
                }

                // il ne doit pas y avoir de fils splitte, sinon annulation du split, on ne fait rien
                if (m_children[NorthWestNode]->hasChildren() ||
                    m_children[NorthEastNode]->hasChildren() ||
                    m_children[SouthEastNode]->hasChildren() ||
                    m_children[SouthWestNode]->hasChildren())
                {
                    return;
                }

                (*m_mergehandler)(this);

                _DRAWSPACE_DELETE_(m_children[NorthWestNode]);
                _DRAWSPACE_DELETE_(m_children[NorthEastNode]);
                _DRAWSPACE_DELETE_(m_children[SouthEastNode]);
                _DRAWSPACE_DELETE_(m_children[SouthWestNode]);

                m_splitted = false;
            }

            long getDepthLevel(void)
            {
                return m_depth_level;
            }

            void recursiveMerge(void)
            {
                if (!m_splitted)
                {
                    // leaf reached
                    return;
                }

                for (int i = 0; i < 4; i++)
                {
                    m_children[i]->recursiveMerge();
                }

                (*m_mergehandler)(this);

                for (int i = 0; i < 4; i++)
                {
                    _DRAWSPACE_DELETE_(m_children[i]);
                }

                m_splitted = false;
            }

        private:

            Base* m_content{ nullptr };
            InstanciationHandler* m_insthandler{ nullptr };
            DeletionHandler* m_delhandler{ nullptr };
            SplitHandler* m_splithandler{ nullptr };
            MergeHandler* m_mergehandler{ nullptr };
            long                    m_depth_level{ 0 };

            QuadtreeNode(InstanciationHandler* p_insthandler, DeletionHandler* p_delhandler, SplitHandler* p_splithandler, MergeHandler* p_mergehandler, BaseQuadtreeNode* p_parent, int p_id, long p_depth_level) : BaseQuadtreeNode(p_parent, p_id),
                m_insthandler(p_insthandler),
                m_delhandler(p_delhandler),
                m_splithandler(p_splithandler),
                m_mergehandler(p_mergehandler),
                m_depth_level(p_depth_level)
            {
                (*m_insthandler)(this);
            }
        };
    }    
}

