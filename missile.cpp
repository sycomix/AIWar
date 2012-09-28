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

#include "missile.hpp"

#include <iostream>

#include "config.hpp"

using namespace aiwar::core;

Missile::Missile(ItemManager& im, Key k, double px, double py, Living* target)
    : Item(im, k, px, py, MISSILE_SIZE_X, MISSILE_SIZE_Y),
      Movable(im, k, MISSILE_SPEED, MISSILE_START_FUEL, MISSILE_MAX_FUEL, MISSILE_MOVE_CONSO),
      Living(im, k, MISSILE_LIFE, MISSILE_LIFE),
      _target(target->_key)
{
}

Missile::~Missile()
{
}

void Missile::update(unsigned int)
{
    Movable::preUpdate();

    // check if the target is still alive
//    std::cout << "Missile[" << this << "]: check target " << _target << std::endl;
    Living *target = dynamic_cast<Living*>(_im.get(_target));
    if(! target)
    {
	// no more target, auto destruction
	_toRemoveFlag = true;
    }
    else
    {
	// move to the target
	rotateTo(target);
	double d = distanceTo(target);
	bool reached = false;
	if(d < _speed)
	{
	    _speed = d;
	    reached = true;
	}
	move();
	
	// target reached ?
	if(reached)
	{
	    target->_takeLife(MISSILE_DAMAGE, true);	    
	    _toRemoveFlag = true;
	}
    }
}