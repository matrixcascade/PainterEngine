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


export void GameInitialize(int width,int height)
{
    loadimage("games/tutorial_3/map.png","map");
    createplayer(10,10,16);
    createfood(400,400,1,1,0,0,16,"food");
    setlayer(0);
    drawbackgroundimage(0,0,"map");
    wait(3000);
}


export void GameRun(int elapsed)
{

}