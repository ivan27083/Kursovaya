#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "C:/Учеба/1 курс/2 семестр/Учебная практика/Kursovaya/packages/sdl2_mixer.vc.2.0.1.1/build/native/lib/x64/dynamic/SDL2_mixer.lib")
#include "SDL.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <SDL_ttf.h>
#include <windows.h>
#include <SDL_image.h>
#include <cmath>
#include <locale>
#include "SDL_mixer.h"
#define BUTTON_COUNT 5
#define SCREEN_WIDTH 1500
#define SCREEN_HEIGHT 900
using namespace std;
Mix_Chunk* Sound = NULL;
Mix_Music* fon = NULL;
bool quit = false;

SDL_Texture* get_text_texture(SDL_Renderer*& renderer, char* text, TTF_Font* font) {
	SDL_Surface* textSurface = NULL;
	SDL_Color fore_color = { 0,0,0 };
	SDL_Color back_color = { 255,255,255 };

	textSurface = TTF_RenderUTF8_Shaded(font, text, fore_color, back_color);
	SDL_SetColorKey(textSurface, SDL_TRUE, SDL_MapRGB(textSurface->format, 255, 255, 255));
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_FreeSurface(textSurface);
	return texture;
}

SDL_Texture* get_text_texture2(SDL_Renderer*& renderer, char* text, TTF_Font* font) {
	SDL_Surface* textSurface = NULL;
	SDL_Color fore_color = { 255,255,255 };
	SDL_Color back_color = { 0,0,0 };

	textSurface = TTF_RenderUTF8_Shaded(font, text, fore_color, back_color);
	SDL_SetColorKey(textSurface, SDL_TRUE, SDL_MapRGB(textSurface->format, 0, 0, 0));
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_FreeSurface(textSurface);
	return texture;
}

void DrawCircle(SDL_Renderer* renderer, int x0, int y0, int r) {
	double u = 3.14 / 180 * 0;
	for (int j = 0; j < r; j++) {
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		int x1 = int(x0 + cos(u) * j);
		int y1 = int(y0 - sin(u) * j);
		for (int i = 1; i <= 360; i++) {
			u = 3.14 / 180 * i;
			int x2 = int(x0 + cos(u) * j);
			int y2 = int(y0 - sin(u) * j);
			SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
			x1 = x2;
			y1 = y2;
		}
	}
}

