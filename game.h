#ifndef COURSE_OF_THE_GAME
#define COURSE_OF_THE_GAME

#include"constants.h"
#include<math.h>
#include<stdio.h>
#include<cstdio>
#include<string.h>
#include<time.h>
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"

class Game{
    private:
        SDL_Event event;
	    SDL_Surface *screen, *charset;
	    SDL_Surface *car;
	    SDL_Surface *map;
	    SDL_Texture *scrtex;
        SDL_Texture *texture;
	    SDL_Window *window;
	    SDL_Renderer *renderer;
        SDL_Rect road;
        SDL_Rect carRect;
        SDL_Rect missile;

        struct positionConstants posConstants;  

        char text[128];
        char timeArr[32];

	    int t1, t2, quit, frames, rc, y = posConstants.mapStartingPosition, direction[2] = {0, 0}, countTheSaves;
	    double delta, worldTime, fpsTimer, fps, distance, carSpeed = DEFAULT_SPEED, timeStart, duration;
        long long score;
        bool gameLoaded;

        int czarny;
	    int zielony;
	    int czerwony;
	    int niebieski;
	    int fioletowy;

        int countBullets;
        typedef SDL_Rect BULLET;
        BULLET *bulletsArray = NULL;

    public:
        Game();

        //main functions | methods
        int setup();
        int loadImages();
        void mainLoop();
        void clearVideo();


        //function to display starting screen
        void startingScreen(const int czarny, const int zielony, const int czerwony, const int niebieski, const int fioletowy);

        //function for drawing all surfaces
        void draw(const int current, const int czerwony, const int niebieski);

        void drawRoad(SDL_Surface *screen, SDL_Surface *sprite);
        void drawCar(SDL_Surface *screen, SDL_Surface *sprite, int x);
        void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor);
        void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color);
        void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color);
        void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y);
        void DrawString(SDL_Surface *screen, int x, int y, const char *text, SDL_Surface *charset);
        void DrawInfoBar(SDL_Surface *screen, SDL_Surface *charset, const int czerwony, const int niebieski, double worldTime, int score);
        void showOnTheScreen();

        char* presentSaves(char **saves, int tempSaves);

        //handle getting color of the particular pixel
        SDL_Color GetPixelColor(const SDL_Surface *pSurface, const int X, const int Y);
        bool offTheRoad();

        //handle moving of the road / car
        void moveCarRoad();

        //reset the game and all the parameters
        void resetGame();

        //function returning current datetime
        char *datetime();

        //save | load the state of the game 
        void saveTheGame();
        void loadTheGame();
        void pauseTheGame();
        void startOfTheGame();

        void setupStruct();
        
        //shooting function
        void shoot();
        void addBullet();
        void incrementBulletCords();
};

#endif