#include<stdio.h>
#define SDL_MAIN_HANDLED
#include "./include/SDL.h"
#include "./include/SDL_image.h"
#include "./include/SDL_mixer.h"
#define true 1
#define false 0
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT  480
#define MAP_WIDTH 20
#define MAP_HEIGHT 15
#define CELL_SIZE 32
#define BLOCKS 300
#define ROCKS 5
#define DIAMONDS 6
#define SPIDERS 3
#define MONSTERS 2
#define STAGES 6
#define GLYPH_HEIGHT 28
#define GLYPH_WIDTH  18

enum Block {background, earth, diamond, border, rock, door, spider, monster};

typedef struct {
	SDL_Renderer *renderer;
	SDL_Window *window;
} App;

typedef struct {
	int x;
	int y;
	int corX;
	int corY;
	int live;
	int lifes;
	int diamonds;
	int score;
	SDL_Texture* texture;
}Miner;

typedef struct {
	int x;
	int y;
	int live;
	int currentTime;
	int lastTime;
	int side;
	SDL_Texture* texture;
}Spider;

typedef struct {
	int x;
	int y;
	int live;
	int xDif;
	int yDif;
	int currentTime;
	int lastTime;
	SDL_Texture* texture;
}Monster;

typedef struct {
	int x;
	int y;
	SDL_Texture* texture;
}Block;

typedef struct {
	int x;
	int y;
	int fall;
	int cooldown;
	int startTime;
	int endTime;
	SDL_Texture* texture;
}Rock;

typedef struct {
	int lastTime;
	int currentTime;
	int second;
}Time;

void loadMap();

SDL_Texture* rockT;
SDL_Texture* earthT;
SDL_Texture* backgroundT;
SDL_Texture* borderT;
SDL_Texture* diamondT;
SDL_Texture* doorT;
SDL_Texture* spiderT;
SDL_Texture* fontTexture;
SDL_Texture* monsterT;
SDL_Texture* heartT;

Mix_Music *music;
App app;
Miner player;
Rock rocks[ROCKS];
Spider spiders[SPIDERS];
Monster monsters[MONSTERS];
Time time;
int diamondGoal;
int diamondGoals[STAGES] = { 6,8,10,12,14,14};
int stageTimes[STAGES] = { 30,30,40,50,60,70 };
int Stages[STAGES];
int gameover = false;
int currentLevel;

