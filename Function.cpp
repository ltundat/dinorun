#include "Game_Utils.h"

std::string GetHighScoreFromFile(std::string path)
{
	std::fstream HighScoreFile;
	std::string highscore;

	HighScoreFile.open(path, std::ios::in);
	HighScoreFile >> highscore;

	return highscore;
}

void UpdateHighScore(std::string path, const int& score, const std::string& old_high_score)
{
	int oldHighScore = 0;
	std::fstream HighScoreFile;
	std::string newHighScore;
	std::stringstream ConvertToInt(old_high_score);

	HighScoreFile.open(path, std::ios::out);

	ConvertToInt >> oldHighScore;
	if (score > oldHighScore)
	{
		oldHighScore = score;
	}
	newHighScore = std::to_string(oldHighScore);

	HighScoreFile << newHighScore;
}

void UpdateGameTimeAndScore( int& speed, int& score)
{
	if (time % 5 == 0)
	{
		score += SCORE_INCREASEMENT;
		time = 0;
	}

}

void RenderScrollingBackground(std::vector <double>& offsetSpeed,
	LTexture(&gBackgroundTexture)[BACKGROUND_LAYER],
	SDL_Renderer* gRenderer)
{

	for (int i = 0; i < BACKGROUND_LAYER; ++i)
	{
		gBackgroundTexture[i].render(offsetSpeed[i], 0, gRenderer);
		gBackgroundTexture[i].render(offsetSpeed[i] + gBackgroundTexture[i].getWidth(), 0, gRenderer);
	}
}

void RenderScrollingGround(int& speed, const int acceleration, LTexture gGroundTexture, SDL_Renderer* gRenderer)
{
	speed -= GROUND_SPEED + acceleration;
	if (speed < -gGroundTexture.getWidth())
	{
		speed = 0;
	}
	gGroundTexture.render(speed, 0, gRenderer);
	gGroundTexture.render(speed + gGroundTexture.getWidth(), 0, gRenderer);
}

void HandlePlayButton(SDL_Event* e, Button& PlayButton, bool& QuitMenu, bool& Play, Mix_Chunk* gClick)
{
	if (e->type == SDL_QUIT)
	{
		QuitMenu = true;
	}

	if (PlayButton.IsInside(e, COMMON_BUTTON))
	{
		switch (e->type)
		{
		case SDL_MOUSEMOTION:
			PlayButton.currentSprite = BUTTON_MOUSE_OVER;
			break;
		case SDL_MOUSEBUTTONDOWN:
			Play = true;
			QuitMenu = true;
			Mix_PlayChannel(MIX_CHANNEL, gClick, 0);
			PlayButton.currentSprite = BUTTON_MOUSE_OVER;
			break;
		}
	}
	else
	{
		PlayButton.currentSprite = BUTTON_MOUSE_OUT;
	}
}

void GenerateEnemy(Enemy& enemy1,
	Enemy& enemy2,
	Enemy& enemy3,
	SDL_Rect(&gEnemyClips)[FLYING_FRAMES],
	SDL_Renderer * gRenderer)
{
	enemy1.LoadFromFile("imgs/enemy/obs.png", gRenderer);
	enemy2.LoadFromFile("imgs/enemy/obs.png", gRenderer);
	enemy3.LoadFromFile("imgs/enemy/bat.png", gRenderer);
	{
		gEnemyClips[0].x = 43 * 3;
		gEnemyClips[0].y = 0;
		gEnemyClips[0].w = 43;
		gEnemyClips[0].h = 30;

		gEnemyClips[1].x = 43 * 4;
		gEnemyClips[1].y = 0;
		gEnemyClips[1].w = 43;
		gEnemyClips[1].h = 30;

		gEnemyClips[2].x = 43 * 2;
		gEnemyClips[2].y = 0;
		gEnemyClips[2].w = 43;
		gEnemyClips[2].h = 30;

		gEnemyClips[3].x = 43;
		gEnemyClips[3].y = 0;
		gEnemyClips[3].w = 43;
		gEnemyClips[3].h = 30;

		gEnemyClips[4].x = 0;
		gEnemyClips[4].y = 0;
		gEnemyClips[4].w = 43;
		gEnemyClips[4].h = 30;
	}
}