void draw_text(SDL_Renderer*& renderer, SDL_Texture* texture, int len)
{
	SDL_Rect rect = { SCREEN_WIDTH/2 - (35 * len)/2, 60, 35 * len, 80 };
	SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void draw_restext(SDL_Renderer*& renderer, SDL_Texture* texture, int k)
{
	int len = 0;
	if (k == 0) len = 1;
	while (k != 0) {
		k /= 10;
		len++;
	}
	SDL_Rect rect = { SCREEN_WIDTH / 2 - (40 * len), SCREEN_HEIGHT / 2, 30 * len, 60 };
	SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void draw_balls(SDL_Renderer*& renderer, SDL_Rect balls[],
	int count, SDL_Texture* texture, SDL_Texture** textTexture, SDL_Rect points[])
{
	for (int i = 0; i < count; i++)
	{
		SDL_RenderCopy(renderer, textTexture[i], NULL, &points[i]);
		SDL_RenderCopy(renderer, texture, NULL, &balls[i]);
	}
}

bool is_ball_hit(SDL_Rect ball, int x, int y)
{
	if (ball.w == 0) return false;
	int centerX = ball.x + ball.w / 2;
	int centerY = ball.y + ball.h / 2;
	int radius = ball.w / 2;
	return sqrt((centerX - x) * (centerX - x) + (centerY - y) * (centerY - y)) < radius;
}

bool is_square_hit(SDL_Rect ball, int x, int y)
{
	if (ball.w == 0) return false;
	if (ball.x < x && ball.x + ball.w > x && ball.y < y && ball.y + ball.h > y) return true;
	else return false;
}

void loadmusic() {
	fon = Mix_LoadMUS("music.wav");
	Mix_PlayMusic(fon, -1);
}

void sound(char* name) {
	Sound = Mix_LoadWAV(name);
	Mix_PlayChannel(-1, Sound, 0);
}

void initButtons(SDL_Rect* buttons) {
	for (int i = 0; i < BUTTON_COUNT; i++)
	{
		buttons[i] = { SCREEN_WIDTH / 2 - (30 * 15) / 2, 240 + (i*100), 30 * 15, 80};
	}
}
void initLables(SDL_Rect* lables) {
	for (int i = 0; i < BUTTON_COUNT; i++)
	{
		lables[i] = { SCREEN_WIDTH / 2 - (30 * 10) / 2, 250 + (i * 100), 30 * 10, 60 };
	}
}

SDL_Texture* createTexture(SDL_Renderer* renderer, const char* name) {
	SDL_Surface* Image = IMG_Load(name);
	SDL_SetColorKey(Image, SDL_TRUE, SDL_MapRGB(Image->format, 255, 255, 255));
	SDL_Texture* Texture = SDL_CreateTextureFromSurface(renderer, Image);
	SDL_FreeSurface(Image);
	return Texture;
}

int game(SDL_Renderer* renderer, TTF_Font* my_font, int scale, int pic) {
	SDL_Rect back = { 90, 70, 100, 100 };
	SDL_Texture* backTexture = createTexture(renderer, "arrow.bmp");

	SDL_Texture* fonTexture = createTexture(renderer, "fon.bmp");
	SDL_Rect fonRect = { 0,0,SCREEN_WIDTH, SCREEN_HEIGHT };

	string name;
	SDL_Texture*** fieldTexture = (SDL_Texture***)malloc(sizeof(SDL_Texture**) * scale);
	if (scale > 0) {
		for (int i = 0; i < scale; i++)
		{
			fieldTexture[i] = (SDL_Texture**)malloc(sizeof(SDL_Texture*) * scale);
			for (int j = 0; j < scale; j++)
			{
				fieldTexture[i][j] = NULL;
			}
		}
	}
	SDL_Rect** backfield = (SDL_Rect**)malloc(sizeof(SDL_Rect*) * scale);
	int wh = 540 / scale;
	if (scale > 0) {
		for (int i = 0; i < scale; i++)
		{
			backfield[i] = (SDL_Rect*)malloc(sizeof(SDL_Rect) * scale);
			for (int j = 0; j < scale; j++)
			{
				backfield[i][j] = { 500 + wh * j, 200 + wh * i, wh, wh };
			}
		}
	}
	SDL_Rect** field = (SDL_Rect**)malloc(sizeof(SDL_Rect*) * scale);
	if (scale > 0) {
		for (int i = 0; i < scale; i++)
		{
			field[i] = (SDL_Rect*)malloc(sizeof(SDL_Rect) * scale);
			for (int j = 0; j < scale; j++)
			{
				field[i][j] = { 500 + wh * j+1, 200 + wh * i+1, wh-2, wh-2 };
			}
		}
	}
	SDL_Rect hearts[3];
	for (int i = 0; i < 3; i++)
	{
		hearts[i] = { 1200 + i * 70 , 50, 70, 70 };
	}
	SDL_Texture* heartTexture = createTexture(renderer, "heart.bmp");

	int** ideal = (int**)malloc(sizeof(int*) * scale);
	if (scale > 0) {
		for (int i = 0; i < scale; i++)
		{
			ideal[i] = (int*)malloc(sizeof(int) * scale);
		}
	}
	int** mas = (int**)malloc(sizeof(int*) * scale);
	if (scale > 0) {
		for (int i = 0; i < scale; i++)
		{
			mas[i] = (int*)malloc(sizeof(int) * scale);
			for (int j = 0; j < scale; j++) {
				mas[i][j] = 0;
			}
		}
	}

	

	switch (pic) {
	case 0: name = "tree"; break;
	case 1: name = "watermelon"; break;
	case 2: name = "Pokeball"; break;
	case 3: name = "flame"; break;
	case 4: name = "tree"; break;
	case 5: name = "tree"; break;
	default: return -1; break;
	}

	char intStr[10];
	_itoa(scale, intStr, 10);
	string str = string(intStr);
	name += str + ".txt";
	fstream f(name);
	int n;
	f >> n;
	int* colors = new int[n];
	for (int i = 0; i < n; i++)
	{
		f >> colors[i];
	}
	if (!f) return -2;
	for (int i = 0; i < scale; i++) {
		for (int j = 0; j < scale; j++) {
			f >> ideal[i][j];
		}
	}
	f.close();
	/*for (int i = 0; i < scale; i++) {
		cout << endl;
		for (int j = 0; j < scale; j++) {
			cout << ideal[i][j] << " ";
		}
	}
	cout << endl;*/
	SDL_Texture* timerTexture;
	int min, sec;
	int frameCounter = 0;
	int curFrames = SDL_GetTicks()/1000;

	SDL_Rect* circles = (SDL_Rect*)malloc(sizeof(SDL_Rect)*n);
	SDL_Texture** paint = (SDL_Texture**)malloc(sizeof(SDL_Texture*) * n);
	SDL_Texture** paint_chosen = (SDL_Texture**)malloc(sizeof(SDL_Texture*) * n);
	for (int i = 0; i < n; i++)
	{
		circles[i] = {650 + i* 80, 800, 70, 70};
		switch (colors[i]) {
		case 1: {
			paint[i] = createTexture(renderer, "GreenCircle.bmp");
			paint_chosen[i] = createTexture(renderer, "GreenCircleBold.bmp");
			break; }
		case 2:{ paint[i] = createTexture(renderer, "BrownCircle.bmp");
			paint_chosen[i] = createTexture(renderer, "BrownCircleBold.bmp");
			break; }
		case 3: {paint[i] = createTexture(renderer, "RedCircle.bmp");
			paint_chosen[i] = createTexture(renderer, "RedCircleBold.bmp");
			break; }
		case 4: {paint[i] = createTexture(renderer, "PurpleCircle.bmp");
			paint_chosen[i] = createTexture(renderer, "PurpleCircleBold.bmp");
			break; }
		case 5: {paint[i] = createTexture(renderer, "LightGreenCircle.bmp");
			paint_chosen[i] = createTexture(renderer, "LightGreenCircleBold.bmp");
			break; };
		case 6: {paint[i] = createTexture(renderer, "BlackCircle.bmp"); 
			paint_chosen[i] = createTexture(renderer, "BlackCircleBold.bmp");
			break; }
		case 7: {paint[i] = createTexture(renderer, "GreyCircle.bmp");
			paint_chosen[i] = createTexture(renderer, "GreyCircleBold.bmp");
			break; }
		case 8: {paint[i] = createTexture(renderer, "OrangeCircle.bmp");
			paint_chosen[i] = createTexture(renderer, "OrangeCircleBold.bmp");
			break; }
		}
	}
	

	int tmp = 0, counter = 0;
	int* lens = (int*)malloc(sizeof(int) * scale);
	SDL_Rect** Leftb = (SDL_Rect**)malloc(sizeof(SDL_Rect*) * scale);
	SDL_Rect** Leftf = (SDL_Rect**)malloc(sizeof(SDL_Rect*) * scale);
	SDL_Texture*** textTexture = (SDL_Texture***)malloc(sizeof(SDL_Texture**) * scale);
	SDL_Texture*** Lcolor = (SDL_Texture***)malloc(sizeof(SDL_Texture**) * scale);
	int** left = (int**)malloc(sizeof(int*) * scale);
	for (int i = scale-1; i >= 0; i--)
	{
		lens[i] = 0;
		tmp = 0;
		counter = -1;
		left[i] = (int*)malloc(sizeof(int));
		Leftb[i] = (SDL_Rect*)malloc(sizeof(SDL_Rect));
		Leftf[i] = (SDL_Rect*)malloc(sizeof(SDL_Rect));
		Lcolor[i] = (SDL_Texture**)malloc(sizeof(SDL_Texture*));
		textTexture[i] = (SDL_Texture**)malloc(sizeof(SDL_Texture*));
		for (int j = scale-1; j >= 0; j--) {
			if (tmp != ideal[i][j] && ideal[i][j] != 0) {
				counter++;
				lens[i]++;
				tmp = ideal[i][j];
				left[i] = (int*)realloc(left[i], sizeof(int) * (counter + 1));
				left [i][counter] = 0;
				Leftb[i] = (SDL_Rect*)realloc(Leftb[i], sizeof(SDL_Rect) * (counter + 1));
				Leftf[i] = (SDL_Rect*)realloc(Leftf[i], sizeof(SDL_Rect) * (counter + 1));
				Lcolor[i] = (SDL_Texture**)realloc(Lcolor[i], sizeof(SDL_Texture*) * (counter + 1));
				textTexture[i] = (SDL_Texture**)realloc(textTexture[i], sizeof(SDL_Texture*) * (counter + 1));
				Leftb[i][counter] = { 460 - 540 / 15 * counter, 200 + 125 / scale + wh * i , 540 / 15 - scale, 540 / 12 - scale };
				Leftf[i][counter] = { 460 - 540 / 15 * counter, 200 + 125 / scale + wh * i , 540 / 20 - 2 * scale / 3, 540 / 15 - scale };
				left[i][counter] += 1;
				switch (colors[tmp - 1]) {
				case 1: Lcolor[i][counter] = createTexture(renderer, "Green.bmp"); break;
				case 2: Lcolor[i][counter] = createTexture(renderer, "Brown.bmp"); break;
				case 3: Lcolor[i][counter] = createTexture(renderer, "Red.bmp"); break;
				case 4: Lcolor[i][counter] = createTexture(renderer, "Purple.bmp"); break;
				case 5: Lcolor[i][counter] = createTexture(renderer, "LightGreen.bmp"); break;
				case 6: Lcolor[i][counter] = createTexture(renderer, "Black.bmp"); break;
				case 7: Lcolor[i][counter] = createTexture(renderer, "Grey.bmp"); break;
				case 8: Lcolor[i][counter] = createTexture(renderer, "Orange.bmp"); break;
				}
			}
			else if (tmp == ideal[i][j] && ideal[i][j] != 0) {
				left[i][counter] += 1;
			}
			else tmp = ideal[i][j];
		}
	}

	int* lens2 = (int*)malloc(sizeof(int) * scale);
	SDL_Rect** Upb = (SDL_Rect**)malloc(sizeof(SDL_Rect*) * scale);
	SDL_Rect** Upf = (SDL_Rect**)malloc(sizeof(SDL_Rect*) * scale);
	SDL_Texture*** textTexture2 = (SDL_Texture***)malloc(sizeof(SDL_Texture**) * scale);
	SDL_Texture*** Ucolor = (SDL_Texture***)malloc(sizeof(SDL_Texture**) * scale);
	int** up = (int**)malloc(sizeof(int*) * scale);
	for (int i = scale-1; i >= 0; i--)
	{
		lens2[i] = 0;
		tmp = 0;
		counter = -1;
		up[i] = (int*)malloc(sizeof(int));
		Upb[i] = (SDL_Rect*)malloc(sizeof(SDL_Rect));
		Upf[i] = (SDL_Rect*)malloc(sizeof(SDL_Rect));
		Ucolor[i] = (SDL_Texture**)malloc(sizeof(SDL_Texture*));
		textTexture2[i] = (SDL_Texture**)malloc(sizeof(SDL_Texture*));
		for (int j = scale-1; j >= 0; j--) {
			if (tmp != ideal[j][i] && ideal[j][i] != 0) {
				counter++;
				lens2[i]++;
				tmp = ideal[j][i];
				up[i] = (int*)realloc(up[i], sizeof(int) * (counter + 1));
				up[i][counter] = 0;
				Upb[i] = (SDL_Rect*)realloc(Upb[i], sizeof(SDL_Rect) * (counter + 1));
				Upf[i] = (SDL_Rect*)realloc(Upf[i], sizeof(SDL_Rect) * (counter + 1));
				Ucolor[i] = (SDL_Texture**)realloc(Ucolor[i], sizeof(SDL_Texture*) * (counter + 1));
				textTexture2[i] = (SDL_Texture**)realloc(textTexture2[i], sizeof(SDL_Texture*) * (counter + 1));
				Upb[i][counter] = { 500 + 190 / scale + wh * i, 140 - 540 / 16 * counter, 540 / 15 - scale, 540 / 12 - scale };
				Upf[i][counter] = { 500 + 190 / scale + wh * i + (540 / 20 - scale)/5, 140 - 540 / 15 * counter + (540 / 20 - 5)/2, 540 /20 - 2*scale/3, 540 / 15 - scale };
				up[i][counter] += 1;
				switch (colors[tmp-1]) {
				case 1: Ucolor[i][counter] = createTexture(renderer, "Green.bmp"); break;
				case 2: Ucolor[i][counter] = createTexture(renderer, "Brown.bmp"); break;
				case 3: Ucolor[i][counter] = createTexture(renderer, "Red.bmp"); break;
				case 4: Ucolor[i][counter] = createTexture(renderer, "Purple.bmp"); break;
				case 5: Ucolor[i][counter] = createTexture(renderer, "LightGreen.bmp"); break;
				case 6: Ucolor[i][counter] = createTexture(renderer, "Black.bmp"); break;
				case 7: Ucolor[i][counter] = createTexture(renderer, "Grey.bmp"); break;
				case 8: Lcolor[i][counter] = createTexture(renderer, "Orange.bmp"); break;
				}
			}
			else if (tmp == ideal[j][i] && ideal[j][i] != 0) {
				up[i][counter] += 1;
			}
			else tmp = ideal[j][i];
		}
	}

	SDL_Texture* pict[6];
	SDL_Surface* Image = IMG_Load("tree.bmp");
	pict[0] = SDL_CreateTextureFromSurface(renderer, Image);
	Image = IMG_Load("watermelon.bmp");
	pict[1] = SDL_CreateTextureFromSurface(renderer, Image);
	Image = IMG_Load("Pokeball.bmp");
	pict[2] = SDL_CreateTextureFromSurface(renderer, Image);
	Image = IMG_Load("flame.bmp");
	pict[3] = SDL_CreateTextureFromSurface(renderer, Image);
	Image = IMG_Load("tree.bmp");
	pict[4] = SDL_CreateTextureFromSurface(renderer, Image);
	Image = IMG_Load("tree.bmp");
	pict[5] = SDL_CreateTextureFromSurface(renderer, Image);
	SDL_FreeSurface(Image);
	SDL_Rect pictRect = { 645, 250, 250, 250 };
	

	SDL_Texture* resultTexture = createTexture(renderer, "square.bmp");
	SDL_Rect resultRect = { 250, 150, 1000,600 };
	bool check = false, ch1 = true, ch2 = true;
	char text[10], nums[10];
	int color = 1, hp = 3;
	int res = 0;
	char hitsound[13] = "hitsound.wav";
	SDL_Event event;
	while (!quit)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) {
				quit = true;
				return res;
			}
			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
			{
				for (int i = 0; i < scale; i++)
				{
					for (int j = 0; j < scale; j++)
					{
						if (is_square_hit(field[i][j], event.button.x, event.button.y) && fieldTexture[i][j] == NULL) {
							if (color == ideal[i][j]) {
								switch (colors[color-1]) {
								case 1: fieldTexture[i][j] = createTexture(renderer, "Green.bmp"); break;
								case 2: fieldTexture[i][j] = createTexture(renderer, "Brown.bmp"); break;
								case 3: fieldTexture[i][j] = createTexture(renderer, "Red.bmp"); break;
								case 4: fieldTexture[i][j] = createTexture(renderer, "Purple.bmp"); break;
								case 5: fieldTexture[i][j] = createTexture(renderer, "LightGreen.bmp"); break;
								case 6: fieldTexture[i][j] = createTexture(renderer, "Black.bmp"); break;
								case 7: fieldTexture[i][j] = createTexture(renderer, "Grey.bmp"); break;
								case 8: fieldTexture[i][j] = createTexture(renderer, "Orange.bmp"); break;
								}
								mas[i][j] = color;
							}
							else {
								hp -= 1;
								hearts[hp].w = hearts[hp].h = 0;
							}
						}
					}
				}
				for (int i = 0; i < n; i++) {
					if (is_ball_hit(circles[i], event.button.x, event.button.y)) {
						color = i + 1;
					}
				}
				if (is_square_hit(back, event.button.x, event.button.y))
				{
					sound(hitsound);
					for (int i = 0; i < scale; i++)
					{
						for (int j = 0; j < lens[i]; j++) {
							SDL_DestroyTexture(textTexture[i][j]);
							SDL_DestroyTexture(Lcolor[i][j]);
						}
						for (int j = 0; j < lens2[i]; j++) {
							SDL_DestroyTexture(textTexture2[i][j]);
							SDL_DestroyTexture(Ucolor[i][j]);
						}
					}
					for (int i = 0; i < scale; i++)
					{
						for (int j = 0; j < scale; j++)
						{
							SDL_DestroyTexture(fieldTexture[i][j]);
						}
					}
					for (int i = 0; i < 6; i++)
					{
						SDL_DestroyTexture(pict[i]);
					}
					for (int i = 0; i < n; i++)
					{
						SDL_DestroyTexture(paint[i]);
						SDL_DestroyTexture(paint_chosen[i]);
					}
					SDL_DestroyTexture(fonTexture);
					SDL_DestroyTexture(heartTexture);
					SDL_DestroyTexture(backTexture);
					return res;
				}
			}
			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT) {
				for (int i = 0; i < scale; i++)
				{
					for (int j = 0; j < scale; j++)
					{
						if (is_square_hit(field[i][j], event.button.x, event.button.y) && fieldTexture[i][j] == NULL) {
							if (mas[i][j] == ideal[i][j]) {
								fieldTexture[i][j] = createTexture(renderer, "cross.bmp");
							}
							else {
								hp -= 1;
								hearts[hp].w = hearts[hp].h = 0;
							}
						}
					}
				}
			}
		}

		SDL_RenderCopy(renderer, fonTexture, NULL, &fonRect);
		SDL_RenderCopy(renderer, backTexture, NULL, &back);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		for (int i = 0; i < 3; i++)
		{
			SDL_RenderCopy(renderer, heartTexture, NULL, &hearts[i]);
		}

		for (int i = 0; i < scale; i++)
		{
			ch1 = true;
			ch2 = true;
			for (int j = 0; j < scale; j++)
			{
				if (mas[i][j] != ideal[i][j]) ch1 = false;
				if (mas[j][i] != ideal[j][i]) ch2 = false;
			}
			for (int j = 0; j < scale; j++) {
				if (ch1 && fieldTexture[i][j] == NULL) fieldTexture[i][j] = createTexture(renderer, "cross.bmp");
				if (ch2 && fieldTexture[j][i] == NULL) fieldTexture[j][i] = createTexture(renderer, "cross.bmp");
			}
		}

		for (int i = 0; i < scale; i++)
		{
			for (int j = 0; j < scale; j++) {
				SDL_RenderDrawRect(renderer, &backfield[i][j]);
				SDL_RenderCopy(renderer, fieldTexture[i][j], NULL, &field[i][j]);
			}
		}
		for (int i = 0; i < scale; i++)
		{
			for (int j = 0; j < lens[i]; j++) {
				_itoa(left[i][j], text, 10);
				textTexture[i][j] = get_text_texture2(renderer, text, my_font);
				SDL_RenderCopy(renderer, Lcolor[i][j], NULL, &Leftb[i][j]);
				SDL_RenderCopy(renderer, textTexture[i][j], NULL, &Leftf[i][j]);
			}
		}
		for (int i = 0; i < scale; i++)
		{
			for (int j = 0; j < lens2[i]; j++) {
				_itoa(up[i][j], text, 10);
				textTexture2[i][j] = get_text_texture2(renderer, text, my_font);
				SDL_RenderCopy(renderer, Ucolor[i][j], NULL, &Upb[i][j]);
				SDL_RenderCopy(renderer, textTexture2[i][j], NULL, &Upf[i][j]);
			}
		}
		for (int i = 0; i < n; i++)
		{
			if ((color-1) == i) SDL_RenderCopy(renderer, paint_chosen[i], NULL, &circles[i]);
			else SDL_RenderCopy(renderer, paint[i], NULL, &circles[i]);
		}
		check = true;
		for (int i = 0; i < scale; i++)
		{
			for (int j = 0; j < scale; j++)
			{
				if (mas[i][j] != ideal[i][j]) check = false;
			}
		}
		frameCounter += SDL_GetTicks() / 1000 - curFrames;
		if (SDL_GetTicks() / 1000 > curFrames) { curFrames++; }
		min = frameCounter / 60;
		sec = frameCounter % 60;
		SDL_Rect timer = { 1250 - frameCounter / 3600 * 70 , 150,  frameCounter / 3600 * 70 + 160, 70 };
		char txt[10];
		_itoa(min, nums, 10);
		strcat(nums,":");
		_itoa(sec, txt, 10);
		if (sec < 10) strcat(nums, "0");
		strcat(nums, txt);
		timerTexture = get_text_texture(renderer,nums,my_font);
		SDL_RenderCopy(renderer, timerTexture, NULL, &timer);
		SDL_DestroyTexture(timerTexture);
		
		if (check) {
			fstream file("records.txt", ios::app);
			fstream file1("records.txt");
			if (file1.peek() != EOF) file << "\n";
			file1.close();
			file << pic;
			file << "\n";
			file << scale / 5-1;
			file << "\n";
			file << nums;
			file << "\n";
			file << hp;
			file.close();
			string nazv;
			switch (pic) {
			case 0: nazv = u8"Дерево"; break;
			case 1: nazv = u8"Арбуз"; break;
			case 2: nazv = u8"Покеболл"; break;
			case 3: nazv = u8"Огонь"; break;
			case 4: nazv = u8"Дерево"; break;
			case 5: nazv = u8"Дерево"; break;
			}
			char* cnazv = &nazv[0];
			char menuexit[] = u8"Главное меню";
			SDL_Texture* buttonText = get_text_texture(renderer, menuexit, my_font);
			SDL_Rect menuButton = { 525, 625, 20 * strlen(menuexit), 50 };
			SDL_Texture* Nazvanie = get_text_texture(renderer,cnazv,my_font);
			SDL_Rect nazvRect = { 645, 530, 20 * strlen(nazv.data()), 50 };
			SDL_Event event_fin;
			SDL_PollEvent(&event_fin);
			while (!(event_fin.type == SDL_MOUSEBUTTONDOWN && event_fin.button.button == SDL_BUTTON_LEFT && is_square_hit(menuButton, event_fin.button.x, event_fin.button.y))&& !quit) {
				while (SDL_PollEvent(&event_fin)) {
					if (event_fin.type == SDL_QUIT) {
						quit = true;
					}
					SDL_RenderCopy(renderer, resultTexture, NULL, &resultRect);
					SDL_RenderCopy(renderer, buttonText, NULL, &menuButton);
					SDL_RenderCopy(renderer, pict[pic], NULL, &pictRect);
					SDL_RenderCopy(renderer, Nazvanie, NULL, &nazvRect);
					SDL_RenderPresent(renderer);
				}
			}
			for (int i = 0; i < n; i++)
			{
				SDL_DestroyTexture(paint[i]);
				SDL_DestroyTexture(paint_chosen[i]);
			}
			for (int i = 0; i < scale; i++)
			{
				for (int j = 0; j < lens[i]; j++) {
					SDL_DestroyTexture(textTexture[i][j]);
					SDL_DestroyTexture(Lcolor[i][j]);
				}
				for (int j = 0; j < lens2[i]; j++) {
					SDL_DestroyTexture(textTexture2[i][j]);
					SDL_DestroyTexture(Ucolor[i][j]);
				}
			}
			for (int i = 0; i < scale; i++)
			{
				for (int j = 0; j < scale; j++)
				{
					SDL_DestroyTexture(fieldTexture[i][j]);
				}
			}
			for (int i = 0; i < 6; i++)
			{
				SDL_DestroyTexture(pict[i]);
			}
			SDL_DestroyTexture(fonTexture);
			SDL_DestroyTexture(heartTexture);
			SDL_DestroyTexture(backTexture);
			return res;
		}
		if (hp <= 0) {
			char lose[] = u8"Попробуйте еще раз!";
			char menuexit[] = u8"Главное меню";
			SDL_Texture* Lose = get_text_texture(renderer, lose, my_font);
			SDL_Rect loseRect = { 300, 350, 25 * strlen(lose), 70 };
			SDL_Texture* buttonText = get_text_texture(renderer, menuexit, my_font);
			SDL_Rect menuButton = { 525, 550, 20 * strlen(menuexit), 50 };
			
			while (!(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT && is_square_hit(menuButton, event.button.x, event.button.y))) {
				while (SDL_PollEvent(&event)) {
					if (event.type == SDL_QUIT) {
						quit = true;
					}
					SDL_RenderCopy(renderer, resultTexture, NULL, &resultRect);
					SDL_RenderCopy(renderer, Lose, NULL, &loseRect);
					SDL_RenderCopy(renderer, buttonText, NULL, &menuButton);
					SDL_RenderPresent(renderer);
				}
			}
			SDL_DestroyTexture(resultTexture);
			SDL_DestroyTexture(Lose);
			SDL_DestroyTexture(buttonText);
			for (int i = 0; i < n; i++)
			{
				SDL_DestroyTexture(paint[i]);
				SDL_DestroyTexture(paint_chosen[i]);
			}
			for (int i = 0; i < scale; i++)
			{
				for (int j = 0; j < lens[i]; j++) {
					SDL_DestroyTexture(textTexture[i][j]);
				}
				for (int j = 0; j < lens2[i]; j++) {
					SDL_DestroyTexture(textTexture2[i][j]);
				}
			}
			for (int i = 0; i < scale; i++)
			{
				for (int j = 0; j < scale; j++)
				{
					SDL_DestroyTexture(fieldTexture[i][j]);
				}
			}
			for (int i = 0; i < 6; i++)
			{
				SDL_DestroyTexture(pict[i]);
			}
			SDL_DestroyTexture(fonTexture);
			SDL_DestroyTexture(heartTexture);
			SDL_DestroyTexture(backTexture);
			return res;
		}
		SDL_RenderPresent(renderer);
		for (int i = 0; i < scale; i++)
		{
			for (int j = 0; j < lens[i]; j++) {
				SDL_DestroyTexture(textTexture[i][j]);
			}
			for (int j = 0; j < lens2[i]; j++) {
				SDL_DestroyTexture(textTexture2[i][j]);
			}
		}
	}
	for (int i = 0; i < n; i++)
	{
		SDL_DestroyTexture(paint[i]);
		SDL_DestroyTexture(paint_chosen[i]);
	}
	for (int i = 0; i < scale; i++)
	{
		for (int j = 0; j < lens[i]; j++) {
			SDL_DestroyTexture(textTexture[i][j]);
			SDL_DestroyTexture(Lcolor[i][j]);
		}
		for (int j = 0; j < lens2[i]; j++) {
			SDL_DestroyTexture(textTexture2[i][j]);
			SDL_DestroyTexture(Ucolor[i][j]);
		}
	}
	for (int i = 0; i < scale; i++)
	{
		for (int j = 0; j < scale; j++)
		{
			SDL_DestroyTexture(fieldTexture[i][j]);
		}
	}
	for (int i = 0; i < 6; i++)
	{
		SDL_DestroyTexture(pict[i]);
	}
	SDL_DestroyTexture(fonTexture);
	SDL_DestroyTexture(heartTexture);
	SDL_DestroyTexture(backTexture);
}

