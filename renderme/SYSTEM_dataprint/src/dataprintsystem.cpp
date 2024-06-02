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

#include <string>
#include <unordered_map>

#include "dataprintsystem.h"
#include "entity.h"
#include "entitygraph.h"
#include "aspects.h"
#include "ecshelpers.h"
#include "datacloud.h"
#include "tvector.h"
#include "renderingqueue.h"
#include "syncvariable.h"

using namespace renderMe;
using namespace renderMe::core;

DataPrintSystem::DataPrintSystem(Entitygraph& p_entitygraph) : System(p_entitygraph)
{
	
	// TEMP
	for (int i = 0; i < 200; i++)
	{		
		m_sv_strings.push_back(std::string( "test " + std::to_string(i + 1) ));
	}
	
}

void DataPrintSystem::run()
{
	collectData();

	print(m_dc_strings, 0, dcTextsIdBase, dcNbCols, dcNbRows, dcColWidth, dcRowHeight);

	// positioning sync vars print bloc at bottomo of the window : compute y position

	const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };

	const auto window_dims{ dataCloud->readDataValue<renderMe::core::maths::IntCoords2D>("std.window_resol") };

	const int y_pos = window_dims[1] - (svNbRows * svRowHeight);

	print(m_sv_strings, y_pos, svTextsIdBase, svNbCols, svNbRows, svColWidth, svRowHeight);
}

void DataPrintSystem::setRenderingQueue(renderMe::rendering::Queue* p_queue)
{
	m_renderingQueue = p_queue;
}

