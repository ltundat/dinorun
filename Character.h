#ifndef CHARACTER_H
#define CHARACTER_H

#include "LTexture.h"

#define JUMP 1
#define FALL 2

class Character
{
public:
	static const int JUMP_SPEED = 7;
	static const int FALL_SPEED = 7;

	Character();

	bool OnGround();

	void HandleEvent(SDL_Event& e, Mix_Chunk *gJump);

	void Move();

	void Render(SDL_Rect* currentClip, SDL_Renderer *gRenderer, LTexture gCharacterTexture);

	int GetPosX();

	int GetPosY();

private:
	int posX, posY;

	int status;
};

#endif // !CHARACTER_H
