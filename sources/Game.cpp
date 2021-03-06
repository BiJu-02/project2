#include "Game.h"


Game::Game(const char* title, int x, int y, int w, int h, int sc, int fps) : wrld(World())
{
	if (!SDL_Init(SDL_INIT_EVERYTHING))
	{
		std::cout << "SDL initialized" << std::endl;

		win = SDL_CreateWindow(title, x, y, w, h, sc);
		if (win)
		{ std::cout << "Window created" << std::endl; }
		else { exit(1); }

		ren = SDL_CreateRenderer(win, -1, sc);
		if (ren)
		{ 
			std::cout << "Render created" << std::endl; 
			SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
		}
		else { exit(1); }

		if (!TTF_Init())
		{
			font = TTF_OpenFont("assets/fonts/ARCADECLASSIC.TTF", 30);
			std::cout << "font loaded" << std::endl;
		}
		else { exit(1); }

		frame_delay = 1000 / fps;
		is_running = true;
		// for actual game screen = 0;
		screen = 0; 
		set_misc_render();
		wrld.map_tex = load_texture("assets/images/mapwithtexture.png");
		icons = load_texture("assets/images/icons.png");
		credits = load_texture("assets/images/credits.png");

		// load all misc_tex

		misc_tex[0] = load_texture("assets/images/homescreen.png");
		misc_tex[1] = load_texture("assets/images/hud.png");
		misc_tex[2] = load_texture("assets/images/pause.png");

		sprite_tex[0] = load_texture("assets/images/minion.png");
		sprite_tex[1] = load_texture("assets/images/mid_enemy.png");
		sprite_tex[2] = load_texture("assets/images/boss.png");
		sprite_tex[3] = load_texture("assets/images/melee.png");
		sprite_tex[4] = load_texture("assets/images/range.png");
		sprite_tex[5] = load_texture("assets/images/energyball.png");
	}
}

void Game::loop()
{
	// application window loop
	while (is_running)
	{
		frame_time = SDL_GetTicks64();

		handle_event();
		update();
		render();

		frame_time = SDL_GetTicks64() - frame_time;

		if (frame_time < frame_delay)
		{ SDL_Delay(frame_delay - frame_time); }
	}

	clean();
}

void Game::handle_event()
{
	SDL_PollEvent(&event);
	switch (event.type)
	{
	case SDL_MOUSEBUTTONDOWN:
		if (screen == 1)
		{ handle_game_screen(event.button.x, event.button.y);	}
		else if (screen == 2)
		{ handle_credits_screen(event.button.x, event.button.y); }
		else
		{ handle_start_screen(event.button.x, event.button.y); }
		break;

	case SDL_QUIT:
		is_running = false;
		break;

	default:
		break;
	}
}

void Game::handle_start_screen(int x, int y)
{
	// for start butt
	if (590 < x && x < 730 && 440 < y && y < 512)
	{
		screen = 1; 
		wrld.start_game(); 
		set_misc_render();
	}
	// for credits butt
	if (590 < x && x < 730 && 560 < y && y < 632)
	{ screen = 2; }
}

void Game::handle_game_screen(int x, int y)
{
	// if quit button is pressed ...is_paused = true ...and dialog box for confirmation
	// if quit confirmed ...wrld.exit_game() ...screen = 0
	if (1170 < x && x < 1230 && 10 < y && y < 70)		// pause / play
	{
		if (!wrld.is_playing)
		{ 
			wrld.is_playing = true; 
			std::cout << "playing" << std::endl; 
		}
		else if (wrld.is_paused)
		{ 
			wrld.is_paused = false; 
			misc_render[2] = false;
			std::cout << "resumed" << std::endl; 
		}
		else
		{ 
			wrld.is_paused = true; 
			misc_render[2] = true;
			std::cout << "paused" << std::endl; 
		}
	}
	//hero slot
	else if (!wrld.is_paused)
	{
		if (640 < y)
		{
			if (760 < x && x < 1240)			// hero slots
			{
				int tmpx = 840;
				for (int i = 0; i < 6; i++)
				{
					if (x < tmpx)
					{
						wrld.slot = i;
						std::cout << wrld.slot << std::endl;
						set_misc_render();
						break;
					}
					else
					{ tmpx += 80; }
				}
			}
			else if (520 < x && x < 600)
			{
				if (wrld.slot != -1)
				{
					if (wrld.hero_arr[wrld.slot])		// check for coins too
					{
						// upgrade
						if (wrld.coins > wrld.lvl_up_cost * wrld.hero_arr[wrld.slot]->get_lvl())
						{
							wrld.coins -= wrld.lvl_up_cost * wrld.hero_arr[wrld.slot]->get_lvl();
							wrld.hero_arr[wrld.slot]->lvl_up();
						}
					}
					else
					{
						// hero selection and spawning;
						if (wrld.coins >= wrld.hero_cost)
						{
							if (x < 560)
							{ wrld.spawn_melee(); }
							else
							{ wrld.spawn_range(); }
							wrld.coins -= wrld.hero_cost;
							wrld.slot = -1;
						}
					}
				}
			}
			else
			{ wrld.slot = -1; }
		}
		else if (120 < y)		//map
		{
			if (wrld.slot != -1)
			{
				if (wrld.hero_arr[wrld.slot])
				{
					wrld.hero_arr[wrld.slot]->set_camp(x, y);
					wrld.slot = -1;
				}
				else
				{ wrld.slot = -1; }
			}
		}
		else
		{ wrld.slot = -1; }
	}

}

