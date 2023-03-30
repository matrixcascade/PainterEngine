#name "main"
#include "stdlib.h"

export void GameOnTrigger(string id)
{
   
}

export void GameOnDie()
{
   sleep(3000);
   defeat();
}

export void GameOnFood(string id)
{
    setspeed(0);
    sleep(1000);
    victory();
}

void drawmap(int x,int y)
{
    drawrect(x*20,y*20,20,20,1,1,1,1);
}

#define ROW 40
#define COL 40

export void GameInitialize(int width,int height)
{
    int i,j;
    createplayer(8,8,10);
    createfood(800-8,800-8,1,1,0,0,16,"food");
    setlayer(0);
    clearmap(1,0,0,0);
    rand();
    rand();
    rand(); 
    rand();  
    rand(); 
    rand();
    rand();
    rand();
    rand();
    rand();
    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
           if (rand() % 3 != 0) 
            {
                drawmap(i,j);
            }
        }
    }
    drawmap(0,0);
    drawmap(ROW - 1,COL - 1);
    wait(3000);
}


export void GameRun(int elapsed)
{

}