int options(SDL_Renderer* renderer, TTF_Font* my_font, int* x) {
	char title[19] = u8"Параметры";
	SDL_Texture* textTexture = get_text_texture(renderer, title, my_font);
	SDL_Event event;
	char hitsound[13] = "hitsound.wav";
	SDL_Rect sliders[2];
	sliders[0] = { SCREEN_WIDTH / 2 - (100 * 10) / 2, 340, 100 * 10, 40 };
	sliders[1] = { SCREEN_WIDTH / 2 - (100 * 10) / 2, 590, 100 * 10, 40 };
	SDL_Rect back = {90, 70, 100, 100};

	SDL_Texture* fonTexture = createTexture(renderer, "fon.bmp");
	SDL_Rect fonRect = { 0,0,SCREEN_WIDTH, SCREEN_HEIGHT };

	SDL_Rect text[2];
	SDL_Texture* texts[2];
	char ch1[32] = u8"Громкость музыки";
	char ch2[53] = u8"Громкость звуковых эффектов";
	text[0] = { SCREEN_WIDTH / 2 - (100 * 10)/2, 200, 30 * 16, 60 };
	text[1] = { SCREEN_WIDTH / 2 - (100 * 10)/2, 450, 30 * 27, 60 };
	texts[0] = get_text_texture(renderer, ch1, my_font);
	texts[1] = get_text_texture(renderer, ch2, my_font);

	SDL_Surface* backImage = IMG_Load("arrow.bmp");
	SDL_SetColorKey(backImage, SDL_TRUE, SDL_MapRGB(backImage->format, 255, 255, 255));
	SDL_Texture* backTexture = SDL_CreateTextureFromSurface(renderer, backImage);
	SDL_FreeSurface(backImage);

	SDL_Surface* sliderImage = IMG_Load("stick.bmp");
	SDL_SetColorKey(sliderImage, SDL_TRUE, SDL_MapRGB(sliderImage->format, 255, 255, 255));
	SDL_Texture* sliderTexture = SDL_CreateTextureFromSurface(renderer, sliderImage);
	SDL_FreeSurface(sliderImage);

	while (!quit)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) {
				quit = true;
				return 0;
			}
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				for (int i = 0; i < 2; i++)
				{
					if (is_square_hit(sliders[i], event.button.x, event.button.y))
					{
						x[i] = event.button.x;
					}
				}
				if (is_square_hit(back, event.button.x, event.button.y))
				{
					sound(hitsound);
					return 0;
				}
			}
		}
		Mix_VolumeMusic((x[0] - 10 - (SCREEN_WIDTH / 2 - (100 * 10) / 2)) / 8);
		Mix_Volume(-1, (x[1] - 10 - (SCREEN_WIDTH / 2 - (100 * 10) / 2)) / 8);
		SDL_RenderCopy(renderer, fonTexture, NULL, &fonRect);
		DrawCircle(renderer, x[0], 359, 20);
		DrawCircle(renderer, x[1], 609, 20);
		SDL_RenderCopy(renderer, sliderTexture, NULL, &sliders[0]);
		SDL_RenderCopy(renderer, sliderTexture, NULL, &sliders[1]);
		SDL_RenderCopy(renderer, backTexture, NULL, &back);

		SDL_RenderCopy(renderer, texts[0], NULL, &text[0]);
		SDL_RenderCopy(renderer, texts[1], NULL, &text[1]);
		draw_text(renderer, textTexture, 9);
		SDL_RenderPresent(renderer);
	}
}

