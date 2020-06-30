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
	gfx( wnd )

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
   
	gfx.EndFrame();
  
}

void Game::UpdateModel()
{
   
    strings.clear();
    fontPositions.clear();
    fonts.clear();

    server.Frame();
  
    //StringHandling sh = { server.GetMsg() };
    StringHandling sh = { "Hello!!\rCock Rodgers Is Here!" };
    strings = sh.GetStringies();
    int totalsize = 0;
    if (strings.size() >= 1)
    {
        totalsize = -int((strings.size() * fontSize)) / 2;
    }
    for (int i = 0; i < strings.size(); i++)
    {
        std::unique_ptr<Font> font = std::make_unique<Font>(fontName.c_str(), fontSize);
        fonts.push_back(std::move(font));
        
        Vec2 posi = { float(Graphics::ScreenWidth / 2), float((Graphics::ScreenHeight / 2))};
        fonts[i]->ComputeString(strings[i]);

        posi.y = (Graphics::ScreenHeight /2) + totalsize;
        posi.x = posi.x - (fonts[i]->GetStringWidth() / 2);
        fontPositions.push_back(posi);
        totalsize += fontSize;
    }
   
    //Frame timer code.
    std::stringstream fps;
    float dt = ft.Mark();
    if (counter < 15)
    {
        counter++;
        dtCount += dt;
    }
    else
    {
        frames = dtCount / 15.0f;
        counter = 0;
        dtCount = 0.0f;
    }
    fps << "fps: " << (frames * 60) * 60;
    fpsCounterFont.ComputeString(fps.str());
    
    
}

void Game::ComposeFrame()
{
    gfx.DrawRect(0, 0, 1280, 720, Colors::Black);
    for (int i = 0; i < fonts.size(); i++)
    {
      
        fonts[i]->RenderString(gfx, fontPositions[i]);
    //    //test border for text string.
        /*  gfx.DrawBorder(
      (fonts[i]->GetStringBox().left + fontPositions[i].x),
      (fonts[i]->GetStringBox().top + fontPositions[i].y),
      ((fonts[i]->GetStringBox().right + fontPositions[i].x) - (fonts[i]->GetStringBox().left + fontPositions[i].x)),
      ((fonts[i]->GetStringBox().bottom + fontPositions[i].y) - (fonts[i]->GetStringBox().top + fontPositions[i].y)),
      1, Colors::Red);*/
    }
    ndi.SendNDIFrame(gfx);
    fpsCounterFont.RenderString(gfx,fpsPos);
 
}
