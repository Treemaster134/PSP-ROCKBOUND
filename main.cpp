#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspaudio.h>
#include <psprtc.h>

#include <cstdio>
#include <math.h>
#include <cstdlib>
#include <cstring>
#include <vector>

#include <iostream>
#include <fstream>

#include "gfx.hpp"
#include "imageresources.hpp"


PSP_MODULE_INFO("Rockbound", 0,1,0);

struct Point
{
    int X;
    int Y;

    Point(int init_x, int init_y)
    {
        X = init_x;
        Y = init_y;
    }

    Point()
    {
        X = 0;
        Y = 0;
    }
};

/*
If you don't want to extract the raw audio from the wav files beforehand, you could read the wav directly (if it's uncompressed wav).
The file format is pretty simple.
In the first 44 bytes of the file you have the header and all the necessary info like data length, number of channels, ... and the size of the raw data to read.
*/


int playerPos[] = {1,1};


int r_Score = 0;
int r_Steps = 0;
int r_Treasure = 0; 

pspTime lastLoop; 
pspTime thisLoop;
sceRtcGetCurrentClockLocalTime(&lastLoop);
sceRtcGetCurrentClockLocalTime(&thisLoop);

int SFX_CHANNEL1 = 0;
int SFX_CHANNEL2 = 1;

int SND_CHANNELS[] = {
    0, 1, 2, 3
};

bool gameOver = false;
bool onMenu = true;
bool isPlayerDead = false;

int score = 0;

int requiredTreasure = 0;

int steps = 0;

int bonusSteps = 180;

int difficultyLevel = 0;

int collectedTreasure = 0;

int level = 0;


uint16_t *sound_pickup2;
uint16_t *sound_wall;
uint16_t *sound_select;
uint16_t *sound_tada;