int picture(SDL_Renderer* renderer, TTF_Font* my_font) {
	char title[28] = u8"Выбор картинки";
	SDL_Texture* textTexture = get_text_texture(renderer, title, my_font);

	SDL_Texture* fonTexture = createTexture(renderer, "fon.bmp");
	SDL_Rect fonRect = { 0,0,SCREEN_WIDTH, SCREEN_HEIGHT };

	SDL_Rect back = { 90, 70, 100, 100 };
	SDL_Surface* backImage = IMG_Load("arrow.bmp");
	SDL_SetColorKey(backImage, SDL_TRUE, SDL_MapRGB(backImage->format, 255, 255, 255));
	SDL_Texture* backTexture = SDL_CreateTextureFromSurface(renderer, backImage);
	SDL_FreeSurface(backImage);
	SDL_Texture* pic[6];
	pic[0] = createTexture(renderer, "tree.bmp");
	pic[1]= createTexture(renderer, "watermelon.bmp");
	pic[2] = createTexture(renderer, "Pokeball.bmp");
	pic[3] = createTexture(renderer, "flame.bmp");
	pic[4] = createTexture(renderer, "tree.bmp");
	pic[5] = createTexture(renderer, "tree.bmp");
	int res = 0;
	SDL_Rect pics[6];
	for (int i = 0; i < 6; i++)
	{
		pics[i] = {300+(i%3)*400, 200 + (i/3)*400, 200, 200};
	}

	char hitsound[13] = "hitsound.wav";
	SDL_Event event;
	while (!quit)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) {
				quit = true;
				return res;
			}
			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
			{
				for (int i = 0; i < 6; i++)
				{
					if (is_square_hit(pics[i], event.button.x, event.button.y))
					{
						sound(hitsound);
						res = i;
						return res;
					}
				}
				if (is_square_hit(back, event.button.x, event.button.y))
				{
					sound(hitsound);
					return res;
				}
			}
		}
		
		
		SDL_RenderCopy(renderer, fonTexture, NULL, &fonRect);
		SDL_RenderCopy(renderer, backTexture, NULL, &back);
		for (int i = 0; i < 6; i++)
		{
			SDL_RenderCopy(renderer, pic[i], NULL, &pics[i]);
		}

		draw_text(renderer, textTexture, 14);
		SDL_RenderPresent(renderer);
	}
}

