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

#ifndef HANDLER_INTERFACE_HPP
#define HANDLER_INTERFACE_HPP

#include "playable.hpp" // for Playable::PlayFunction

#include <string>

namespace aiwar {
    namespace core {

	class HandlerInterface
	{
	public:
	    virtual ~HandlerInterface() {}
    
	    virtual bool initialize() = 0;
	    virtual bool finalize() = 0;

	    virtual PlayFunction& get_BaseHandler(Playable::Team team) = 0;
	    virtual PlayFunction& get_MiningShipHandler(Playable::Team team) = 0;
	    virtual PlayFunction& get_FighterHandler(Playable::Team team) = 0;
	};

    } // aiwar::core
} // aiwar

#endif /* HANDLER_INTERFACE_HPP */