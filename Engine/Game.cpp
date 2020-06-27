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

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
    font("Fonts/arial.ttf", 42)
{
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
    std::string str = server.GetMsg();
    fontPos = { Graphics::ScreenWidth / 2,Graphics::ScreenHeight / 2 };  
    font.ComputeString(str);
    fontPos.x = fontPos.x - (font.GetStringWidth() / 2);
 
}

void Game::ComposeFrame()
{
    //Draw solid BG color
    //gfx.DrawRect(0, 0, Graphics::ScreenWidth, Graphics::ScreenHeight, Colors::MakeRGB(0, 0, 0));
    font.RenderString(gfx, fontPos);
    //test border for text string.
    /*gfx.DrawBorder(
        (font.GetStringBox().left + fontPos.x),
        (font.GetStringBox().top + fontPos.y),
        ((font.GetStringBox().right + fontPos.x) - (font.GetStringBox().left + fontPos.x)),
        ((font.GetStringBox().bottom + fontPos.y) - (font.GetStringBox().top + fontPos.y)),
        1, Colors::Green);*/

}