int map1Start[STAGES][MAP_HEIGHT][MAP_WIDTH] = {

	{
		{border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border},
		{border, earth, earth, earth, earth, earth, earth, earth, earth, rock, earth, earth, earth, earth, earth, earth, earth, earth, earth, border},
		{border, earth, earth, earth, earth, earth, earth, earth, earth, diamond, earth, earth, earth, earth, earth, earth, earth, earth, earth, border},
		{border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, rock, earth, earth, border},
		{border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, diamond, earth, earth, border},
		{border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border},
		{border, earth, earth, earth, earth, rock, earth, earth, earth, earth, earth, earth, earth, earth, earth, background, background, earth, earth, border},
		{border, earth, earth, earth, earth, diamond, earth, earth, earth, earth, earth, earth, earth, rock, earth, background, spider, earth, earth, border},
		{border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, diamond, earth, earth, earth, earth, earth, border},
		{border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, diamond, earth, earth, earth, earth, earth, border},
		{border, earth, earth, rock, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border},
		{border, earth, earth, diamond, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border},
		{border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border},
		{border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border},
		{border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border},
	},
	{
		{ border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, rock, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, rock, earth, earth, earth, earth, earth, earth, earth, diamond, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, diamond, earth, earth, earth, earth, earth, background, background, background, earth, earth, border },
		{ border, earth, earth, earth, earth, rock, earth, earth, earth, earth, earth, earth, earth, earth, background, background, background, earth, earth, border },
		{ border, earth, earth, earth, earth, diamond, earth, earth, earth, earth, earth, earth, rock, earth, background, background, spider, earth, earth, border },
		{ border, earth, earth, earth, background, background, earth, earth, earth, earth, earth, earth, diamond, earth, earth, earth, earth, earth, earth, border },
		{ border, earth, earth, earth, background, spider, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, earth, earth, earth, diamond, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, rock, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, diamond, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border },
	},
	{
		{ border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, rock, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, diamond, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, rock, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, rock, earth, earth, earth, earth, earth, earth, earth, diamond, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, diamond, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, earth, earth, earth, earth, rock, earth, earth, earth, earth, earth, earth, earth, earth, earth, background, background, earth, earth, border },
		{ border, earth, earth, earth, earth, diamond, earth, earth, earth, earth, earth, earth, earth, earth, earth, background, spider, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, earth, background, background, monster, background, background, earth, earth, earth, earth, earth, earth, rock, earth, earth, earth, earth, earth, border },
		{ border, earth, background, earth, diamond, earth, background, earth, earth, earth, earth, earth, earth, diamond, earth, earth, earth, earth, earth, border },
		{ border, earth, background, background, background, background, background, earth, earth, earth, earth, earth, earth, diamond, earth, earth, earth, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border },
	},
	{
		{ border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, rock, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, diamond, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, rock, earth, earth, border },
		{ border, background, background, background, background, background, background, background, rock, earth, earth, earth, earth, earth, earth, earth, diamond, earth, earth, border },
		{ border, background, earth, earth, earth, earth, earth, background, diamond, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, background, earth, earth, earth, rock, earth, background, earth, earth, earth, earth, earth, earth, earth, background, background, earth, earth, border },
		{ border, background, earth, earth, earth, diamond, earth, background, earth, earth, earth, earth, earth, earth, earth, background, spider, earth, earth, border },
		{ border, background, earth, earth, earth, earth, earth, background, earth, earth, earth, earth, earth, earth, rock, earth, earth, earth, earth, border },
		{ border, background, earth, earth, earth, earth, earth, background, earth, earth, earth, earth, earth, earth, diamond, earth, earth, earth, earth, border },
		{ border, background, earth, earth, earth, earth, earth, background, earth, earth, earth, earth, earth, earth, diamond, earth, earth, earth, earth, border },
		{ border, monster, earth, earth, earth, earth, earth, background, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, background, background, background, background, background, background, background, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border },
	},
	{
		{ border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, rock, earth, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, diamond, earth, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, rock, earth, earth, earth, earth, earth, earth, background, spider, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, diamond, earth, earth, earth, earth, earth, earth, background, background, earth, earth, border },
		{ border, earth, earth, earth, earth, rock, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, earth, earth, earth, earth, diamond, earth, earth, earth, earth, earth, earth, earth, rock, earth, earth, earth, earth, earth, border },
		{ border, earth, earth, earth, earth, spider, background, earth, earth, earth, earth, earth, earth, diamond, earth, earth, earth, earth, earth, border },
		{ border, earth, earth, earth, earth, background, background, earth, earth, earth, earth, earth, earth, diamond, earth, earth, earth, earth, earth, border },
		{ border, earth, rock, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, earth, diamond, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, earth, spider, background, background, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border },
	},
	{
		{ border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, rock, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, diamond, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, background, background, background, background, background, background, monster, background, background, background, background, background, background, background, background, background, background, background, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, rock, earth, earth, earth, earth, earth, earth, earth, diamond, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, diamond, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, earth, earth, earth, earth, rock, earth, earth, earth, earth, earth, earth, earth, earth, earth, background, background, earth, earth, border },
		{ border, earth, earth, earth, earth, diamond, earth, earth, earth, earth, earth, earth, earth, rock, earth, background, spider, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, diamond, earth, earth, earth, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, background, background, background, background, background, background, monster, background, background, background, background, background, background, background, background, background, background, background, border },
		{ border, earth, rock, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, earth, diamond, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, earth, border },
		{ border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border, border },
	},

};
		
int map[MAP_HEIGHT][MAP_WIDTH];

void initSDL() {
	
	int windowFlags, rendererFlags;

	rendererFlags = SDL_RENDERER_ACCELERATED;
	windowFlags = 0;

	SDL_Init(SDL_INIT_VIDEO);
	app.window = SDL_CreateWindow("Boulder Dash", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);
	
	app.renderer = SDL_CreateRenderer(app.window, -1, rendererFlags);
	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
}

void blit(SDL_Texture *texture, int x, int y)
{
	SDL_Rect dest;

	dest.x = x;
	dest.y = y;
	SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

	SDL_RenderCopy(app.renderer, texture, NULL, &dest);
}