void Game::handle_credits_screen(int x, int y)
{
	if (1160 < x && x < 1240 && 600 < y && y < 680)
	{
		screen = 0;
	}
}


//void Game::handle_score_screen()
//{
//
//}


void Game::update()
{
	if (screen == 1)
	{
		if (!wrld.is_paused)
		{ wrld.update(); }
		if (wrld.lives < 1)
		{
			screen = 3;
			wrld.exit_game();
		}
	}
	if (screen == 3)
	{
		
	}
}

void Game::render()
{
	SDL_RenderClear(ren);
	if (screen == 1)
	{
		render_game_screen();
	}
	else if (screen == 2)
	{
		render_credits_screen();
	}
	else
	{ SDL_RenderCopy(ren, misc_tex[0], NULL, NULL); }
	
	SDL_RenderPresent(ren);
}

inline void Game::render_game_screen()
{
	SDL_RenderCopy(ren, wrld.map_tex, NULL, NULL);
	for (auto sprite : wrld.sprite_list)
	{
		src_rec.x = sprite->src_x;
		src_rec.y = sprite->src_y;
		src_rec.w = sprite->src_w;
		src_rec.h = sprite->src_h;
		des_rec.x = sprite->xscrn;
		des_rec.y = sprite->yscrn;
		des_rec.w = sprite->wscrn;
		des_rec.h = sprite->hscrn;
		SDL_RenderCopy(ren, sprite_tex[sprite->tex_id], &src_rec, &des_rec);
	}
	for (int i = 0; i < 10; i++)
	{
		if (misc_render[i])
		{
			SDL_RenderCopy(ren, misc_tex[i], NULL, NULL);
		}
	}
	// icons...how to do dis?
	// play/pause buttn
	if (!wrld.is_playing || wrld.is_paused)
	{ src_rec.x = 120; src_rec.y = 0; }
	else
	{ src_rec.x = 80; src_rec.y = 0; }
	src_rec.w = 40; src_rec.h = 40;
	des_rec.x = 1170; des_rec.y = 10;
	des_rec.w = 60; des_rec.h = 60;
	SDL_RenderCopy(ren, icons, &src_rec, &des_rec);

	// icons around slots
	if (wrld.slot != -1)
	{	// check if affordable
		if (wrld.hero_arr[wrld.slot])
		{
			// render update button
			if (wrld.coins < wrld.lvl_up_cost * wrld.hero_arr[wrld.slot]->get_lvl())
			{ src_rec.x = 40; }
			else
			{ src_rec.x = 0; }
			src_rec.y = 80;
			src_rec.w = 40; src_rec.h = 40;
			des_rec.x = 520; des_rec.y = 650;
			des_rec.w = 60; des_rec.h = 60;
			SDL_RenderCopy(ren, icons, &src_rec, &des_rec);
			// display stats...icons and text

		}
		else
		{
			// render hero options to spawn...
			if (wrld.coins < wrld.hero_cost)
			{ src_rec.x = 80; }
			else
			{ src_rec.x = 0; }
			src_rec.y = 40;
			src_rec.w = 80; src_rec.h = 40;
			des_rec.x = 520; des_rec.y = 660;
			des_rec.w = 80; des_rec.h = 40;
			SDL_RenderCopy(ren, icons, &src_rec, &des_rec);
		}
	}
	// all the filled hero slots
	int tmpx = 770;
	for (int i = 0; i < 6; i++)
	{
		if (wrld.hero_arr[i])
		{
			if (wrld.hero_arr[i]->name == "melee")
			{ src_rec.x = 0; src_rec.y = 40; }
			else
			{ src_rec.x = 40; src_rec.y = 40; }
			src_rec.w = 40; src_rec.h = 40;
			des_rec.x = tmpx; des_rec.y = 650;
			des_rec.w = 60; des_rec.h = 60;
			SDL_RenderCopy(ren, icons, &src_rec, &des_rec);
		}
		tmpx += 80;
	}
	// ze texts on hud at top
	//lives
	if (wrld.lives < 10)
	{ des_rec.x = 125; des_rec.w = 30; }
	else
	{ des_rec.x = 85; des_rec.w = 70; }
	des_rec.y = 25; des_rec.h = 40;
	load_text(std::to_string(wrld.lives));
	SDL_RenderCopy(ren, text_tex, NULL, &des_rec);
	SDL_DestroyTexture(text_tex);

	// coins
	if (wrld.coins < 10)
	{ des_rec.x = 320; des_rec.w = 30; }
	else if (wrld.coins < 100)
	{ des_rec.x = 280; des_rec.w = 70; }
	else
	{ des_rec.x = 240; des_rec.w = 110; }
	des_rec.y = 25;
	des_rec.h = 40;
	load_text(std::to_string(wrld.coins));
	SDL_RenderCopy(ren, text_tex, NULL, &des_rec);
	SDL_DestroyTexture(text_tex);

	// score
	if (wrld.score < 10)
	{ des_rec.x = 640; des_rec.w = 30; }
	else if (wrld.score < 100)
	{ des_rec.x = 600; des_rec.w = 70; }
	else
	{ des_rec.x = 560; des_rec.w = 110; }
	des_rec.y = 25;
	des_rec.h = 40;
	load_text(std::to_string(wrld.score));
	SDL_RenderCopy(ren, text_tex, NULL, &des_rec);
	SDL_DestroyTexture(text_tex);

	//waves
	if (wrld.wave_no < 10)
	{ des_rec.x = 960; des_rec.w = 30; }
	else if (wrld.wave_no < 100)
	{ des_rec.x = 920; des_rec.w = 70; }
	else
	{ des_rec.x = 880; des_rec.w = 110; }
	des_rec.y = 25;
	des_rec.h = 40;
	load_text(std::to_string(wrld.wave_no));
	SDL_RenderCopy(ren, text_tex, NULL, &des_rec);
	SDL_DestroyTexture(text_tex);

	// hero stats

}


