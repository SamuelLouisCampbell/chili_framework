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


Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ) 

{
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
}

void Game::ComposeFrame()
{
    gfx.DrawSurface(500, 300, sfc);

   // gfx.DrawSurface(200, 300, fsfc);

     gfx.DrawLine({ 1.0f,1.0f }, (Vec2)wnd.mouse.GetPos(), Colors::Red);
     gfx.DrawLine({ 799.0f,1.0f }, (Vec2)wnd.mouse.GetPos(), Colors::Cyan);
     gfx.DrawLine({ 1.0f,599.0f }, (Vec2)wnd.mouse.GetPos(), Colors::Yellow);
     gfx.DrawLine({ 799.0f,599.0f }, (Vec2)wnd.mouse.GetPos());
   
     gfx.DrawClosedPolyLine({ {100.f,200.f},{200.f,300.f},{300.f,300.f},{300.f,200.f} }, Colors::Blue);
     
}
