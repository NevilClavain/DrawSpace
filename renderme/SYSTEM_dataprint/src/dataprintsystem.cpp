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

using namespace renderMe;
using namespace renderMe::core;

DataPrintSystem::DataPrintSystem(Entitygraph& p_entitygraph) : System(p_entitygraph)
{
	// TEMP
	for (int i = 0; i < 200; i++)
	{		
		m_strings.push_back(std::string( "test " + std::to_string(i + 1) ));
	}
}

void DataPrintSystem::run()
{
	collectData();
	print();
}

void DataPrintSystem::setRenderingQueue(renderMe::rendering::Queue* p_queue)
{
	m_renderingQueue = p_queue;
}

void DataPrintSystem::collectData()
{
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
					var_str_value = std::to_string(value);
				}
			},
			{
				typeid(int).hash_code(),
				[&](const std::string& p_id)
				{
					const auto value { dataCloud->readDataValue<long>(p_id) };
					var_str_value = std::to_string(value);
				}
			},
			{
				typeid(unsigned long).hash_code(),
				[&](const std::string& p_id)
				{
					const auto value { dataCloud->readDataValue<long>(p_id) };
					var_str_value = std::to_string(value);
				}
			},
			{
				typeid(unsigned int).hash_code(),
				[&](const std::string& p_id)
				{
					const auto value { dataCloud->readDataValue<long>(p_id) };
					var_str_value = std::to_string(value);
				}
			},
			{
				typeid(size_t).hash_code(),
				[&](const std::string& p_id)
				{
					const auto value { dataCloud->readDataValue<long>(p_id) };
					var_str_value = std::to_string(value);
				}
			},
			{
				typeid(float).hash_code(),
				[&](const std::string& p_id)
				{
					const auto value { dataCloud->readDataValue<long>(p_id) };
					var_str_value = std::to_string(value);
				}
			},
			{
				typeid(double).hash_code(),
				[&](const std::string& p_id)
				{
					const auto value { dataCloud->readDataValue<long>(p_id) };
					var_str_value = std::to_string(value);
				}
			},
			{
				typeid(core::maths::Real3Vector).hash_code(),
				[&](const std::string& p_id)
				{
					const auto value { dataCloud->readDataValue<core::maths::Real3Vector>(p_id) };
					var_str_value = "[" + std::to_string(value[0]) + " " + std::to_string(value[1]) + " " + std::to_string(value[2]) + " ]";
				}
			},
			{
				typeid(core::maths::Real4Vector).hash_code(),
				[&](const std::string& p_id)
				{
					const auto value { dataCloud->readDataValue<core::maths::Real4Vector>(p_id) };
					var_str_value = var_str_value = "[ " + std::to_string(value[0]) + " " + std::to_string(value[1]) + " " + std::to_string(value[2]) + " " + std::to_string(value[3]) + " ]";
				}
			},
			{
				typeid(std::string).hash_code(),
				[&](const std::string& p_id)
				{
					const auto value { dataCloud->readDataValue<std::string>(p_id) };
					var_str_value = value;
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

			var_str_value = "<unknown type>";
		}

		_asm nop
	}
}

void DataPrintSystem::print()
{
	//m_renderingQueue->setText(3, { "Hello world !", "Bahnschrift.16.spritefont", { 0, 255, 0, 255 }, { 400, 10 }, 0.0 });

	int curr_row{ 0 };
	int curr_col{ 0 };
	int index{ 0 };

	for (const auto& e : m_strings)
	{
		m_renderingQueue->setText(textsIdBase + index, { m_strings.at(index), "CourierNew.10.spritefont", {255, 100, 100, 255}, {curr_col * colWidth, curr_row * rowHeight}, 0.0});
		index++;

		curr_col++;
		if (nbCols == curr_col)
		{
			curr_col = 0;
			curr_row++;

			if (nbRows == curr_row)
			{
				break;
			}
		}
	}
}