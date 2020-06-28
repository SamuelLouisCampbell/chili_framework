/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"
#include "Vec2.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <thread>
#include <chrono>
#include "StringHandling.h"

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
    font("Fonts/arial.ttf", 42)
 
{ 
    // Init Network & Server
    if (ntwrk.Initialize())
    {
        server.Initialize(ip);
    }
}

void Game::Go()
{
	gfx.BeginFrame();
    using namespace std::chrono_literals;
	UpdateModel();
	ComposeFrame();
    std::this_thread::sleep_for(0.016s);
	gfx.EndFrame();
  
}

void Game::UpdateModel()
{
    server.Frame();
    for (auto f : fonts)
    {
        f.~Font();
    }
    fonts.clear();
    fontPositions.clear();
    StringHandling sh = { server.GetMsg() };
    
    strings = sh.GetStringies();


    for (int i = 0; i < strings.size(); i++)
    {
        fonts.push_back(font);
        fonts[i].ComputeString(strings[i]);
    }

 
}

void Game::ComposeFrame()
{
    


    //for (int i = 0; i < fonts.size(); i++)
    //{
    //    fonts[i].RenderString(gfx, fontPositions[i]);
    //    //test border for text string.
    //      gfx.DrawBorder(
    //  (fonts[i].GetStringBox().left + fontPositions[i].x),
    //  (fonts[i].GetStringBox().top + fontPositions[i].y),
    //  ((fonts[i].GetStringBox().right + fontPositions[i].x) - (fonts[i].GetStringBox().left + fontPositions[i].x)),
    //  ((fonts[i].GetStringBox().bottom + fontPositions[i].y) - (fonts[i].GetStringBox().top + fontPositions[i].y)),
    //  1, Colors::Green);
    //}
   


}
