#include "Game_Base.h"
#include "Function.h"
#include "LTexture.h"
#include "Button.h"
#include "Character.h"
#include "Obstacle.h"

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Color textColor = { 255, 255, 255 };
TTF_Font* gFont = NULL;
Mix_Music* gMusic = NULL;
Mix_Music* gMenuMusic = NULL;
Mix_Chunk* gClick = NULL;
Mix_Chunk* gJump = NULL;
Mix_Chunk* gLose = NULL;

SDL_Rect gPlayButton[BUTTON_TOTAL];
SDL_Rect gDinoClips[RUNNING_FRAMES];
SDL_Rect gEnemyClips[FLYING_FRAMES];

LTexture gMenuTexture;
LTexture gCharacterTexture;
LTexture gBGTexture;
LTexture gPlayButtonTexture;
LTexture gLoseTexture;
LTexture gText1Texture;
LTexture gScoreTexture;
LTexture gText2Texture;
LTexture gHighScoreTexture;

Button PlayButton(PLAY_BUTON_POSX, PLAY_BUTTON_POSY);

Character character;

int main(int argc, char* argv[])
{
	if (!Init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		if (!LoadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			bool Quit_Menu = false;
			bool Play_Again = false;

			Mix_PlayMusic(gMenuMusic, IS_REPEATITIVE);
			while (!Quit_Menu)
			{
				SDL_Event e_mouse;
				while (SDL_PollEvent(&e_mouse) != 0)
				{
					if (e_mouse.type == SDL_QUIT)
					{
						Quit_Menu = true;
					}

					bool Quit_Game = false;
					HandlePlayButton(&e_mouse, PlayButton, Quit_Menu, Play_Again, gClick);

					if (Quit_Game == true)
					{
						return 0;
					}
				}

				gMenuTexture.render(0, 0, gRenderer);

				SDL_Rect* currentClip_Play = &gPlayButton[PlayButton.currentSprite];
				PlayButton.Render(currentClip_Play, gRenderer, gPlayButtonTexture);


				SDL_RenderPresent(gRenderer);
			}

			while (Play_Again)
			{
				srand(time(NULL));
				int time = 0;
				int score = 0;
				int acceleration = 0;
				int frame_Character = 0;
				int frame_Enemy = 0;
				std::string highscore = GetHighScoreFromFile("high_score.txt");

				SDL_Event e;
				Enemy enemy1(ON_GROUND_ENEMY);
				Enemy enemy2(ON_GROUND_ENEMY);
				Enemy enemy3(IN_AIR_ENEMY);

				Mix_PlayMusic(gMusic, IS_REPEATITIVE);
				GenerateEnemy(enemy1, enemy2, enemy3, gEnemyClips, gRenderer);

				int OffsetSpeed_Ground = BASE_OFFSET_SPEED;
				std::vector <double> OffsetSpeed_Bkgr(BACKGROUND_LAYER, BASE_OFFSET_SPEED);

				bool Quit = false;
				bool Game_State = true;
				while (!Quit)
				{
					if (Game_State)
					{
						UpdateGameTimeAndScore(time, acceleration, score);

						while (SDL_PollEvent(&e) != 0)
						{
							if (e.type == SDL_QUIT)
							{
								Quit = true;
								Play_Again = false;
							}

							character.HandleEvent(e, gJump);
						}
						SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
						SDL_RenderClear(gRenderer);

						RenderScrollingGround(OffsetSpeed_Ground, acceleration, gBGTexture, gRenderer);


						character.Move();
						SDL_Rect* currentClip_Character = NULL;
						if (character.OnGround())
						{
							currentClip_Character = &gDinoClips[frame_Character / SLOW_FRAME_CHAR];
							character.Render(currentClip_Character, gRenderer, gCharacterTexture);
						}
						else
						{
							currentClip_Character = &gDinoClips[0];
							character.Render(currentClip_Character, gRenderer, gCharacterTexture);
						}


						enemy1.Move(acceleration);
						enemy1.Render(gRenderer);

						enemy2.Move(acceleration);
						enemy2.Render(gRenderer);

						SDL_Rect* currentClip_Enemy = &gEnemyClips[frame_Enemy / SLOW_FRAME_ENEMY];
						enemy3.Move(acceleration);
						enemy3.Render(gRenderer, currentClip_Enemy);

						DrawPlayerScore(gText1Texture, gScoreTexture, textColor, gRenderer, gFont, score);
						DrawPlayerHighScore(gText2Texture, gHighScoreTexture, textColor, gRenderer, gFont, highscore);

						if (CheckEnemyColission(character,
							enemy1, enemy2, enemy3,
							currentClip_Character, currentClip_Enemy))
						{
							Mix_PauseMusic();
							Mix_PlayChannel(MIX_CHANNEL, gLose, NOT_REPEATITIVE);
							UpdateHighScore("high_score.txt", score, highscore);
							Quit = true;
						}


						SDL_RenderPresent(gRenderer);

						ControlCharFrame(frame_Character);
						ControlEnemyFrame(frame_Enemy);
					}
				}

				DrawEndGameSelection(gLoseTexture, &e, gRenderer, Play_Again);
				if (!Play_Again)
				{
					enemy1.~Enemy();
					enemy2.~Enemy();
					enemy3.~Enemy();
				}
			}
		}
	}
	Close();

	return 0;
}