void blitRect(SDL_Texture *texture, SDL_Rect *src, int x, int y)
{
	SDL_Rect dest;

	dest.x = x;
	dest.y = y;
	dest.w = src->w;
	dest.h = src->h;

	SDL_RenderCopy(app.renderer, texture, src, &dest);
}

void resetFalls() {
	int i;
	for (i = 0; i < ROCKS; i++) {
		rocks[i].fall = false;
	}
}

void resetSpiders() {
	int i;
	for (i = 0; i < SPIDERS; i++) {
		spiders[i].live = false;
		spiders[i].side = 0;
	}
}

void resetMonsters() {
	int i;
	for (i = 0; i < MONSTERS; i++) {
		monsters[i].live = false;
	}
}

void resetStage() {

	int i, j, z;
	for (z = 0; z < STAGES; z++) {
		if (Stages[z] == false) {
			for (i = 0; i < MAP_HEIGHT; i++) {
				for (j = 0; j < MAP_WIDTH; j++) {
					map[i][j] = map1Start[z][i][j];
				}
			}
			diamondGoal = diamondGoals[z];
			time.second = stageTimes[z];
			currentLevel = z + 1;
			player.diamonds = 0;
			player.live = true;
			player.x = 32;
			player.y = 32;
			player.corX = player.x / CELL_SIZE;
			player.corY = player.y / CELL_SIZE;	
			resetFalls();
			resetSpiders();
			resetMonsters();
			loadMap();
			return;
		}
	}

}

void drawText(int x, int y, int c)
{
	SDL_Rect rect;
	
	rect.w = GLYPH_WIDTH;
	rect.h = GLYPH_HEIGHT;
	rect.y = 0;

	SDL_SetTextureColorMod(fontTexture, 255, 255, 255);

	while (c>=1 || c==0)
	{
		if (c == 0) {
			rect.x = (c + 16) * GLYPH_WIDTH;
			c = -1;
		}
		else if (c >= 10) {
			rect.x = (c / 10 + 16) * GLYPH_WIDTH;
			c = c % 10;
		}
		else {
			rect.x = (c % 10 + 16) * GLYPH_WIDTH;
			c = c / 10;
			if (c == 0) {
				c = -1;
			}
		}
		
		blitRect(fontTexture, &rect, x, y);

		x += GLYPH_WIDTH;

	}
}

void counter() {

	time.currentTime = SDL_GetTicks();
	if (time.currentTime - time.lastTime >= 1000) {
		time.lastTime = time.currentTime;
		time.second -= 1;
		if (time.second < 0) {
			player.lifes -= 1;
			resetStage();
		}
	}
	drawText(MAP_WIDTH/2*CELL_SIZE, 0, time.second);
}

void PushRock(int x) {
	int i;
	for (i = 0; i < ROCKS; i++) {
		if (rocks[i].x == player.x && rocks[i].y == player.y) {
			if (x == SDL_SCANCODE_RIGHT) {
				rocks[i].x += CELL_SIZE;	
			}else if (x == SDL_SCANCODE_LEFT) {
				rocks[i].x -= CELL_SIZE;
			}
			map[player.corY][rocks[i].x / CELL_SIZE] = rock;
			blit(rocks[i].texture, rocks[i].x, rocks[i].y);
		}
	}
}