bool CheckColission(Character character,
	SDL_Rect* char_clip,
	Enemy enemy,
	SDL_Rect* enemy_clip)
{
	bool collide = false;

	int left_a = character.GetPosX();
	int right_a = character.GetPosX() + char_clip->w;
	int top_a = character.GetPosY();
	int bottom_a = character.GetPosY() + char_clip->h;

	if (enemy.GetType() == ON_GROUND_ENEMY)
	{
		const int TRASH_PIXEL_1 = 25;
		const int TRASH_PIXEL_2 = 30;

		int left_b = enemy.GetPosX();
		int right_b = enemy.GetPosX() + enemy.GetWidth();
		int top_b = enemy.GetPosY();
		int bottom_b = enemy.GetPosY() + enemy.GetHeight();

		if (right_a - TRASH_PIXEL_1 >= left_b && left_a + TRASH_PIXEL_1 <= right_b)
		{
			if (bottom_a - TRASH_PIXEL_2 >= top_b)
			{
				collide = true;
			}
		}
	}
	else
	{
		const int TRASH_PIXEL_1 = 22;
		const int TRASH_PIXEL_2 = 18;

		int left_b = enemy.GetPosX() + TRASH_PIXEL_1;
		int right_b = enemy.GetPosX() + enemy_clip->w - TRASH_PIXEL_1;
		int top_b = enemy.GetPosY();
		int bottom_b = enemy.GetPosY() + enemy_clip->h - TRASH_PIXEL_2;

		if (right_a >= left_b && left_a <= right_b)
		{
			if (top_a <= bottom_b && top_a >= top_b)
			{
				collide = true;
			}

			if (bottom_a >= bottom_b && bottom_a <= top_b)
			{
				collide = true;
			}
		}
	}

	return collide;
}

bool CheckEnemyColission(Character character,
	Enemy enemy1,
	Enemy enemy2,
	Enemy enemy3,
	SDL_Rect* char_clip,
	SDL_Rect* enemy_clip
	)
{
	if (CheckColission(character, char_clip, enemy1))
	{
		return true;
	}
	if (CheckColission(character, char_clip, enemy2))
	{
		return true;
	}
	if (CheckColission(character, char_clip, enemy3, enemy_clip))
	{
		return true;
	}
	return false;
}

void ControlCharFrame(int &frame)
{
	frame += FRAME_INCREASEMENT;
	if (frame / SLOW_FRAME_CHAR >= RUNNING_FRAMES)
	{
		frame = 0;
	}
}

void ControlEnemyFrame(int &frame)
{
	frame += FRAME_INCREASEMENT;
	if (frame / SLOW_FRAME_ENEMY >= FLYING_FRAMES)
	{
		frame = 0;
	}
}

void DrawPlayerScore(LTexture gTextTexture,
	LTexture gScoreTexture,
	SDL_Color textColor,
	SDL_Renderer *gRenderer,
	TTF_Font *gFont,
	const int& score)
{
	gTextTexture.render(TEXT_1_POSX, TEXT_1_POSY, gRenderer);
	if (gScoreTexture.loadFromRenderedText(std::to_string(score), gFont, textColor, gRenderer))
	{
		gScoreTexture.render(SCORE_POSX, SCORE_POSY, gRenderer);
	}
}

void DrawPlayerHighScore(LTexture gTextTexture,
	LTexture gHighScoreTexture,
	SDL_Color textColor,
	SDL_Renderer* gRenderer,
	TTF_Font* gFont,
	const std::string& HighScore)
{
	gTextTexture.render(TEXT_2_POSX, TEXT_2_POSY, gRenderer);
	if (gHighScoreTexture.loadFromRenderedText(HighScore, gFont, textColor, gRenderer))
	{
		gHighScoreTexture.render(HIGH_SCORE_POSX, HIGH_SCORE_POSY, gRenderer);
	}
}

void DrawEndGameSelection(LTexture gLoseTexture,
	SDL_Event *e,
	SDL_Renderer *gRenderer,
	bool &Play_Again)
{
	if (Play_Again)
	{
		bool End_Game = false;
		while (!End_Game)
		{
			while (SDL_PollEvent(e) != 0)
			{
				if (e->type == SDL_QUIT)
				{
					Play_Again = false;
				}

				if (e->type == SDL_KEYDOWN)
				{
					switch (e->key.keysym.sym)
					{
					case SDLK_SPACE:
						End_Game = true;
						break;
					case SDLK_ESCAPE:
						End_Game = true;
						Play_Again = false;
						break;
					}
				}
			}

			gLoseTexture.render(0, 0, gRenderer);

			SDL_RenderPresent(gRenderer);
		}
	}
}
