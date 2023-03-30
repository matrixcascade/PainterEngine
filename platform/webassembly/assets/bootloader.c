#name "main"
/////////////////////////////////////////////////////////////////
// game lobby bootloader for painterscript
// code by DBinary 20230321
/////////////////////////////////////////////////////////////////
#include "stdlib.h"


int name_run_once=0;
int loadgameinfo(string url)
{
    seturl(url);
    if(!request(url+"/config.json"))
    {
      message("无法获取游戏数据:"+url+"/config.json");
      return 0;
    }
    if(!datatoinfo())
    {
      message("无法获取游戏数据:"+url);
      return 0;
    }

    if(!request(url+"/logo.png"))
    {
      message("无法获取游戏数据:"+url+"/logo.png");
      return 0;
    }
    if(!datatologo())
    {
      message("无法获取游戏数据:"+url);
      return 0;
    }

    return 1;
}

export int loadgameinfos()
{
  message("正在启动游戏大厅...");
  sleep(0);
  if(!request("games/games.json"))
  {
    message("无法连接到游戏大厅服务器");
    return 0;
  }

  if (!datatogamejson())
  {
    message("无法解析游戏大厅服务器返回的数据");
    return 0;
  }
  
  message("正在进入游戏大厅...");
  int game_index=0;

  while(1)
  {
    string url;
    url=geturl("games[+"+string(game_index)+"]");
    game_index++;

    if(url=="")
    break;
    
    if (loadgameinfo(url))
    {
      pushgameinfo();
    }
  }
  
  done();
  if(name_run_once==0)
  {
    message("请输入你的游戏名称");
    name_run_once=1;
  }
  sleep(1000);
  while(1)
  {
    if(typename())
     break;
    message("不合法的名称,重新输入");  
    sleep(2000);
  }
  
  message("");
}

export int loadgame(string url)
{
  if(!loadgameinfo(url))
     return 0;

  message("正在载入游戏: "+url);
  if(request(url+"/code.bin"))
    {
      if(!datatobin())
      {
        message("无法获取游戏数据:"+url+"/code.bin");
        return 0;
      }
    }
    else
    {
      if(request(url+"/code.c"))
      {
        if(!datatocode())
        {
          message("无法获取游戏数据:"+url+"/code.c");
          return 0;
        }
      }
      else
      {
        message("无法获取游戏数据:"+url+"/code.c");
      return 0;
      }
    }
  message("");
  rungame();
  return 1;
}