void move(SDL_KeyboardEvent *event) {
	if (player.live == true) {
		if (event->repeat == 0)
		{
			if (event->keysym.scancode == SDL_SCANCODE_UP)
			{
				if (map[player.corY - 1][player.corX] == diamond) {
					player.diamonds+=1;
					player.score += 1;
					player.y -= CELL_SIZE;
				}
				else if (map[player.corY - 1][player.corX] != border && map[player.corY - 1][player.corX] != rock) {
					player.y -= CELL_SIZE;
				}
			}

			if (event->keysym.scancode == SDL_SCANCODE_DOWN)
			{
				if (map[player.corY + 1][player.corX] == diamond) {
					player.diamonds += 1;
					player.score += 1;
					player.y += CELL_SIZE;
				}
				else if (map[player.corY + 1][player.corX] != border && map[player.corY + 1][player.corX] != rock) {
					player.y += CELL_SIZE;
				}
			}

			if (event->keysym.scancode == SDL_SCANCODE_LEFT)
			{
				if (map[player.corY][player.corX - 1] == rock) {
					if (map[player.corY][player.corX - 2] == background) {
						player.x -= CELL_SIZE;
						PushRock(SDL_SCANCODE_LEFT);
					}
				}
				else if (map[player.corY][player.corX - 1] == diamond) {
					player.diamonds += 1;
					player.score += 1;
					player.x -= CELL_SIZE;
				}
				else if (map[player.corY][player.corX - 1] != border) {
					player.x -= CELL_SIZE;
				}
			}

			if (event->keysym.scancode == SDL_SCANCODE_RIGHT)
			{
				if (map[player.corY][player.corX + 1] == rock) {
					if (map[player.corY][player.corX + 2] == background) {
						player.x += CELL_SIZE;
						PushRock(SDL_SCANCODE_RIGHT);
					}
				}
				else if (map[player.corY][player.corX + 1] == diamond) {
					player.diamonds += 1;
					player.score += 1;
					player.x += CELL_SIZE;
				}
				else if (map[player.corY][player.corX + 1] != border) {
					player.x += CELL_SIZE;
				}
			}

			player.corX = player.x / CELL_SIZE;
			player.corY = player.y / CELL_SIZE;
			map[player.corY][player.corX] = background;
		}
	}
}

void doInput(void)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			exit(0);
			break;
		case SDL_KEYDOWN:
			move(&event.key);		
			break;
		
		default:
			break;
		}
	}
}

void cleanup()
{
	SDL_DestroyRenderer(app.renderer);

	SDL_DestroyWindow(app.window);

	SDL_Quit();
}

void prepareScene(void)
{
	SDL_SetRenderDrawColor(app.renderer, 96, 128, 255, 255);
	SDL_RenderClear(app.renderer);
}

void presentScene(void)
{
	SDL_RenderPresent(app.renderer);
}

SDL_Texture *loadTexture(char *filename)
{
	SDL_Texture *texture=NULL;
	
	texture = IMG_LoadTexture(app.renderer, filename);
	
	return texture;
}

void loadMap() {
	int i, j, k = 0, l = 0, m = 0, n=0; 
	int x = 0, y = 0;

	for (i = 0; i < MAP_HEIGHT; i++) {
		for (j = 0; j < MAP_WIDTH; j++) {
			if (map[i][j] == rock) {
				rocks[l].texture = rockT;
				rocks[l].x = x;
				rocks[l].y = y;
				blit(rocks[l].texture, rocks[l].x, rocks[l].y);
				l++;
			}
			else if (map[i][j] == spider) {
				spiders[m].texture = spiderT;
				spiders[m].x = x;
				spiders[m].y = y;
				spiders[m].live = true;
				blit(spiders[m].texture, spiders[m].x, spiders[m].y);
				m++;	
			}
			else if (map[i][j] == monster) {
				monsters[n].texture = monsterT;
				monsters[n].x = x;
				monsters[n].y = y;
				monsters[n].live = true;
				blit(monsters[n].texture, monsters[n].x, monsters[n].y);
				n++;
			}
			else {
				if (map[i][j] == background) {
					blit(backgroundT, x, y);
				}
				else if (map[i][j] == earth) {
					blit(earthT, x, y);
				}
				else if (map[i][j] == diamond) {
					blit(diamondT, x, y);
				}
				else if (map[i][j] == border) {
					blit(borderT, x, y);
				}
				else if (map[i][j] == door) {
					blit(doorT, x, y);
				}
			}
			x += CELL_SIZE;		
		}
		x = 0;
		y += CELL_SIZE;
	}
	
}

