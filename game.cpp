#include"game.h"
#include"constants.h"
#include<iostream>

Game::Game(){
    road.x = (SCREEN_WIDTH / 40) - 300;            			// set initial x-axis point of road
    carRect.x = (SCREEN_WIDTH / 2);                         // set initial x-axis point of car
    carRect.y = (SCREEN_HEIGHT / 1.5) - car->h / 2;         // set initial y-axis point of car

    // initial local variables
    frames = 0;
	fpsTimer = 0;
	fps = 0;
	quit = 0;
	worldTime = 0;
	distance = 0;
	score = 0;
    timeStart = SDL_GetTicks();
    duration = 10000;
	countTheSaves = 0;
	countBullets = 0;
	gameLoaded = false;

	BULLET *bulletsArray;
	bulletsArray = (BULLET*) malloc(sizeof(BULLET) * countBullets);
}


int Game::setup(){
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}

	// fullscreen mode
	//rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &renderer);
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
	if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	};
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "Szablon do zdania drugiego 2017");

	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
	                              0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
	                           SDL_TEXTUREACCESS_STREAMING,
	                           SCREEN_WIDTH, SCREEN_HEIGHT);

	// wy��czenie widoczno�ci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);
    
    return 0;
}

void Game::startingScreen(const int czarny, const int zielony, const int czerwony, const int niebieski, const int fioletowy){
	bool quit = false;
	while(!quit){
		SDL_FillRect(screen, NULL, fioletowy);
		DrawRectangle(screen, BAR_X, BAR_Y, SCREEN_WIDTH, BAR_H, czarny, niebieski);
		DrawString(screen, SCREEN_WIDTH / 3 - strlen(text) * 8 , HEADER_Y, "---- WELCOME TO THE SPY HUNTER ----", charset);
		DrawString(screen, SCREEN_WIDTH / 1.5 - strlen(text) * 8 , FOOTER_Y, "to start the game press 'p'", charset);
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_KEYDOWN:
					if(event.key.keysym.sym == SDLK_p) quit = true;
					break;
			}
		}
		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);
	}
}

