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

#include <Python.h> // to define some constant before everything else
#include <cerrno>
#include <iostream>
#include <cstdlib>

#ifndef _WIN32
#       include <sys/resource.h>
#endif

#include "game_manager.hpp"

#include "handler_interface.hpp"
#include "handler_dummy.hpp"
#include "handler_example.hpp"
#include "python_handler.hpp"

#include "config.hpp"

#include "renderer_interface.hpp"
#include "renderer_dummy.hpp"
#include "renderer_sdl.hpp"
#include "renderer_summary.hpp"

using namespace aiwar::core;

int main(int argc, char* argv[])
{

#ifndef _WIN32
    /***** rlimit *****/
    rlimit rlim;
    if(getrlimit(RLIMIT_CORE, &rlim) == -1)
        std::cerr << "Cannot get CORE rlimit: " << strerror(errno) << std::endl;
    else
    {
        rlim.rlim_cur = rlim.rlim_max;
        if(setrlimit(RLIMIT_CORE, &rlim) == -1)
            std::cerr << "Cannot set new CORE rlimit: " << strerror(errno) << std::endl;
    }
    /******************/
#endif

    /*** read configuration ***/
    aiwar::core::Config &cfg = aiwar::core::Config::instance();

    if(!cfg.parseCmdLine(argc, argv))
    {
        std::cerr << "Bad options\n" << cfg.usage();
        return -1;
    }

    if(cfg.help)
    {
        std::cout << cfg.usage();
        return 0;
    }

    if(!cfg.loadConfigFile())
    {
        std::cerr << "Bad config file\n";
        return -1;
    }

//    std::cout << cfg.dump();
//    return 0;

    // initialize pseudo-random
    std::srand(cfg.seed);
    std::cout << "Pseudo-random generator seed: " << cfg.seed << std::endl;

    /*** Start handlers ***/

    HandlerDummy th;
    if(!th.initialize())
    {
        std::cerr << "Fail to initialize dummy handler\n";
        return -1;
    }

    HandlerExample eh;
    if(!eh.initialize())
    {
        std::cerr << "Fail to initialize example handler\n";
        th.finalize();
        return -1;
    }

    PythonHandler ph;
    if(!ph.initialize())
    {
        std::cerr << "Fail to initialize python handler\n";
        eh.finalize();
        th.finalize();
        return -1;
    }

    /*** Load teams ***/

    // load blue Team
    aiwar::core::Config::PlayerInfo &pblue = cfg.players[cfg.blue];
    HandlerInterface *hblue = NULL;
    if(pblue.handler == "dummy")
        hblue = &th;
    else if(pblue.handler == "example")
        hblue = &eh;
    else if(pblue.handler == "python")
        hblue = &ph;
    else
    {
        std::cerr << "Unknown handler name for blue player: " << pblue.handler << std::endl;
        th.finalize();
        eh.finalize();
        ph.finalize();
        return -1;
    }

    if(!hblue->load(cfg.blue, pblue.params))
    {
        std::cerr << "Fail to load blue handler\n";
        th.finalize();
        eh.finalize();
        ph.finalize();
        return -1;
    }

    // load red Team
    aiwar::core::Config::PlayerInfo &pred = cfg.players[cfg.red];
    HandlerInterface *hred = NULL;
    if(pred.handler == "dummy")
        hred = &th;
    else if(pred.handler == "example")
        hred = &eh;
    else if(pred.handler == "python")
        hred = &ph;
    else
    {
        std::cerr << "Unknown handler name for red team: " << pred.handler << std::endl;
        th.finalize();
        eh.finalize();
        ph.finalize();
        return -1;
    }

    if(!hred->load(cfg.red, pred.params))
    {
        std::cerr << "Fail to load red handler\n";
        th.finalize();
        eh.finalize();
        ph.finalize();
        return -1;
    }

    /*** Load the renderer ***/
    aiwar::renderer::RendererInterface *renderer = NULL;

    // load the dummy renderer
    aiwar::renderer::RendererDummy dummyRenderer;
    aiwar::renderer::RendererSDL sdlRenderer;
    aiwar::renderer::RendererSummary summaryRenderer;

    if(cfg.renderers[cfg.renderer].name == "dummy")
        renderer = &dummyRenderer;
    else if(cfg.renderers[cfg.renderer].name == "sdl")
        renderer = &sdlRenderer;
    else if(cfg.renderers[cfg.renderer].name == "summary")
        renderer = &summaryRenderer;
    else
    {
        std::cerr << "Cannot find renderer '" << cfg.renderers[cfg.renderer].name << "'\n";
        th.finalize();
        eh.finalize();
        ph.finalize();
        return -1;
    }

    if(!renderer->initialize(cfg.renderers[cfg.renderer].params))
    {
        std::cerr << "Fail to initialize renderer\n";
        th.finalize();
        eh.finalize();
        ph.finalize();
        return -1;
    }

    /*** Init the game ***/

    GameManager gm;

    gm.registerTeam(BLUE_TEAM, hblue->get_BaseHandler(cfg.blue), hblue->get_MiningShipHandler(cfg.blue), hblue->get_FighterHandler(cfg.blue));
    gm.registerTeam(RED_TEAM, hred->get_BaseHandler(cfg.red), hred->get_MiningShipHandler(cfg.red), hred->get_FighterHandler(cfg.red));

    if(!gm.init())
    {
        std::cerr << "Error while initializing GameManager\n";
        th.finalize();
        eh.finalize();
        ph.finalize();
        renderer->finalize();
        return -1;
    }

    /*** enter the main loop ***/
    bool done = false, gameover = false;
    Team winner = NO_TEAM, loser = NO_TEAM;
    unsigned int tick = 0;

    // game over ?
    if(gm.gameOver())
    {
        gameover = true;
        winner = gm.getWinner();
        std::cout << "********** GameOver *********\n";
        if(winner == NO_TEAM)
            std::cout << "Draw !\n";
        else
            std::cout << "Winner is: " << ((winner == BLUE_TEAM) ? cfg.players[cfg.blue].name : cfg.players[cfg.red].name) << std::endl;
    }

    // first render
    done = !renderer->render(gm.getItemManager(), gm.getStatManager(), gameover, winner) || gameover;

    while(!done)
    {
        // play
        try
        {
            gm.update(tick++);
        }
        catch(const aiwar::core::HandlerError &e)
        {
            std::cout << "********** GameOver *********\n";
            std::string name = (e.team() == BLUE_TEAM) ? cfg.players[cfg.blue].name : cfg.players[cfg.red].name;
            std::cout << "Team " << name << " has lost because an error occured in his play handler: " << e.what() << std::endl;
            loser = e.team();
            winner = (e.team() == BLUE_TEAM) ? RED_TEAM : BLUE_TEAM;
            gameover = true;
        }

        // game over ?
        if(!gameover && gm.gameOver())
        {
            gameover = true;
            winner = gm.getWinner();
            std::cout << "********** GameOver *********\n";
            if(winner == NO_TEAM)
                std::cout << "Draw !\n";
            else
                std::cout << "Winner is: " << ((winner == BLUE_TEAM) ? cfg.players[cfg.blue].name : cfg.players[cfg.red].name) << std::endl;
        }

        // render
        done = !renderer->render(gm.getItemManager(), gm.getStatManager(), gameover, winner) || gameover;
    }

    std::cout << "Number of rounds: " << gm.getStatManager().round() << "\n";

    renderer->finalize();

    // unload teams
    hred->unload(cfg.red);
    hblue->unload(cfg.blue);

    // finalize handlers
    th.finalize();
    eh.finalize();
    ph.finalize();

    std::cout << "Exiting gracefully...\n";
    std::cout << "(seed: " << cfg.seed << ")\n";

    int rc = 0;

    if(loser != NO_TEAM)
    {
        switch(loser)
        {
        case BLUE_TEAM:
            rc = 11;
            break;

        case RED_TEAM:
            rc = 12;
            break;

        default:
            break;
        }
    }
    else
    {
        switch(winner)
        {
        case BLUE_TEAM:
            rc = 1;
            break;

        case RED_TEAM:
            rc = 2;
            break;

        case NO_TEAM:
        default:
            rc = 0;
        }
    }

    return rc;
}