void updateMap() {

	int i, j;
	int n;
	int x = 80;

	for (i = 0; i < MAP_HEIGHT; i++) {
		for (j = 0; j < MAP_WIDTH; j++) {
			if (map[i][j] == background) {
				blit(backgroundT, j*CELL_SIZE, i*CELL_SIZE);
			}
			else if (map[i][j] == earth) {
				blit(earthT, j*CELL_SIZE, i*CELL_SIZE);
			}
			else if (map[i][j] == diamond) {
				blit(diamondT, j*CELL_SIZE, i*CELL_SIZE);
			}
			else if (map[i][j] == border) {
				blit(borderT, j*CELL_SIZE, i*CELL_SIZE);
			}
			else if (map[i][j] == door) {
				blit(doorT, j*CELL_SIZE, i*CELL_SIZE);
			}
			else if (map[i][j] == diamond) {
				blit(diamondT, j*CELL_SIZE, i*CELL_SIZE);
			}
		}
	}
	
	for (n = 0; n < ROCKS; n++) {
		blit(rockT, rocks[n].x, rocks[n].y);
	}
	for (n = 0; n < SPIDERS; n++) {
		if (spiders[n].live == true) {
			blit(spiderT, spiders[n].x, spiders[n].y);
		}
	}
	for (n = 0; n < MONSTERS; n++) {
		if (monsters[n].live == true) {
			blit(monsterT, monsters[n].x, monsters[n].y);
		}	
	}
	for (n = 0; n < player.lifes; n++) {
		blit(heartT, x, 0);
		x += 25;
	}

	counter();
	drawText(5, 0, currentLevel);
	drawText((MAP_WIDTH - 2) * CELL_SIZE , 0, player.score);
}