int Game::loadImages(){
    // wczytanie obrazka cs8x8.bmp
	charset = SDL_LoadBMP("./cs8x8.bmp");
	if(charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	SDL_SetColorKey(charset, true, 0x000000);

    // load image of the car
	car = SDL_LoadBMP("./car.bmp");
	if(car == NULL) {
		printf("SDL_LoadBMP(car.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

    // load image of the road
	map = SDL_LoadBMP("./mappicture.bmp");
	if(map == NULL) {
		printf("SDL_LoadBMP(mappicture.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

    return 0;
}

void Game::mainLoop(){
    const int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	const int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	const int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	const int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
	const int fioletowy = SDL_MapRGB(screen->format, 184, 3, 244);	

    t1 = SDL_GetTicks();
	int lastUpdate = SDL_GetTicks();

	startingScreen(czarny, zielony, czerwony, niebieski, fioletowy);

    while(!quit) {
		int current = SDL_GetTicks();
		t2 = SDL_GetTicks();
		// here t2-t1 is the time in milliseconds since
		// the last screen was drawn
		// delta is the same time in seconds
		delta = (t2 - t1) * 0.001;
		t1 = t2;
		distance += carSpeed * delta;
		worldTime += delta;

		SDL_FillRect(screen, NULL, czarny);
        draw(current, czerwony, niebieski); //draw surfaces
		showOnTheScreen(); // show everything on the screen
		if(gameLoaded) { pauseTheGame(); gameLoaded = false; }
		// handling of events (if there were any)s
		while(SDL_PollEvent(&event)) {
			score+=carSpeed;
			switch(event.type) {
				case SDL_KEYDOWN:
					if(event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
					else if(event.key.keysym.sym == SDLK_UP) carSpeed = ACCELERATED_SPEED;
					else if(event.key.keysym.sym == SDLK_DOWN) carSpeed = DEACCELERATED_SPEED;
					else if(event.key.keysym.sym == SDLK_RIGHT){
						direction[0] = 1, direction[1] = 0;
					}
					else if(event.key.keysym.sym == SDLK_LEFT){
						direction[1] = 1, direction[0] = 0;
					}
					else if(event.key.keysym.sym == SDLK_n) resetGame();
 					else if(event.key.keysym.sym == SDLK_s) saveTheGame();
					else if(event.key.keysym.sym == SDLK_l) { loadTheGame(); gameLoaded = true; }
					else if(event.key.keysym.sym == SDLK_p) pauseTheGame();
					else if(event.key.keysym.sym == SDLK_SPACE) addBullet();
					break;
				case SDL_KEYUP:
					carSpeed = DEFAULT_SPEED;
					direction[0] = 0, direction[1] = 0;
					break;
				case SDL_QUIT:
					quit = 1;
					break;
			};
		};
		shoot();
		incrementBulletCords();
		moveCarRoad();
		if(offTheRoad()) resetGame();
	};
}

void Game::clearVideo(){
	free(bulletsArray);
    SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_FreeSurface(car);
	SDL_DestroyTexture(texture);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
}

void Game::draw(const int current, const int czerwony, const int niebieski){
    drawRoad(screen, map);
	int elapsedTime = SDL_GetTicks() - current;
	if(elapsedTime < 5){
		SDL_Delay(5 - elapsedTime);
	}
	DrawSurface(screen, car, carRect.x, SCREEN_HEIGHT / 1.5);
	DrawInfoBar(screen, charset, czerwony, niebieski, worldTime, score);
}

void Game::drawRoad(SDL_Surface *screen, SDL_Surface *sprite){
	SDL_Rect dest;	
	road.w = sprite->w;
	road.h = sprite->h;
	dest.x = road.x;
	dest.y = road.y;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &road);
}

void Game::drawCar(SDL_Surface *screen, SDL_Surface *sprite, int x){
    carRect.x = x - sprite->w / 2;
	carRect.w = sprite->w;
	carRect.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &carRect);
}

// draw a rectangle of size l by k
void Game::DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
                   Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};

// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void Game::DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for(int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};

// draw a single pixel
void Game::DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
};

// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void Game::DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
};

// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void Game::DrawString(SDL_Surface *screen, int x, int y, const char *text,
                SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	};
};

void Game::DrawInfoBar(SDL_Surface *screen, SDL_Surface *charset, const int czerwony, const int niebieski, double worldTime, int score){
    //display the bar with funcionalities
    DrawRectangle(screen, posConstants.topBarXY, posConstants.topBarXY, SCREEN_WIDTH - 8, 80, czerwony, niebieski);
	
    sprintf(text, "Maksymilian Igla, 193093, czas trwania = %.1lf s, Esc - wyjscie", worldTime);
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, posConstants.firstRow, text, charset);

	sprintf(text, "Arrows - moving the player in given direction, n - new game, f - finish");
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, posConstants.secondRow, text, charset);

	sprintf(text, "s - save the game, l - load the game, p - pause/continue, space - shooting");
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, posConstants.thirdRow, text, charset);
    
	//display the score
	sprintf(text, "SCORE: %d", score);
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, posConstants.fourthRow, text, charset);
}

SDL_Color Game::GetPixelColor(const SDL_Surface *pSurface, const int X, const int Y){
	// Bytes per pixel
  const Uint8 Bpp = pSurface->format->BytesPerPixel;

  /*
  Retrieve the address to a specific pixel
  pSurface->pixels  = an array containing the SDL_Surface' pixels
  pSurface->pitch       = the length of a row of pixels (in bytes)
  X and Y               = the offset on where on the image to retrieve the pixel; (0, 0) is the upper left corner
  */
  Uint8* pPixel = (Uint8*)pSurface->pixels + Y * pSurface->pitch + X * Bpp;

  Uint32 PixelData = *(Uint32*)pPixel;

  SDL_Color Color = {0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE};

  // Retrieve the RGB values of the specific pixel
  SDL_GetRGB(PixelData, pSurface->format, &Color.r, &Color.g, &Color.b);

  return Color;
}

