#name "main"
#include "stdlib.h"

int ate = 0;

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
   ate++;
   if (ate==4)
   {
      setspeed(0);
      sleep(1000);
      victory();
   }
}

export void thread1()
{
   float angle1;
   float angle2;
   float angle3;
   float angle4;
   float angle5;
   while (1)
   {
      sleep(150);
      setlayer(1);
      clearmap(0,1,1,1);
      drawcircle(400,400,400,16,1,0,0,0);
      drawcircle(400,400,300,16,1,0,0,0);
      drawcircle(400,400,200,16,1,0,0,0);
      drawcircle(400,400,100,16,1,0,0,0);
      drawcircle(400,400,50,16,1,0,0,0);

      angle1=angle1+3;
      angle2=angle2+6;
      angle3=angle3+9;
      angle4=angle4+12;
      angle5=angle5+15;

      drawsolidcircle(400+cos(angle1/180.0*3.14159)*400,400+sin(angle1/180.0*3.14159)*400,32,1,1,1,1);
      drawsolidcircle(400+cos(angle2/180.0*3.14159)*300,400+sin(angle2/180.0*3.14159)*300,32,1,1,1,1);
      drawsolidcircle(400+cos(angle3/180.0*3.14159)*200,400+sin(angle3/180.0*3.14159)*200,32,1,1,1,1);
      drawsolidcircle(400+cos(angle4/180.0*3.14159)*100,400+sin(angle4/180.0*3.14159)*100,32,1,1,1,1);
      drawsolidcircle(400+cos(angle5/180.0*3.14159)*50,400+sin(angle5/180.0*3.14159)*50,32,1,1,1,1);
   }
}


export void GameInitialize(int width,int height)
{
    createplayer(400,400,16);
    createfood(16,16,1,1,0,0,16,"food");
    createfood(800-16,16,1,1,0,0,16,"food");
    createfood(16,800-16,1,1,0,0,16,"food");
    createfood(800-16,800-16,1,1,0,0,16,"food");
    createthread("thread1",1);
    wait(3000);
}


export void GameRun(int elapsed)
{

}