void doRock() {
	int i;
	int j;
	for (i = 0; i < ROCKS; i++) {
		if (rocks[i].fall ==false && rocks[i].y / CELL_SIZE + 1 == player.corY && rocks[i].x / CELL_SIZE == player.corX) {
		}
		else {
			if (rocks[i].fall == false) {
				rocks[i].fall = true;
			}
			if (map[rocks[i].y / CELL_SIZE + 1][rocks[i].x / CELL_SIZE] == background ) {
					rocks[i].startTime = SDL_GetTicks();
					if (rocks[i].startTime - rocks[i].endTime >= 400) {
						map[rocks[i].y / CELL_SIZE][rocks[i].x / CELL_SIZE] = background;
						rocks[i].y += CELL_SIZE;
						map[rocks[i].y / CELL_SIZE ][rocks[i].x / CELL_SIZE] = rock;
						rocks[i].endTime = rocks[i].startTime;				
				}			
			}
			else if(map[rocks[i].y / CELL_SIZE + 1][rocks[i].x / CELL_SIZE] != monster && map[rocks[i].y / CELL_SIZE + 1][rocks[i].x / CELL_SIZE] != spider){
				rocks[i].fall = false;
			}
		}	
		if (rocks[i].fall == true && rocks[i].y / CELL_SIZE  == player.corY && rocks[i].x / CELL_SIZE == player.corX) {
			player.live = false;
			player.lifes -= 1;
			resetStage();
			return;
		}
		if (rocks[i].fall == true && map[rocks[i].y / CELL_SIZE + 1][rocks[i].x / CELL_SIZE]==spider) {
			for(j = 0; j < SPIDERS; j++) {
				if (spiders[j].live == true && spiders[j].y == rocks[i].y + CELL_SIZE && spiders[j].x == rocks[i].x) {
					spiders[j].live = false;
					rocks[i].y += CELL_SIZE;
					map[spiders[j].y / CELL_SIZE][spiders[j].x / CELL_SIZE] = rock;
					if (map[spiders[j].y / CELL_SIZE + 1][spiders[j].x / CELL_SIZE] != border) {
						map[spiders[j].y / CELL_SIZE + 1][spiders[j].x / CELL_SIZE] = diamond;
					}
					if (map[spiders[j].y / CELL_SIZE + 1][spiders[j].x / CELL_SIZE + 1] != border) {
						map[spiders[j].y / CELL_SIZE + 1][spiders[j].x / CELL_SIZE + 1] = diamond;
					}
					if (map[spiders[j].y / CELL_SIZE][spiders[j].x / CELL_SIZE + 1] != border) {
						map[spiders[j].y / CELL_SIZE][spiders[j].x / CELL_SIZE + 1] = diamond;
					}
					if (map[spiders[j].y / CELL_SIZE - 1][spiders[j].x / CELL_SIZE] != border) {
						map[spiders[j].y / CELL_SIZE - 1][spiders[j].x / CELL_SIZE] = diamond;
					}
					if (map[spiders[j].y / CELL_SIZE - 1][spiders[j].x / CELL_SIZE - 1] != border) {
						map[spiders[j].y / CELL_SIZE - 1][spiders[j].x / CELL_SIZE - 1] = diamond;
					}
					if (map[spiders[j].y / CELL_SIZE][spiders[j].x / CELL_SIZE - 1] != border) {
						map[spiders[j].y / CELL_SIZE][spiders[j].x / CELL_SIZE - 1] = diamond;
					}
					if (map[spiders[j].y / CELL_SIZE + 1][spiders[j].x / CELL_SIZE - 1] != border) {
						map[spiders[j].y / CELL_SIZE + 1][spiders[j].x / CELL_SIZE - 1] = diamond;
					}
					if (map[spiders[j].y / CELL_SIZE - 1][spiders[j].x / CELL_SIZE + 1] != border) {
						map[spiders[j].y / CELL_SIZE - 1][spiders[j].x / CELL_SIZE + 1] = diamond;
					}
				}
			}
		}
		if (rocks[i].fall == true && map[rocks[i].y / CELL_SIZE + 1][rocks[i].x / CELL_SIZE] == monster) {
			for (j = 0; j < MONSTERS; j++) {
				if (monsters[j].live == true && monsters[j].y == rocks[i].y + CELL_SIZE && monsters[j].x == rocks[i].x) {
					monsters[j].live = false;
					rocks[i].y += CELL_SIZE;
					map[monsters[j].y / CELL_SIZE][monsters[j].x / CELL_SIZE] = rock;
					if (map[monsters[j].y / CELL_SIZE - 1][monsters[j].x / CELL_SIZE - 1] != border) {
						map[monsters[j].y / CELL_SIZE - 1][monsters[j].x / CELL_SIZE - 1] = diamond;
					}
					if (map[monsters[j].y / CELL_SIZE - 1][monsters[j].x / CELL_SIZE] != border) {
						map[monsters[j].y / CELL_SIZE - 1][monsters[j].x / CELL_SIZE] = diamond;
					}
					if (map[monsters[j].y / CELL_SIZE - 1][monsters[j].x / CELL_SIZE + 1] != border) {
						map[monsters[j].y / CELL_SIZE - 1][monsters[j].x / CELL_SIZE + 1] = diamond;
					}
					if (map[monsters[j].y / CELL_SIZE - 1][monsters[j].x / CELL_SIZE + 2] != border) {
						map[monsters[j].y / CELL_SIZE - 1][monsters[j].x / CELL_SIZE + 2] = diamond;
					}
					if (map[monsters[j].y / CELL_SIZE][monsters[j].x / CELL_SIZE + 2] != border) {
						map[monsters[j].y / CELL_SIZE][monsters[j].x / CELL_SIZE + 2] = diamond;
					}
					if (map[monsters[j].y / CELL_SIZE + 1][monsters[j].x / CELL_SIZE + 2] != border) {
						map[monsters[j].y / CELL_SIZE + 1][monsters[j].x / CELL_SIZE + 2] = diamond;
					}
					if (map[monsters[j].y / CELL_SIZE + 2][monsters[j].x / CELL_SIZE + 2] != border) {
						map[monsters[j].y / CELL_SIZE + 2][monsters[j].x / CELL_SIZE + 2] = diamond;
					}
					if (map[monsters[j].y / CELL_SIZE + 2][monsters[j].x / CELL_SIZE + 1] != border) {
						map[monsters[j].y / CELL_SIZE + 2][monsters[j].x / CELL_SIZE + 1] = diamond;
					}
					if (map[monsters[j].y / CELL_SIZE + 2][monsters[j].x / CELL_SIZE] != border) {
						map[monsters[j].y / CELL_SIZE + 2][monsters[j].x / CELL_SIZE] = diamond;
					}
					if (map[monsters[j].y / CELL_SIZE + 2][monsters[j].x / CELL_SIZE - 1] != border) {
						map[monsters[j].y / CELL_SIZE + 2][monsters[j].x / CELL_SIZE - 1] = diamond;
					}
					if (map[monsters[j].y / CELL_SIZE + 1][monsters[j].x / CELL_SIZE - 1] != border) {
						map[monsters[j].y / CELL_SIZE + 1][monsters[j].x / CELL_SIZE - 1] = diamond;
					}
					if (map[monsters[j].y / CELL_SIZE][monsters[j].x / CELL_SIZE - 1] != border) {
						map[monsters[j].y / CELL_SIZE][monsters[j].x / CELL_SIZE - 1] = diamond;
					}
				}
			}
		}
	}
}