void DataPrintSystem::collectData()
{
	/////// collect datacloud vars

	m_dc_strings.clear();

	const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };
	const auto dataCloudVariables{ dataCloud->getVarsIdsList() };

	for (const auto& e : dataCloudVariables)
	{
		const std::string var_id{ e.first };
		const size_t type_id{ e.second };

		std::string var_str_value;

		std::unordered_map<size_t, std::function<void(const std::string&)>> conv_funcs
		{
			{
				typeid(long).hash_code(),
				[&](const std::string& p_id)
				{
					const auto value { dataCloud->readDataValue<long>(p_id) };
					var_str_value = "dc." + p_id + " " + std::to_string(value);
				}
			},
			{
				typeid(int).hash_code(),
				[&](const std::string& p_id)
				{
					const auto value { dataCloud->readDataValue<int>(p_id) };
					var_str_value = "dc." + p_id + " " + std::to_string(value);
				}
			},
			{
				typeid(unsigned long).hash_code(),
				[&](const std::string& p_id)
				{
					const auto value { dataCloud->readDataValue<unsigned long>(p_id) };
					var_str_value = "dc." + p_id + " " + std::to_string(value);
				}
			},
			{
				typeid(unsigned int).hash_code(),
				[&](const std::string& p_id)
				{
					const auto value { dataCloud->readDataValue<unsigned int>(p_id) };
					var_str_value = "dc." + p_id + " " + std::to_string(value);
				}
			},
			{
				typeid(size_t).hash_code(),
				[&](const std::string& p_id)
				{
					const auto value { dataCloud->readDataValue<size_t>(p_id) };
					var_str_value = "dc." + p_id + " " + std::to_string(value);
				}
			},
			{
				typeid(float).hash_code(),
				[&](const std::string& p_id)
				{
					const auto value { dataCloud->readDataValue<float>(p_id) };
					var_str_value = "dc." + p_id + " " + std::to_string(value);
				}
			},
			{
				typeid(double).hash_code(),
				[&](const std::string& p_id)
				{
					const auto value { dataCloud->readDataValue<double>(p_id) };
					var_str_value = "dc." + p_id + " " + std::to_string(value);
				}
			},
			{
				typeid(core::maths::IntCoords2D).hash_code(),
				[&](const std::string& p_id)
				{
					const auto value { dataCloud->readDataValue<core::maths::IntCoords2D>(p_id) };
					var_str_value = "dc." + p_id + " " + "[ " + std::to_string(value[0]) + " " + std::to_string(value[1]) + " ]";
				}
			},
			{
				typeid(core::maths::FloatCoords2D).hash_code(),
				[&](const std::string& p_id)
				{
					const auto value { dataCloud->readDataValue<core::maths::FloatCoords2D>(p_id) };
					var_str_value = "dc." + p_id + " " + "[ " + std::to_string(value[0]) + " " + std::to_string(value[1]) + " ]";
				}
			},
			{
				typeid(core::maths::Real3Vector).hash_code(),
				[&](const std::string& p_id)
				{
					const auto value { dataCloud->readDataValue<core::maths::Real3Vector>(p_id) };
					var_str_value = "dc." + p_id + " " + "[ " + std::to_string(value[0]) + " " + std::to_string(value[1]) + " " + std::to_string(value[2]) + " ]";
				}
			},
			{
				typeid(core::maths::Real4Vector).hash_code(),
				[&](const std::string& p_id)
				{
					const auto value { dataCloud->readDataValue<core::maths::Real4Vector>(p_id) };
					var_str_value = "dc." + p_id + " " + "[ " + std::to_string(value[0]) + " " + std::to_string(value[1]) + " " + std::to_string(value[2]) + " " + std::to_string(value[3]) + " ]";
				}
			},
			{
				typeid(std::string).hash_code(),
				[&](const std::string& p_id)
				{
					const auto value { dataCloud->readDataValue<std::string>(p_id) };
					var_str_value = "dc." + p_id + " " + value;
				}
			}

		};


		/////////////////////////////////////////////////////////////

		if (conv_funcs.count(type_id))
		{
			conv_funcs.at(type_id)(var_id);
		}
		else
		{
			// cannot infer type

			var_str_value = var_id + " <unknown type>";
		}

		m_dc_strings.push_back(var_str_value);
	}

	/////// collect sync vars

	m_sv_strings.clear();

	const auto forEachTimeAspect
	{
		[&](Entity* p_entity, const ComponentContainer& p_time_aspect)
		{
			const auto comps { p_time_aspect.getComponentsIdList() };
			const size_t sv_hash{ typeid(core::SyncVariable).hash_code() };

			for (const auto& e : comps)
			{
				if (e.second == sv_hash)
				{
					const std::string id{ e.first };

					const auto& sync_var{ p_time_aspect.getComponent<core::SyncVariable>(id)->getPurpose() };

					const std::string type { core::SyncVariable::Type::ANGLE == sync_var.type ? "ANGLE" : "POS" };


					//const std::string direction { SyncVariable::Direction::INC == sync_var.direction ? "INC" : "DEC" };

					std::string direction{ "?" };

					if (SyncVariable::Direction::INC == sync_var.direction)
					{
						direction = "INC";
					}
					else if (SyncVariable::Direction::DEC == sync_var.direction)
					{
						direction = "DEC";
					}
					else // ZERO
					{
						direction = "ZERO";
					}



					const std::string value { std::to_string(sync_var.value) };
					const std::string step { std::to_string(sync_var.step) };
					const std::string var_str_value{ "syncv." + id + " = " + value + "(" + direction + " " + step + ")"};

					m_sv_strings.push_back(var_str_value);
				}
			}
		}
	};

	renderMe::helpers::extractAspectsTopDown<renderMe::core::timeAspect>(m_entitygraph, forEachTimeAspect);

	/////// collect rendering queues

	const auto forEachRenderingAspect
	{
		[&](Entity* p_entity, const ComponentContainer& p_rendering_aspect)
		{
			const auto rendering_queues_list { p_rendering_aspect.getComponentsByType<rendering::Queue>() };
			if (rendering_queues_list.size() > 0)
			{
				auto& renderingQueue{ rendering_queues_list.at(0)->getPurpose() };

			}
		}
	};

	renderMe::helpers::extractAspectsTopDown<renderMe::core::renderingAspect>(m_entitygraph, forEachRenderingAspect);
}

void DataPrintSystem::print(const std::vector<std::string>& p_list, int p_y_base, int p_id_base, int p_nbCols, int p_nbRows, int p_colWidth, int p_rowHeight)
{
	int curr_row{ 0 };
	int curr_col{ 0 };
	int index{ 0 };

	for (const auto& e : p_list)
	{
		m_renderingQueue->setText(p_id_base + index, { e, "CourierNew.10.spritefont", {255, 100, 100, 255}, {curr_col * p_colWidth, curr_row * p_rowHeight + p_y_base}, 0.0});
		index++;

		curr_col++;
		if (p_nbCols == curr_col)
		{
			curr_col = 0;
			curr_row++;

			if (p_nbRows == curr_row)
			{
				break;
			}
		}
	}
}