void Game::moveCarRoad(){
	if(direction[0] == 1) carRect.x+=2;
	else if(direction[1] == 1) carRect.x-=2;
	y += carSpeed;
	road.y = y;
	if(y >= 0) y = posConstants.mapResetPosition;	
	score += carSpeed;
}

void Game::resetGame(){
	worldTime = 0; 
	score = 0;
	y = posConstants.mapStartingPosition;
	carRect.x = ((SCREEN_WIDTH / 2));
	distance = 0;
	delta = 0;
}

bool Game::offTheRoad(){
	SDL_Color pixelColor=GetPixelColor(map, carRect.x, y*(-1));
	if(pixelColor.r == 60 || pixelColor.r == 180 || pixelColor.r == 76) return true;
	return false;
}

char *Game::datetime(){
	time_t seconds;
	struct tm *timeStruct;
	seconds = time(NULL);
	timeStruct = localtime(&seconds);
	sprintf(timeArr, "%d-%d-%d-%d-%d-%d.txt", timeStruct->tm_year+1900, timeStruct->tm_mon+1, timeStruct->tm_mday,timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec);
	return timeArr;
}	

void Game::saveTheGame(){
	FILE* save = fopen(datetime(), "w");
	FILE* fileWithSaves = fopen("saves.txt", "a");
	FILE* savesCount = fopen("savesCount.txt", "w+");

	fscanf(savesCount, "%d\n", &countTheSaves);
	fprintf(savesCount, "%d\n", ++countTheSaves);

	fprintf(fileWithSaves, "%s\n", datetime());

	fprintf(save, "%d\n", carRect.x);
	fprintf(save, "%d\n", road.y);
	fprintf(save, "%lld\n", score);
	fprintf(save, "%lf\n", worldTime);	

	fclose(fileWithSaves);
	fclose(save);
	fclose(savesCount);
}

typedef struct {
	int x;
	int y;
	int l;
	int k;
} linesCords;

linesCords line[4];

void Game::setupStruct(){
	line[0].x = 160, line[0].y = 29, line[0].l = SCREEN_WIDTH/2, line[0].k = 1;
	line[1].x = 480, line[1].y = 29, line[1].l = 1, line[1].k = 10;
	line[2].x = 160, line[2].y = 39, line[2].l = SCREEN_WIDTH/2, line[2].k = 1;
	line[3].x = 160, line[3].y = 29, line[3].l = 1, line[3].k = 10;
}