void openDoor() {
	int z;
	if (map[MAP_HEIGHT - 2][MAP_WIDTH - 1] != door) {
		map[MAP_HEIGHT - 2][MAP_WIDTH - 1] = door;
	}
	if (map[MAP_HEIGHT - 2][MAP_WIDTH - 2] == rock) {
		map[MAP_HEIGHT - 2][MAP_WIDTH - 2] = background;
	}
	if (player.corY == MAP_HEIGHT - 2 && player.corX == MAP_WIDTH - 1) {
		for (z = 0; z < STAGES; z++) {
			if (Stages[z] == false) {
				Stages[z] = true;
				if (z == 2) {
					player.lifes = 3;
				}
				resetStage();
				return;
			}
		}
	}
}

void doSpider() {
	int i;

	for (i = 0; i < SPIDERS; i++) {
		
		if (spiders[i].live == true) {
			spiders[i].currentTime = SDL_GetTicks();
			if (map[spiders[i].y / CELL_SIZE][spiders[i].x / CELL_SIZE + 1] == background && spiders[i].currentTime - spiders[i].lastTime > 400 && spiders[i].side == 0) {
				map[spiders[i].y / CELL_SIZE][spiders[i].x / CELL_SIZE] = background;
				spiders[i].x += CELL_SIZE;
				map[spiders[i].y / CELL_SIZE][spiders[i].x / CELL_SIZE] = spider;
				spiders[i].lastTime = spiders[i].currentTime;
				if (map[spiders[i].y / CELL_SIZE][spiders[i].x / CELL_SIZE + 1] != background) {
					spiders[i].side += 1;
				}
			}
			else if (map[spiders[i].y / CELL_SIZE + 1][spiders[i].x / CELL_SIZE] == background && spiders[i].currentTime - spiders[i].lastTime > 400 && spiders[i].side == 1) {
				map[spiders[i].y / CELL_SIZE][spiders[i].x / CELL_SIZE] = background;
				spiders[i].y += CELL_SIZE;
				map[spiders[i].y / CELL_SIZE][spiders[i].x / CELL_SIZE] = spider;
				spiders[i].lastTime = spiders[i].currentTime;
				if (map[spiders[i].y / CELL_SIZE + 1][spiders[i].x / CELL_SIZE] != background) {
					spiders[i].side += 1;
				}
			}
			else if (map[spiders[i].y / CELL_SIZE][spiders[i].x / CELL_SIZE - 1] == background && spiders[i].currentTime - spiders[i].lastTime > 400 && spiders[i].side == 2) {
				map[spiders[i].y / CELL_SIZE][spiders[i].x / CELL_SIZE] = background;
				spiders[i].x -= CELL_SIZE;
				map[spiders[i].y / CELL_SIZE][spiders[i].x / CELL_SIZE] = spider;
				spiders[i].lastTime = spiders[i].currentTime;
				if (map[spiders[i].y / CELL_SIZE][spiders[i].x / CELL_SIZE - 1] != background) {
					spiders[i].side += 1;
				}
			}
			else if (map[spiders[i].y / CELL_SIZE - 1][spiders[i].x / CELL_SIZE] == background && spiders[i].currentTime - spiders[i].lastTime > 400 && spiders[i].side == 3) {
				map[spiders[i].y / CELL_SIZE][spiders[i].x / CELL_SIZE] = background;
				spiders[i].y -= CELL_SIZE;
				map[spiders[i].y / CELL_SIZE][spiders[i].x / CELL_SIZE] = spider;
				spiders[i].lastTime = spiders[i].currentTime;
				if (map[spiders[i].y / CELL_SIZE - 1][spiders[i].x / CELL_SIZE] != background) {
					spiders[i].side += 1;
				}
			}
			else if (spiders[i].currentTime - spiders[i].lastTime > 400) {
				spiders[i].side += 1;
				if (spiders[i].side > 3) {
					spiders[i].side = 0;
				}
			}
			if (spiders[i].y  == player.y && spiders[i].x == player.x) {
				player.live = false;
				player.lifes -= 1;
				resetStage();
				return;
			}
		}	
	}
}