int difficulty(SDL_Renderer* renderer, TTF_Font* my_font) {
	char title[30] = u8"Выбор сложности";
	SDL_Texture* textTexture = get_text_texture(renderer, title, my_font);
	SDL_Rect back = { 90, 70, 100, 100 };
	SDL_Texture* backTexture = createTexture(renderer, "arrow.bmp");

	SDL_Texture* fonTexture = createTexture(renderer, "fon.bmp");
	SDL_Rect fonRect = { 0,0,SCREEN_WIDTH, SCREEN_HEIGHT };

	char dif1[] = u8"Легко      5 x 5";
	char dif2[] = u8"Средне      10 x 10";
	char dif3[] = u8"Сложно      15 x 15";
	char dif4[] = u8"Профи      20 x 20";

	SDL_Texture* difTexture[4];
	difTexture[0] = get_text_texture(renderer, dif1, my_font);
	difTexture[1] = get_text_texture(renderer, dif2, my_font);
	difTexture[2] = get_text_texture(renderer, dif3, my_font);
	difTexture[3] = get_text_texture(renderer, dif4, my_font);

	SDL_Rect difs[4];
	for (int i = 0; i < 4; i++)
	{
		difs[i] = { 300, 200 + i * 150, 800, 80 };
	}

	int res = 0;
	char hitsound[13] = "hitsound.wav";
	SDL_Event event;
	while (!quit)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) {
				quit = true;
				return res;
			}
			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
			{
				for (int i = 0; i < 4; i++)
				{
					if (is_square_hit(difs[i], event.button.x, event.button.y))
					{
						sound(hitsound);
						res = i;
						return res;
					}
				}
				if (is_square_hit(back, event.button.x, event.button.y))
				{
					sound(hitsound);
					return res;
				}
			}
		}


		SDL_RenderCopy(renderer, fonTexture, NULL, &fonRect);
		SDL_RenderCopy(renderer, backTexture, NULL, &back);
		for (int i = 0; i < 4; i++)
		{
			SDL_RenderCopy(renderer, difTexture[i], NULL, &difs[i]);
		}

		draw_text(renderer, textTexture, 14);
		SDL_RenderPresent(renderer);
	}
}

