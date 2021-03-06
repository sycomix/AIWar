/*
 * Copyright (C) 2012, 2013 Paul Grégoire
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

#ifndef RENDERER_INTERFACE_HPP
#define RENDERER_INTERFACE_HPP

#include "game_manager.hpp"
#include "item_manager.hpp"
#include "stat_manager.hpp"

namespace aiwar {
    namespace renderer {

        /**
         * \brief Interface for all renderer objects
         */
        class RendererInterface
        {
        public:
            virtual ~RendererInterface() {}

            virtual std::string getName() const = 0;
            virtual std::string getVersion() const = 0;

            /**
             * \brief Called by the core manager to initialize the renderer object
             * \param params A string containg renderer parameters
             * \return True if initialization succeeded
             */
            virtual bool initialize(const std::string& params) = 0;

            /**
             * \brief Finalize the renderer object
             * \return True if the renderer ended successfully
             *
             * This method must not block. If implementation has to do something special
             * at the end of a battle, it can be done in the render function when the
             * gameover flag is set.
             */
            virtual bool finalize() = 0;

            /**
             * \brief Render the battle state
             * \param itemManager an object giving information on items
             * \param statManager a statistic object giving global information about the battle
             * \param gameover True if game is over
             * \param winner the winner, when gameover is true, see game_manager::getWinner()
             * \return True if the game continues, false to stop the game
             *
             * The itemManager and statManager must not be saved accross the
             * call of this function because they will be certainlly
             * invalidated just after this function returns.
             * If gameover is true, this is the last call to the renderer update. When
             * the function return, the core manager will clean and exit the game.
             */
            virtual bool render(const aiwar::core::ItemManager &itemManager,
                                const aiwar::core::StatManager &statManager,
                                bool gameover, const aiwar::core::Team& winner) = 0;
        };

    } // aiwar::renderer
} // aiwar


/**
 * \brief Get the renderer implemented by the shared object
 * \return the renderer object
 */
extern "C" aiwar::renderer::RendererInterface* getRenderer();

#endif /* RENDERER_INTERFACE_HPP */