void doMonster() {
	int i;
	for (i = 0; i < MONSTERS; i++) {
		if (monsters[i].live == true) {
			monsters[i].xDif = monsters[i].x - player.x;
			monsters[i].yDif = monsters[i].y - player.y;
			monsters[i].currentTime = SDL_GetTicks();
			if (monsters[i].currentTime - monsters[i].lastTime >= 400) {
				if (monsters[i].xDif > 0 && map[monsters[i].y / CELL_SIZE][monsters[i].x / CELL_SIZE - 1] == background) {
					map[monsters[i].y / CELL_SIZE][monsters[i].x / CELL_SIZE] = background;
					monsters[i].x -= CELL_SIZE;
					map[monsters[i].y / CELL_SIZE][monsters[i].x / CELL_SIZE] = monster;
					monsters[i].lastTime = monsters[i].currentTime;
				}
				else if (monsters[i].xDif < 0 && map[monsters[i].y / CELL_SIZE][monsters[i].x / CELL_SIZE + 1] == background) {
					map[monsters[i].y / CELL_SIZE][monsters[i].x / CELL_SIZE] = background;
					monsters[i].x += CELL_SIZE;
					map[monsters[i].y / CELL_SIZE][monsters[i].x / CELL_SIZE] = monster;
					monsters[i].lastTime = monsters[i].currentTime;

				}
				else if (monsters[i].yDif > 0 && map[monsters[i].y / CELL_SIZE - 1][monsters[i].x / CELL_SIZE] == background) {
					map[monsters[i].y / CELL_SIZE][monsters[i].x / CELL_SIZE] = background;
					monsters[i].y -= CELL_SIZE;
					map[monsters[i].y / CELL_SIZE][monsters[i].x / CELL_SIZE] = monster;
					monsters[i].lastTime = monsters[i].currentTime;
				}
				else if (monsters[i].yDif < 0 && map[monsters[i].y / CELL_SIZE + 1][monsters[i].x / CELL_SIZE] == background) {
					map[monsters[i].y / CELL_SIZE][monsters[i].x / CELL_SIZE] = background;
					monsters[i].y += CELL_SIZE;
					map[monsters[i].y / CELL_SIZE][monsters[i].x / CELL_SIZE] = monster;
					monsters[i].lastTime = monsters[i].currentTime;
				}
				else {
					monsters[i].lastTime = monsters[i].currentTime;
				}

				if (monsters[i].y == player.y && monsters[i].x == player.x) {
					player.live = false;
					player.lifes -= 1;
					resetStage();
					return;
				}
			}
		}
	}
}

int main()
{
	memset(&app, 0, sizeof(App));
	memset(&player, 0, sizeof(Miner));

	initSDL();
	
	for (int i = 0; i < 6; i++) {
		Stages[i] = false;
	}
	
	resetStage();

	player.lifes = 3;
	player.texture = loadTexture("assets/miner.png");
	
	doorT = loadTexture("assets/door.png");
	rockT = loadTexture("assets/rock.png");
	borderT = loadTexture("assets/border.png");
	diamondT = loadTexture("assets/diamond.png");
	backgroundT = loadTexture("assets/background.png");
	earthT = loadTexture("assets/earth.png");
	spiderT = loadTexture("assets/spider.png");
	monsterT = loadTexture("assets/monster.png");
	heartT = loadTexture("assets/heart.png");
	fontTexture = loadTexture("assets/font.png");
	
	music = Mix_LoadMUS("assets/theme.ogg");
	Mix_PlayMusic(music, -1 );
	atexit(cleanup);
	
	while (gameover==false)
	{
			prepareScene();

			updateMap();

			doInput();

			doSpider();

			doMonster();

			doRock();

			if (player.lifes <= 0) {
				gameover = true;
			}

			if (player.live == true) {
				blit(player.texture, player.x, player.y);
			}

			if (player.diamonds >= diamondGoal) {
				openDoor();
			}

			if (Stages[STAGES - 1] == true) {
				gameover = true;
			}

			presentScene();
		
	}
	return 0;
}