int records(SDL_Renderer* renderer, TTF_Font* my_font, int pic, int dif){
	char title[30] = u8"Рекорды";
	SDL_Texture* textTexture = get_text_texture(renderer, title, my_font);
	SDL_Rect back = { 90, 70, 100, 100 };
	SDL_Texture* backTexture = createTexture(renderer, "arrow.bmp");

	SDL_Texture* fonTexture = createTexture(renderer, "fon.bmp");
	SDL_Rect fonRect = { 0,0,SCREEN_WIDTH, SCREEN_HEIGHT };

	int* pic_mas = (int*)malloc(sizeof(int));
	int* dif_mas = (int*)malloc(sizeof(int));
	char** timer = (char**)malloc(sizeof(char*));
	int* hp = (int*)malloc(sizeof(int));
	char bufer1[20], bufer2[20], bufer3[20];
	char **buf = (char**)malloc(sizeof(char*));
	string lol;
	int counter = 0;
	int c;
	FILE* f = fopen("records.txt", "r");
	while ((c = getc(f)) != EOF) {
		fseek(f,-1,SEEK_CUR);
		pic_mas = (int*)realloc(pic_mas, sizeof(int) * (counter + 1));
		fgets(bufer1, sizeof(int), f);
		pic_mas[counter] = atoi(bufer1);
		dif_mas = (int*)realloc(dif_mas, sizeof(int) * (counter + 1));
		fgets(bufer2, sizeof(int), f);
		dif_mas[counter] = atoi(bufer2);
		timer = (char**)realloc(timer, sizeof(char*) * (counter + 1));
		buf = (char**)realloc(buf, sizeof(char*)*(counter+1));
		buf[counter] = (char*)malloc(sizeof(char) * 6);
		fgets(buf[counter], sizeof(char) * 6, f);
		for (int i = 0; i < 6; i++) {
			if (buf[counter][i] == '\n') { buf[counter][i] = '\0'; break; }
		}
		timer[counter] = (char*)malloc(sizeof(char) * strlen(buf[counter]) - 1);
		timer[counter] = buf[counter];
		hp = (int*)realloc(hp, sizeof(int) * (counter + 1));
		fgets(bufer3, sizeof(int), f);
		hp[counter] = atoi(bufer3);
		counter++;
	}

	SDL_Rect* pic_box = (SDL_Rect*)malloc(sizeof(SDL_Rect)* counter);
	SDL_Rect* dif_box = (SDL_Rect*)malloc(sizeof(SDL_Rect) * counter);
	SDL_Rect* time_box = (SDL_Rect*)malloc(sizeof(SDL_Rect) * counter);
	SDL_Rect* hp_box = (SDL_Rect*)malloc(sizeof(SDL_Rect) * counter);

	SDL_Texture* picTexture,* difTexture, *timeTexture, *hpTexture;
	string name;
	char* cname, chp;
	char txt[2];

	int res = 0;
	char hitsound[13] = "hitsound.wav";
	SDL_Event event;
	while (!quit)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) {
				quit = true;
				return res;
			}
			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
			{
				if (is_square_hit(back, event.button.x, event.button.y))
				{
					sound(hitsound);
					SDL_DestroyTexture(textTexture);
					SDL_DestroyTexture(backTexture);
					return res;
				}
			}
		}

		SDL_RenderCopy(renderer, fonTexture, NULL, &fonRect);
		SDL_RenderCopy(renderer, backTexture, NULL, &back);

		for (int i = 0; i < counter; i++)
		{
			switch (pic_mas[i]) {
			case 0: name = u8"Дерево"; break;
			case 1: name = u8"Арбуз"; break;
			case 2: name = u8"Покеболл"; break;
			case 3: name = u8"-"; break;
			case 4: name = u8"-"; break;
			case 5: name = u8"-"; break;
			default: return -1;
			}
			pic_box[i] = { 150, 200 + 70 * i, int(name.length() * 25), 50 };
			cname = &name[0];
			picTexture = get_text_texture(renderer, cname, my_font);
			SDL_RenderCopy(renderer, picTexture, NULL, &pic_box[i]);
			switch (dif_mas[i]) {
			case 0: name = u8"Легко"; break;
			case 1: name = u8"Средне"; break;
			case 2: name = u8"Сложно"; break;
			case 3: name = u8"Профи"; break;
			default: return -1;
			}
			dif_box[i] = { 600, 200 + 70 * i, int(name.length() * 25), 50 };
			cname = &name[0];
			difTexture = get_text_texture(renderer, cname, my_font);
			SDL_RenderCopy(renderer, difTexture, NULL, &dif_box[i]);
			time_box[i] = { 1100, 200 + 70 * i, int(strlen(timer[i]) * 25), 50};
			timeTexture = get_text_texture(renderer, timer[i], my_font);
			SDL_RenderCopy(renderer, timeTexture, NULL, &time_box[i]);
			chp = (char)(((int)'0') + hp[i]);
			txt[0] = chp;
			txt[1] = '\0';
			hp_box[i] = { 1300, 200 + 70 * i, int(strlen(txt) * 25), 50 };
			hpTexture = get_text_texture(renderer, txt, my_font);
			SDL_RenderCopy(renderer, hpTexture, NULL, &hp_box[i]);
			SDL_DestroyTexture(picTexture);
			SDL_DestroyTexture(difTexture);
			SDL_DestroyTexture(timeTexture);
			SDL_DestroyTexture(hpTexture);
		}
		draw_text(renderer, textTexture, 14);
		SDL_RenderPresent(renderer);
	}
	SDL_DestroyTexture(textTexture);
	SDL_DestroyTexture(backTexture);
}

