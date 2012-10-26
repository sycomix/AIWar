/*
 * Copyright (C) 2012 Paul Grégoire
 *
 * This file is part of AIWar.
 *
 * AIWar is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AIWar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with AIWar.  If not, see <http://www.gnu.org/licenses/>. 
 */

#ifndef RENDERER_DUMMY_HPP
#define RENDERER_DUMMY_HPP

#include "renderer_interface.hpp"

namespace aiwar {
    namespace renderer {

	class RendererDummy : public RendererInterface
	{
	public:
	    RendererDummy();
	    ~RendererDummy();
	    
	    std::string getName() const;
	    std::string getVersion() const;

	    bool initialize(const std::string& params);
	    
	    bool finalize();

	    bool render(aiwar::core::ItemManager::ItemMap::const_iterator begin,
			aiwar::core::ItemManager::ItemMap::const_iterator end,
			const aiwar::core::GameManager::Stat &stats,
			bool gameover);
	};
	
    } // aiwar::renderer
} // aiwar

#endif