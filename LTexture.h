#ifndef LTEXTURE_H
#define LTEXTURE_H

#include "Game_Base.h"

class LTexture
{
public:
	LTexture();

	~LTexture();

	void free();

	bool loadFromRenderedText(std::string textureText, TTF_Font* gFont, SDL_Color textColor, SDL_Renderer* gRenderer);

	bool loadFromFile(std::string path, SDL_Renderer *gRenderer);

	void render(int x, int y, SDL_Renderer* gRenderer, SDL_Rect* clip = NULL);

	int getWidth();

	int getHeight();

private:
	SDL_Texture* mTexture;

	int mWidth;
	int mHeight;
};

#endif // !LTEXTURE_H_