int main(int argc, char** argv)
{
	system("chcp 1251");
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow(u8"Японский кроссворд",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	Mix_Init(0);
	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024);

	TTF_Init();
	TTF_Font* my_font = TTF_OpenFont("Text.ttf", 100);
	SDL_Texture* textTexture;
	SDL_Texture* lables[BUTTON_COUNT];
	SDL_Texture* pointsTexture[BUTTON_COUNT];

	SDL_Rect buttons[BUTTON_COUNT];
	SDL_Rect up[BUTTON_COUNT];
	SDL_Rect gear = { SCREEN_WIDTH - 150 ,SCREEN_HEIGHT - 150, 100, 100 };
	char title[36] = u8"Японский кроссворд";
	char lable1[13] = u8"Играть";
	char lable2[19] = u8"Сложность";
	char lable3[17] = u8"Картинки";
	char lable4[15] = u8"Рекорды";
	char lable5[11] = u8"Выход";

	SDL_Surface* gearImage = IMG_Load("gear.bmp");
	SDL_SetColorKey(gearImage, SDL_TRUE, SDL_MapRGB(gearImage->format, 255, 255, 255));
	SDL_Texture* gearTexture = SDL_CreateTextureFromSurface(renderer, gearImage);
	SDL_FreeSurface(gearImage);

	textTexture = get_text_texture(renderer, title, my_font);
	lables[0] = get_text_texture(renderer, lable1, my_font);
	lables[1] = get_text_texture(renderer, lable2, my_font);
	lables[2] = get_text_texture(renderer, lable3, my_font);
	lables[3] = get_text_texture(renderer, lable4, my_font);
	lables[4] = get_text_texture(renderer, lable5, my_font);
	initButtons(buttons);
	up[0] = { SCREEN_WIDTH / 2 - (30 * 6) / 2, 250, 30 * 6, 60 };
	up[1] = { SCREEN_WIDTH / 2 - (30 * 9) / 2, 250 + 100, 30 * 9, 60 };
	up[2] = { SCREEN_WIDTH / 2 - (30 * 8) / 2, 250 + 200, 30 * 8, 60 };
	up[3] = { SCREEN_WIDTH / 2 - (30 * 7) / 2, 250 + 300, 30 * 7, 60 };
	up[4] = { SCREEN_WIDTH / 2 - (30 * 5) / 2, 250 + 400, 30 * 5, 60 };
	char hitsound[13] = "hitsound.wav";
	SDL_Texture* textures[BUTTON_COUNT];

	int pic = 0, dif = 0;

	int x[2];
	x[0] = x[1] = SCREEN_WIDTH / 2;

	SDL_Texture* fonTexture = createTexture(renderer, "fon.bmp");
	SDL_Rect fonRect = {0,0,SCREEN_WIDTH, SCREEN_HEIGHT};

	Mix_VolumeMusic((x[0] - (SCREEN_WIDTH / 2 - (100 * 10) / 2)) / 8);
	Mix_Volume(-1, (x[1] - (SCREEN_WIDTH / 2 - (100 * 10) / 2)) / 8);

	SDL_Surface* ballImage = IMG_Load("button.bmp");
	SDL_SetColorKey(ballImage, SDL_TRUE, SDL_MapRGB(ballImage->format, 255, 255, 255));
	SDL_Texture* ballTexture = SDL_CreateTextureFromSurface(renderer, ballImage);
	SDL_FreeSurface(ballImage);

	SDL_Event event;
	loadmusic();
	while (!quit)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				quit = true;
			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
			{
				if (is_square_hit(buttons[0], event.button.x, event.button.y))
				{
					sound(hitsound);
					game(renderer, my_font, (dif+1)*5, pic);
				}
				if (is_square_hit(buttons[1], event.button.x, event.button.y))
				{
					sound(hitsound);
					dif = difficulty(renderer, my_font);
				}
				if (is_square_hit(buttons[2], event.button.x, event.button.y))
				{
					sound(hitsound);
					pic = picture(renderer, my_font);
				}
				if (is_square_hit(buttons[3], event.button.x, event.button.y))
				{
					sound(hitsound);
					records(renderer, my_font, pic, dif);
				}
				if (is_square_hit(buttons[4], event.button.x, event.button.y))
				{
					sound(hitsound);
					quit = true;
				}
				if (is_square_hit(gear, event.button.x, event.button.y))
				{
					gear.w = 0;
					gear.h = 0;
					sound(hitsound);
					options(renderer, my_font, x);
				}
			}
		}
		gear.w = 100;
		gear.h = 100;
		SDL_RenderCopy(renderer, fonTexture, NULL, &fonRect);
		SDL_RenderCopy(renderer, gearTexture, NULL, &gear);

		draw_balls(renderer, buttons, BUTTON_COUNT, ballTexture, lables, up);
		draw_text(renderer, textTexture, 19);

		SDL_RenderPresent(renderer);
	}
	SDL_DestroyTexture(fonTexture);
	SDL_DestroyTexture(textTexture);
	SDL_DestroyTexture(ballTexture);
	SDL_DestroyTexture(gearTexture);

	TTF_CloseFont(my_font);
	TTF_Quit();

	Mix_FreeMusic(fon);
	Mix_FreeChunk(Sound);
	Mix_CloseAudio();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}