std::vector<std::vector<std::vector<char>>> levels =
{
    {{'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},{'W', 'P', 'D', 'D', 'W', 'D', 'D', 'R', 'B', 'D', 'W', 'D', 'W', 'D', 'W', 'T', 'D', 'W', 'D', 'W'}, {'W', 'D', 'W', 'D', 'W', 'D', 'W', 'R', 'W', 'D', 'W', 'D', 'W', 'D', 'W', 'W', 'D', 'W', 'D', 'W'}, {'W', 'D', 'W', 'D', 'D', 'D', 'W', 'D', 'W', 'D', 'D', 'D', 'W', 'D', 'W', 'D', 'D', 'D', 'D', 'W'}, {'W', 'W', 'W', 'D', 'W', 'W', 'W', 'W', 'W', 'D', 'W', 'D', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W'}, {'W', 'D', 'W', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'D', 'W'}, {'W', 'D', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'W', 'W', 'W'}, {'W', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'W', 'T', 'W', 'D', 'W', 'D', 'W', 'T', 'D', 'W'}, {'W', 'W', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'D', 'W', 'D', 'W', 'D', 'W', 'W', 'D', 'W'}, {'W', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'D', 'W'}, {'W', 'W', 'W', 'D', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W'}, {'W', 'D', 'D', 'D', 'W', 'D', 'W', 'T', 'W', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'W'}, {'W', 'D', 'W', 'W', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'D', 'W'}, {'W', 'D', 'W', 'D', 'W', 'D', 'D', 'D', 'W', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'W'}, {'W', 'D', 'W', 'D', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'W', 'W', 'W'}, {'W', 'W', 'W', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'D', 'W'}, {'W', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'W', 'W', 'W', 'W', 'W', 'D', 'D', 'D', 'D', 'D', 'T', 'W'}, {'W', 'D', 'W', 'W', 'W', 'D', 'W', 'D', 'W', 'D', 'D', 'D', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'}, {'W', 'D', 'T', 'W', 'D', 'D', 'W', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'E', 'W'}, {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'}},
    {{'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},{'W', 'P', 'D', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'D', 'W'}, {'W', 'D', 'W', 'D', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'D', 'W', 'D', 'W', 'W', 'W', 'W', 'D', 'W'}, {'W', 'D', 'W', 'D', 'W', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'D', 'W'}, {'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'W', 'W', 'D', 'W', 'D', 'D', 'W'}, {'W', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'W', 'D', 'W', 'W'}, {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'D', 'W', 'W', 'W', 'D', 'D', 'W'}, {'W', 'D', 'D', 'T', 'W', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'W', 'D', 'W', 'T', 'W', 'D', 'D', 'W'}, {'W', 'D', 'W', 'W', 'W', 'D', 'W', 'D', 'W', 'D', 'W', 'W', 'W', 'W', 'W', 'D', 'W', 'W', 'D', 'W'}, {'W', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'D', 'W'}, {'W', 'W', 'W', 'D', 'W', 'W', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'D', 'D', 'W'}, {'W', 'D', 'W', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'D', 'T', 'W', 'T', 'W', 'D', 'W', 'W'}, {'W', 'D', 'W', 'D', 'W', 'W', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'W', 'W', 'D', 'W', 'D', 'D', 'W'}, {'W', 'D', 'D', 'D', 'D', 'D', 'D', 'T', 'W', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'D', 'W'}, {'W', 'D', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'W', 'W', 'D', 'W', 'W', 'D', 'W'}, {'W', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'D', 'W', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'W'}, {'W', 'W', 'W', 'D', 'W', 'D', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'W', 'D', 'W', 'D', 'W', 'W', 'W'}, {'W', 'T', 'W', 'D', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'D', 'W', 'D', 'W', 'E', 'W'}, {'W', 'D', 'D', 'D', 'D', 'D', 'D', 'W', 'T', 'T', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'W'}, {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'}},
    {{'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},{'W', 'P', 'D', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'W', 'D', 'W', 'T', 'D', 'W', 'D', 'W'}, {'W', 'D', 'W', 'D', 'W', 'D', 'W', 'D', 'W', 'D', 'W', 'D', 'W', 'D', 'W', 'W', 'D', 'W', 'D', 'W'}, {'W', 'D', 'W', 'D', 'D', 'D', 'W', 'D', 'W', 'D', 'D', 'D', 'W', 'D', 'W', 'D', 'D', 'D', 'D', 'W'}, {'W', 'W', 'W', 'D', 'W', 'W', 'W', 'W', 'W', 'D', 'W', 'D', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W'}, {'W', 'D', 'W', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'D', 'W'}, {'W', 'D', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'W', 'W', 'W'}, {'W', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'W', 'T', 'W', 'D', 'W', 'D', 'W', 'T', 'D', 'W'}, {'W', 'W', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'D', 'W', 'D', 'W', 'D', 'W', 'W', 'D', 'W'}, {'W', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'D', 'W'}, {'W', 'W', 'W', 'D', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W'}, {'W', 'D', 'D', 'D', 'W', 'D', 'W', 'T', 'W', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'W'}, {'W', 'D', 'W', 'W', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'D', 'W'}, {'W', 'D', 'W', 'D', 'W', 'D', 'D', 'D', 'W', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'W'}, {'W', 'D', 'W', 'D', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'W', 'W', 'W'}, {'W', 'W', 'W', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'D', 'W'}, {'W', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'W', 'W', 'W', 'W', 'W', 'D', 'D', 'D', 'D', 'D', 'T', 'W'}, {'W', 'D', 'W', 'W', 'W', 'D', 'W', 'D', 'W', 'D', 'D', 'D', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'}, {'W', 'D', 'T', 'W', 'D', 'D', 'W', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'E', 'W'}, {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'}}
};

char board[20][20] = {
    {'W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W'},{'W','T','T','T','R','R','R','D','D','W','D','D','D','D','D','R','T','T','T','W'},{'W','T','T','T','R','R','R','D','D','W','D','D','D','D','D','R','T','T','T','W'},{'W','T','T','T','R','R','R','D','D','W','D','D','D','D','D','R','T','T','T','W'},{'W','T','T','T','R','R','R','D','D','W','D','D','D','D','D','R','T','T','T','W'},{'W','T','T','T','R','R','R','D','D','W','D','D','D','D','D','R','T','T','T','W'},{'W','T','T','T','R','R','R','D','D','W','D','D','D','D','D','R','T','T','T','W'},{'W','T','T','T','R','R','R','D','D','W','D','D','D','D','D','R','T','T','T','W'},{'W','T','T','T','R','R','R','D','D','W','D','D','D','D','D','R','T','T','T','W'},{'W','T','T','T','R','R','R','D','D','P','D','D','D','D','D','R','T','T','T','W'},{'W','T','T','T','R','R','R','D','D','W','D','D','D','D','D','R','T','T','T','W'},{'W','T','T','T','R','R','R','D','D','W','D','D','D','D','D','R','T','T','T','E'},{'W','T','T','T','R','R','R','D','D','W','D','D','D','D','D','R','T','T','T','W'},{'W','T','T','T','R','R','R','D','D','W','D','D','D','D','D','R','T','T','T','W'},{'W','T','T','T','R','R','R','D','D','W','D','D','D','D','D','R','T','T','T','W'},{'W','T','T','T','R','R','R','D','D','W','D','D','D','D','D','R','T','T','T','W'},{'W','T','T','T','R','R','R','D','D','W','D','D','D','D','D','R','T','T','T','W'},{'W','T','T','T','R','R','R','D','D','W','D','D','D','D','D','R','T','T','T','W'},{'W','T','T','T','R','R','R','D','D','W','D','D','D','D','D','R','T','T','T','W'},{'W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W'}
};

int exit_callback(int arg1, int arg2, void* common)
{
    sceKernelExitGame();
    return 0;
}

int callbackThread(SceSize args, void* argp)
{
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);

    sceKernelSleepThreadCB();
    return 0;
}

void setupCallbacks()
{
    int thid = sceKernelCreateThread("update_thread", callbackThread, 0x11, 0xFA0, 0, NULL);
    if(thid >= 0)
    {
        sceKernelStartThread(thid, 0, NULL);
    }
}

//PORTED
void loadLevel()
{
    level += 1;
    bonusSteps = 180;
    r_Score = score;
    r_Steps = steps;
    r_Treasure = collectedTreasure;
    board[playerPos[1]][playerPos[0]] = 'B';
    playerPos[0] = 1;
    playerPos[1] =  1;
    requiredTreasure = 0;
    board[playerPos[1]][playerPos[0]] = 'P';

    if((level - 1) < levels.size())
    {
        sceAudioOutputBlocking(SND_CHANNELS[0], PSP_AUDIO_VOLUME_MAX * 0.5, sound_pickup2);
        for (int yP = 0; yP < 20; yP++)
        {
            for (int xP = 0; xP < 20; xP++)
            {
                board[yP][xP] = levels[level - 1][yP][xP];
                if(levels[level - 1][yP][xP] == 'P')
                {
                    playerPos[0] = xP;
                    playerPos[1] = yP;
                }
                else if(levels[level - 1][yP][xP] == 'T')
                {
                    requiredTreasure += 1;
                }
            }
        }
        requiredTreasure = requiredTreasure * 0.5;
    }
    else
    {
        sceAudioOutputBlocking(SND_CHANNELS[3], PSP_AUDIO_VOLUME_MAX, sound_tada);
        gameOver = true;
    }
    
    
    return;
}

void restartLevel()
{
    isPlayerDead = false;
    level -= 1;
    collectedTreasure = r_Treasure;
    steps = r_Steps;
    score = r_Score;
    loadLevel();
}

//PORTED
void startGame()
{
    bonusSteps = 180;
    level = 0;
    steps = 0;
    score = 0;
    collectedTreasure = 0;
    
    loadLevel();
    
}


void loadCustomLevel()
{
    
    FILE *lp;
    char line[1000];
    int x = 0;
    int y = 0;
    int CurLev = 0;
    // Open the file
    lp = fopen("level.rblv", "r");
    
    if (lp == NULL)
    {
        sceAudioOutputBlocking(SND_CHANNELS[1], PSP_AUDIO_VOLUME_MAX * 0.5, sound_wall);
        return;
    }
    else
    {

        

        levels = {};

    // Read each line until end of file
        while (fgets(line, sizeof(line), lp) != NULL) {

            levels.push_back({});
            levels[CurLev].push_back({});
            x = 0;
            y = 0;
            for (int i = 0; i < 419; i++) {
            if (line[i] == ',') {
                x = 0;
                y += 1;
                levels[CurLev].push_back({});
                continue;
            } else {
                levels[CurLev][y].push_back(line[i]);
            }
            }
            CurLev += 1;
        }

    }
    // Close the file
    fclose(lp);
    sceAudioOutputBlocking(SND_CHANNELS[0], PSP_AUDIO_VOLUME_MAX * 0.5, sound_pickup2);
    return;
}

Point checkNeighbors(Point p, bool falling)
{
    if((p.Y + 1) < 19)
    {
        if(board[p.Y + 1][p.X] == 'P' && falling == true)
        {
            isPlayerDead = true;
            //loseSound.Play();
            return p;
            /* prevent falling boulders from falling after restarting.*/
        }

        if(board[p.Y + 1][p.X] == 'B')
        {
            return checkNeighbors(Point(p.X, p.Y + 1), true);
        }
        
        if(board[p.Y + 1][p.X] == 'R' && falling == true)
        {
            if((p.X - 1) > -1)
            {
                if(board[p.Y + 1][p.X - 1] == 'B' && board[p.Y][p.X - 1] == 'B')
                {
                    return checkNeighbors(Point(p.X - 1, p.Y + 1), true);
                }
            }
            if((p.X + 1) < 19)
            {
                if(board[p.Y + 1][p.X + 1] == 'B' && board[p.Y][p.X + 1] == 'B')
                {
                    return checkNeighbors(Point(p.X + 1, p.Y + 1), true);
                }
            }
        }
    }
    

    return p;
}

void moveRocks()
{
    for(int i = 19; i >= 0; i--)
    {
        for(int j = 19; j >= 0; j--)
        {
            if(board[i][j] == 'R')
            {
                Point oldPos = Point(j, i);
                Point newPos = checkNeighbors(oldPos, false);
                if(newPos.X == 9999 || newPos.Y == 9999)
                {
                    return;
                }

                board[oldPos.Y][oldPos.X] = 'B';
                board[newPos.Y][newPos.X] = 'R';

                //board[oldPos.Y] = board[oldPos.Y].Remove(oldPos.X, 1).Insert(oldPos.X, "B");
                //board[newPos.Y] = board[newPos.Y].Remove(newPos.X, 1).Insert(newPos.X, "R");
            }
        }
    }

}



//ported
void move(int dX, int dY)
{

    if(isPlayerDead == true)
    {
        return;
    }

    int oldPos[2] = {playerPos[0], playerPos[1]};

    if(playerPos[0] + dX > 19 || playerPos[0] + dX < 0 || playerPos[1] + dY < 0 || playerPos[1] + dY > 19)
    {
        return;
    }
    if (board[playerPos[1] + dY][playerPos[0] + dX] == 'T')
    {
        sceAudioOutputBlocking(SND_CHANNELS[0], PSP_AUDIO_VOLUME_MAX * 0.5, sound_pickup2);
        score += 65;
        collectedTreasure += 1;
    }
    if (board[playerPos[1] + dY][playerPos[0] + dX] == 'W')
    {
        sceAudioOutputBlocking(SND_CHANNELS[1], PSP_AUDIO_VOLUME_MAX * 0.5, sound_wall);
        steps += 1;
        bonusSteps -= 1;
        return;
    }
    if (board[playerPos[1] + dY][playerPos[0] + dX] == 'E')
    {
        steps += 1;
        bonusSteps -= 1;

        if(collectedTreasure < requiredTreasure)
        {
            sceAudioOutputBlocking(SND_CHANNELS[1], PSP_AUDIO_VOLUME_MAX * 0.5, sound_wall);
            return;
        }

        if(bonusSteps > 0)
        {
            score += (bonusSteps * 20);
        }

        loadLevel();

        
        return;
        
    }
    if (board[playerPos[1] + dY][playerPos[0] + dX] == 'R')
    {
        if(dY == 0 && playerPos[0] + (dX * 2) >= 0 && playerPos[0] + (dX * 2) <= 19)
        {
            if(board[playerPos[1] + (dY * 2)][playerPos[0] + (dX * 2)] == 'B')
            {
                board[playerPos[1] + dY][playerPos[0] + dX] = 'B';
                board[playerPos[1] + (dY * 2)][playerPos[0] + (dX * 2)] = 'R';
            }
            else
            {
                steps += 1;
                bonusSteps -= 1;
                sceAudioOutputBlocking(SND_CHANNELS[1], PSP_AUDIO_VOLUME_MAX * 0.5, sound_wall);
                return;
            }
        }
        else
        {
            steps += 1;
            bonusSteps -= 1;
            sceAudioOutputBlocking(SND_CHANNELS[1], PSP_AUDIO_VOLUME_MAX * 0.5, sound_wall);
            return;
        }
    }

    
    

    board[playerPos[1]][playerPos[0]] = 'B';

    playerPos[0] = playerPos[0] + dX;
    playerPos[1] =  playerPos[1] + dY;

    board[playerPos[1]][playerPos[0]] = 'P';
    steps += 1;
    bonusSteps -= 1;
    moveRocks();
    return;
}

uint32_t ColorToUint32(int red, int green, int blue)
{
	return (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);
	
}

void drawMenu()
{
    GFX::clear(0xFF000000);
    GFX::drawImage(0,0,480,272,IMAGE_MENU);
    
    GFX::swapBuffers();
    sceDisplayWaitVblankStart();
}

void drawBoard()
{
    GFX::clear(0xFF000000);
    for (int y = 0; y < 20; y++)
    {
        for (int x = 0; x < 20; x++)
        {

            int drawingX = 6 + (13 * x);
            int drawingY = 6 + (13 * y);

            
            
            

            switch (board[y][x])
            {
                case 'P':
                    if(isPlayerDead == true)
                    {
                        GFX::drawImage(drawingX, drawingY, 13, 13, SPRITE_PLAYER_DEAD);
                        break;
                    }
                    GFX::drawImage(drawingX, drawingY, 13, 13, SPRITE_PLAYER);
                    break;
                case 'W':
                    GFX::drawImage(drawingX, drawingY, 13, 13, SPRITE_BRICK);
                    break;
                case 'B':
                    GFX::drawRect(drawingX, drawingY, 13, 13, 0xFF000000);
                    break;
                case 'T':
                    GFX::drawImage(drawingX, drawingY, 13, 13, SPRITE_DIAMOND);
                    break;
                case 'E':
                    if(collectedTreasure >= requiredTreasure)
                    {
                        GFX::drawImage(drawingX, drawingY, 13, 13, SPRITE_DOOR_OPEN);
                        break;
                    }
                    GFX::drawImage(drawingX, drawingY, 13, 13, SPRITE_DOOR);
                    break;
                case 'D':
                    GFX::drawImage(drawingX, drawingY, 13, 13, SPRITE_DIRT);
                    break;
                case 'R':
                    GFX::drawImage(drawingX, drawingY, 13, 13, SPRITE_BOULDER);
                    break;
                
                default:
                    GFX::drawRect(drawingX, drawingY, 13, 13, 0xFF000000);
                    break;
            }
        }
    }
    

    
    GFX::drawImage(276, 32, 78, 13, TEXT_SCORE);
    GFX::drawImage(276, 50, 78, 13, TEXT_STEPS);
    GFX::drawImage(276, 68, 78, 13, TEXT_BONUS);
    GFX::drawImage(276, 86, 78, 13, TEXT_LEVEL);
    

    GFX::drawRect(354, 0, 78, 32, 0xFF000000);
    GFX::drawInt(score, 354, 32);
    GFX::drawInt(steps, 354, 50);
    GFX::drawInt(collectedTreasure, 354, 68);
    GFX::drawRect(354,86, 126, 91, 0xFF000000);
    GFX::drawInt(level, 354, 86);
    
    int drawFPS = (thisLoop.microseconds - lastLoop.microseconds);
    GFX:drawInt(drawFPS, 430, 256);

    GFX::swapBuffers();
    sceDisplayWaitVblankStart();
    


}


void waitOnPress(void){
	SceCtrlData pad;
	sceCtrlReadBufferPositive(&pad, 1);
	
	while(pad.Buttons & PSP_CTRL_CROSS){
		sceCtrlReadBufferPositive(&pad, 1);
		sceKernelDelayThread(100000);
	}
	while(!(pad.Buttons & PSP_CTRL_CROSS)){
		sceCtrlReadBufferPositive(&pad, 1);
		sceKernelDelayThread(100000);
	}
	while(pad.Buttons & PSP_CTRL_CROSS){
		sceCtrlReadBufferPositive(&pad, 1);
		sceKernelDelayThread(100000);
	}
}


auto main() -> int{
    setupCallbacks();

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    SceCtrlData ctrlData, old;
	
	int size = 0;
	FILE *fp;
	fp = fopen("pickup.raw", "rb");
	if (fp){
		while(fgetc(fp) != EOF)
			size++;
		// align file size to the closest 64 bytes
		size = PSP_AUDIO_SAMPLE_ALIGN(size);
		
		sound_pickup2 = (uint16_t*)malloc(sizeof(uint16_t) * size);
		if (sound_pickup2){
			memset(sound_pickup2, 0, sizeof(uint16_t) * size);
			
			rewind(fp);
			// read raw data into buffer
			fread(sound_pickup2, sizeof(uint16_t), size, fp);
		}
		
		fclose(fp);
	}
	
	// setup channel 1 to size and MONO because pickup.wav is MONO
    SND_CHANNELS[0] = sceAudioChReserve(PSP_AUDIO_NEXT_CHANNEL, size, PSP_AUDIO_FORMAT_MONO);


    size = 0;
	fp = fopen("wall.raw", "rb");
	if (fp){
		while(fgetc(fp) != EOF)
			size++;
		// align file size to the closest 64 bytes
		size = PSP_AUDIO_SAMPLE_ALIGN(size);
		
		sound_wall = (uint16_t*)malloc(sizeof(uint16_t) * size);
		if (sound_wall){
			memset(sound_wall, 0, sizeof(uint16_t) * size);
			
			rewind(fp);
			// read raw data into buffer
			fread(sound_wall, sizeof(uint16_t), size, fp);
		}
		
		fclose(fp);
	}

    SND_CHANNELS[1] = sceAudioChReserve(PSP_AUDIO_NEXT_CHANNEL, size, PSP_AUDIO_FORMAT_MONO);
    
	

    size = 0;
	fp = fopen("select.raw", "rb");
	if (fp){
		while(fgetc(fp) != EOF)
			size++;
		// align file size to the closest 64 bytes
		size = PSP_AUDIO_SAMPLE_ALIGN(size);
		
		sound_select = (uint16_t*)malloc(sizeof(uint16_t) * size);
		if (sound_select){
			memset(sound_select, 0, sizeof(uint16_t) * size);
			
			rewind(fp);
			// read raw data into buffer
			fread(sound_select, sizeof(uint16_t), size, fp);
		}
		
		fclose(fp);
	}

    SND_CHANNELS[2] = sceAudioChReserve(PSP_AUDIO_NEXT_CHANNEL, size, PSP_AUDIO_FORMAT_MONO);

    size = 0;
	fp = fopen("tada.raw", "rb");
	if (fp){
		while(fgetc(fp) != EOF)
			size++;
		// align file size to the closest 64 bytes
		size = PSP_AUDIO_SAMPLE_ALIGN(size);
		
		sound_tada = (uint16_t*)malloc(sizeof(uint16_t) * size);
		if (sound_tada){
			memset(sound_tada, 0, sizeof(uint16_t) * size);
			
			rewind(fp);
			// read raw data into buffer
			fread(sound_tada, sizeof(uint16_t), size, fp);
		}
		
		fclose(fp);
	}

    SND_CHANNELS[3] = sceAudioChReserve(PSP_AUDIO_NEXT_CHANNEL, size, PSP_AUDIO_FORMAT_MONO);
    


	/*
    intraFontInit();
    intraFontActivate(guiFont);
    */

    

    GFX::init();

    bool isButtonHeld = false;

    while (true)
    {

        if(onMenu)
        {
            drawMenu();
            sceCtrlReadBufferPositive(&ctrlData, 1);
            
            if(ctrlData.Buttons & PSP_CTRL_TRIANGLE)
            {
                sceAudioOutputBlocking(SND_CHANNELS[2], PSP_AUDIO_VOLUME_MAX * 0.5, sound_select);
                difficultyLevel = 0;
                onMenu = false;
                gameOver = false;
                GFX::clear(0x00000000);
                GFX::swapBuffers();
                GFX::clear(0x00000000);
                startGame();
            }
            else if(ctrlData.Buttons & PSP_CTRL_SQUARE)
            {
                sceAudioOutputBlocking(SND_CHANNELS[2], PSP_AUDIO_VOLUME_MAX * 0.5, sound_select);
                difficultyLevel = 1;
                onMenu = false;
                gameOver = false;
                GFX::clear(0x00000000);
                GFX::swapBuffers();
                GFX::clear(0x00000000);
                startGame();
            }
            else if(ctrlData.Buttons & PSP_CTRL_CROSS)
            {
                sceAudioOutputBlocking(SND_CHANNELS[2], PSP_AUDIO_VOLUME_MAX * 0.5, sound_select);
                difficultyLevel = 2;
                onMenu = false;
                gameOver = false;
                GFX::clear(0x00000000);
                GFX::swapBuffers();
                GFX::clear(0x00000000);
                startGame();
            }
            else if(ctrlData.Buttons & PSP_CTRL_SELECT)
            {
                loadCustomLevel();
            }
			
			
			old = ctrlData;
            continue;
        }
        
        
        if(gameOver == false)
        {
            sceCtrlReadBufferPositive(&ctrlData, 1);

            if(ctrlData.Buttons & PSP_CTRL_SQUARE)
            {
                
                isButtonHeld = false;
            }
            if(ctrlData.Buttons & PSP_CTRL_SELECT)
            {
                
                onMenu = true;
                gameOver = true;
            }
            if(ctrlData.Buttons & PSP_CTRL_START)
            {
                
                restartLevel();
            }
            
            if(ctrlData.Buttons & PSP_CTRL_UP)
            {
                if(isButtonHeld == false)
                {
                    move(0, -1);
                }
                isButtonHeld = true;
            }
            else if(ctrlData.Buttons & PSP_CTRL_RIGHT)
            {
                if(isButtonHeld == false)
                {
                    move(1, 0);
                }
                isButtonHeld = true;
            }
            else if(ctrlData.Buttons & PSP_CTRL_DOWN)
            {
                if(isButtonHeld == false)
                {
                    move(0, 1);
                }
                isButtonHeld = true;
            }
            else if(ctrlData.Buttons & PSP_CTRL_LEFT)
            {
                if(isButtonHeld == false)
                {
                    move(-1, 0);
                }
                isButtonHeld = true;
            }
            else
            {
                isButtonHeld = false;
            }

            //text rendering

            drawBoard();                                                                                            
        }
        else
        {

        
            sceCtrlReadBufferPositive(&ctrlData, 1);

            if(ctrlData.Buttons & PSP_CTRL_START)
            {
                sceAudioOutputBlocking(SND_CHANNELS[2], PSP_AUDIO_VOLUME_MAX * 0.5, sound_select);
                onMenu = true;
                gameOver = false;
            }


            GFX::clear(0x00000000);
            GFX::drawImage(0, 0, 480, 272, IMAGE_END);
            GFX::drawImage(162, 142, 78, 13, TEXT_SCORE);
            GFX::drawImage(162, 160, 78, 13, TEXT_STEPS);
            GFX::drawImage(162, 178, 78, 13, TEXT_BONUS);
            //GFX::drawImage(162, 86, 65, 13, TEXT_TIME);
            GFX::drawInt(score, 240, 142);
            GFX::drawInt(steps, 240, 160);
            GFX::drawInt(collectedTreasure, 240, 178);
            GFX::swapBuffers();

        }
    }
    

}