char* Game::presentSaves(char** saves, int tempSaves){
	setupStruct();
	char* toLoad;
	int increment = 30, select = 0;
	bool quit = false;
	SDL_FillRect(screen, NULL, fioletowy);
	DrawString(screen, screen->w / 2.5, posConstants.firstRow, "-- SAVED GAMES --", charset);
	for(int i=0; i<tempSaves; i++){
		DrawString(screen, screen->w / 2.5, increment, saves[i], charset);
		increment+=12;
		showOnTheScreen();
	}
	while(!quit){
		DrawRectangle(screen, line[0].x, line[0].y, line[0].l, line[0].k, zielony, zielony);
		DrawRectangle(screen, line[1].x, line[1].y, line[1].l, line[1].k, zielony, zielony);
		DrawRectangle(screen, line[2].x, line[2].y, line[2].l, line[2].k, zielony, zielony);
		DrawRectangle(screen, line[3].x, line[3].y, line[3].l, line[3].k, zielony, zielony);

		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_KEYDOWN:
					if(event.key.keysym.sym == SDLK_q) quit = true;
					if(event.key.keysym.sym == SDLK_e){ 
						toLoad = saves[select];
						quit = true; 
					}
					if(event.key.keysym.sym == SDLK_UP) if(line[0].y != 29){ 
						DrawRectangle(screen, line[0].x, line[0].y, line[0].l, line[0].k, fioletowy, fioletowy);
						DrawRectangle(screen, line[1].x, line[1].y, line[1].l, line[1].k, fioletowy, fioletowy);
						DrawRectangle(screen, line[2].x, line[2].y, line[2].l, line[2].k, fioletowy, fioletowy);
						DrawRectangle(screen, line[3].x, line[3].y, line[3].l, line[3].k, fioletowy, fioletowy);
						for(int i=0; i<4; i++) { line[i].y -= 11; }
						--select;
					}
					if(event.key.keysym.sym == SDLK_DOWN) if(line[2].y != 39 + ((tempSaves-1)*11)){
						DrawRectangle(screen, line[0].x, line[0].y, line[0].l, line[0].k, fioletowy, fioletowy);
						DrawRectangle(screen, line[1].x, line[1].y, line[1].l, line[1].k, fioletowy, fioletowy);
						DrawRectangle(screen, line[2].x, line[2].y, line[2].l, line[2].k, fioletowy, fioletowy);
						DrawRectangle(screen, line[3].x, line[3].y, line[3].l, line[3].k, fioletowy, fioletowy);
						for(int i=0; i<4; i++) { line[i].y += 11; }
						++select;
					}
					break;
			}
		}
		showOnTheScreen();
	}
	return toLoad;
}

void Game::loadTheGame(){
	FILE* fileWithSaves = fopen("saves.txt", "r");
	FILE* savesCount = fopen("savesCount.txt", "r");
	int tempSaves;
	fscanf(savesCount, "%d", &tempSaves);
	char **savesArr;
	savesArr = new char*[tempSaves];
	for(int i=0; i<tempSaves; i++){
		savesArr[i] = new char[22];
		fscanf(fileWithSaves, "%s\n", savesArr[i]);
	}	
	char* chosenSave = presentSaves(savesArr, tempSaves); 
	FILE* fileToLoad = fopen(chosenSave, "r");
	fscanf(fileToLoad, "%d\n", &carRect.x);
	fscanf(fileToLoad, "%d\n", &road.y);
	fscanf(fileToLoad, "%lld\n", &score);
	fscanf(fileToLoad, "%lf\n", &worldTime);
	for(int i=0; i<tempSaves; i++) delete[] savesArr[i];
	delete[] savesArr;
	fclose(fileWithSaves);
	fclose(savesCount);
	fclose(fileToLoad);
}

void Game::pauseTheGame(){
	bool quit = false;
	while(!quit){
		DrawString(screen, screen->w / 2.5 , SCREEN_HEIGHT/3, "-- GAME PAUSED --", charset);
		while(SDL_PollEvent(&event)){
			switch(event.type){
				case SDL_KEYDOWN:
					if(event.key.keysym.sym == SDLK_p) quit = true;
					break;
			}
		}
		showOnTheScreen();
	}	
}

void Game::showOnTheScreen(){
	SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);	
}


void Game::shoot(){
	for(int i=0; i<countBullets; i++){
		if(bulletsArray[i].y <= 0) bulletsArray = (BULLET*) realloc(bulletsArray, sizeof(BULLET) * (--countBullets));
		SDL_FillRect(screen, bulletsArray, 255);
		SDL_RenderDrawRect(renderer, bulletsArray);
		showOnTheScreen();
	}
}

void Game::addBullet(){
	bulletsArray = (BULLET*) realloc(bulletsArray, sizeof(BULLET) * (++countBullets));

	bulletsArray[countBullets-1].x = carRect.x;
	bulletsArray[countBullets-1].y = carRect.y-5;
	bulletsArray[countBullets-1].w = 2;
	bulletsArray[countBullets-1].h = 7;

}

void Game::incrementBulletCords(){
	for(int i=0; i<countBullets; i++) ++bulletsArray[i].y;
}