bool Init()
{
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_VIDEO) < 0)
	{
		LogError("Can not initialize SDL.", SDL_ERROR);
		success = false;
	}
	else
	{
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			std::cout << "Warning: Linear texture filtering not enabled!";
		}

		gWindow = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
								   SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			LogError("Can not create window", SDL_ERROR);
			success = false;
		}
		else
		{
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				LogError("Can not create renderer", SDL_ERROR);
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					LogError("Can not initialize SDL_image", IMG_ERROR);
					success = false;
				}

				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
				{
					printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
					success = false;
				}

				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool LoadMedia()
{
	bool success = true;

	gMusic = Mix_LoadMUS("sound/bg_audio.wav");
	if (gMusic == NULL)
	{
		LogError("Failed to load background music", MIX_ERROR);
		success = false;
	}

	gMenuMusic = Mix_LoadMUS("sound/menu_audio.wav");
	if (gMenuMusic == NULL)
	{
		LogError("Failed to load menu music", MIX_ERROR);
		success = false;
	}

	gClick = Mix_LoadWAV("sound/mouse_click.wav");
	if (gClick == NULL)
	{
		LogError("Failed to load mouse click sound", MIX_ERROR);
		success = false;
	}

	gJump = Mix_LoadWAV("sound/jump_sound.wav");
	if (gJump == NULL)
	{
		LogError("Failed to load jumping sound", MIX_ERROR);
		success = false;
	}

	gLose = Mix_LoadWAV("sound/lose_sound.wav");
	if (gLose == NULL)
	{
		LogError("Failed to load lose sound", MIX_ERROR);
		success = false;
	}

	else
	{
		gFont = TTF_OpenFont("font/pixel_font.ttf", 28);
		if (gFont == NULL)
		{
			LogError("Failed to load font", MIX_ERROR);
			success = false;
		}
		else
		{
			if (!gText1Texture.loadFromRenderedText("Your score ", gFont, textColor, gRenderer))
			{
				std::cout << "Failed to render text1 texture" << std::endl;
				success = false;
			}

			if (!gText2Texture.loadFromRenderedText("High score ", gFont, textColor, gRenderer))
			{
				std::cout << "Failed to render text2 texture" << std::endl;
				success = false;
			}

			if (!gMenuTexture.loadFromFile("imgs/mainmenu/City1.png", gRenderer))
			{
				std::cout << "Failed to load menu image" << std::endl;
				success = false;
			}

			if (!gPlayButtonTexture.loadFromFile("imgs/button/play_button.png", gRenderer))
			{
				std::cout << "Failed to load play_button image" << std::endl;
				success = false;
			}
			else
			{
				for (int i = 0; i < BUTTON_TOTAL; ++i)
				{
					gPlayButton[i].x = 150 * i;
					gPlayButton[i].y = 0;
					gPlayButton[i].w = 150;
					gPlayButton[i].h = 98;
				}
			}

			if (!gBGTexture.loadFromFile("imgs/background/City1.png", gRenderer))
			{
				std::cout << "Failed to load ground image" << std::endl;
				success = false;
			}

			if (!gCharacterTexture.loadFromFile("imgs/character/char.png", gRenderer))
			{
				std::cout << "Failed to load character_run image." << std::endl;
				success = false;
			}
			else
			{
				gDinoClips[0].x = 50 * 3;
				gDinoClips[0].y = 0;
				gDinoClips[0].w = 50;
				gDinoClips[0].h = 50;

				gDinoClips[1].x = 50 * 4;
				gDinoClips[1].y = 0;
				gDinoClips[1].w = 50;
				gDinoClips[1].h = 50;

				gDinoClips[2].x = 50 * 5;
				gDinoClips[2].y = 0;
				gDinoClips[2].w = 50;
				gDinoClips[2].h = 50;

				gDinoClips[3].x = 50 * 6;
				gDinoClips[3].y = 0;
				gDinoClips[3].w = 50;
				gDinoClips[3].h = 50;

				gDinoClips[4].x = 50 * 7;
				gDinoClips[4].y = 0;
				gDinoClips[4].w = 50;
				gDinoClips[4].h = 50;

				gDinoClips[5].x = 50 * 8;
				gDinoClips[5].y = 0;
				gDinoClips[5].w = 50;
				gDinoClips[5].h = 50;
			}

			if (!gLoseTexture.loadFromFile("imgs/background/lose.png", gRenderer))
			{
				std::cout << "Failed to load lose image." << std::endl;
				success = false;
			}
		}
	}
	return success;
}

void Close()
{
	gMenuTexture.free();
	gCharacterTexture.free();
	gBGTexture.free();
	gPlayButtonTexture.free();
	gLoseTexture.free();
	gText1Texture.free();
	gScoreTexture.free();
	gText2Texture.free();
	gHighScoreTexture.free();

	Mix_FreeMusic(gMusic);
	Mix_FreeMusic(gMenuMusic);
	Mix_FreeChunk(gClick);
	Mix_FreeChunk(gLose);
	Mix_FreeChunk(gJump);
	gMusic = NULL;
	gMenuMusic = NULL;
	gClick = NULL;
	gLose = NULL;
	gJump = NULL;

	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;

	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	IMG_Quit();
	Mix_Quit();
	SDL_Quit();
}