//start(1160, 600)
//end(1240, 680
void Game::render_credits_screen()
{
	SDL_RenderCopy(ren, credits, NULL, NULL);
	src_rec.x = 0; src_rec.y = 120;
	src_rec.w = 40; src_rec.h = 40;
	des_rec.x = 1160; des_rec.y = 600;
	des_rec.w = 80; des_rec.h = 80;
	SDL_RenderCopy(ren, icons, &src_rec, &des_rec);
}

void Game::clean()
{
	SDL_DestroyWindow(win);
	SDL_DestroyRenderer(ren);
	SDL_Quit();
	std::cout << "game cleaned" << std::endl;
}


// set values of misc_render array based on screen and slot value
void Game::set_misc_render()
{
	if (screen == 1)
	{
		misc_render[0] = false;
		misc_render[1] = true;

	}
	else
	{
		misc_render[0] = true;
		for (int i=1; i<9; i++)
		{ misc_render[i] = false; }
	}
}


SDL_Texture* Game::load_texture(const char* img_path)
{
	SDL_Surface* temp_surf = IMG_Load(img_path);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, temp_surf);
	SDL_FreeSurface(temp_surf);
	return tex;
}

void Game::load_text(std::string s)
{
	SDL_Surface* surface_message = TTF_RenderText_Blended(font, s.c_str(), text_colr);
	text_tex = SDL_CreateTextureFromSurface(ren, surface_message);
	SDL_FreeSurface(surface_message);
}
