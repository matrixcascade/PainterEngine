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
   runnextgame("games/tutorial_2");
}


export void GameInitialize(int width,int height)
{
    createplayer(100,400,16);
    createfood(700,400,1,1,0,0,16,"food");
    wait(3000);
}


export void GameRun(int elapsed)
{

}