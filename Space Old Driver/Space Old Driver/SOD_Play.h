#ifndef SOD_PLAY_H
#define SOD_PLAY_H

#include "SOD_Runtime.h"

#define SOD_WORLD_WIDTH 1200 //世界宽度
#define SOD_WORLD_HEIGHT 900 //世界高度
#define SOD_WORLD_UPDATE_ATOM 10 //武器升级幅度

#define SOD_USER_BULLET_DAMAGE 5 //外星飞船炮弹伤害
#define SOD_ALIEN_BULLET_DAMAGE 5 //外星飞船炮弹伤害
#define SOD_STONE_IMPACT_DAMAGE 15//石头碰撞伤害
#define SOD_GHOST_IMPACT_DAMAGE 15//鬼魂碰撞伤害
#define SOD_ALIEN_IMPACT_DAMAGE 5//外星飞船碰撞伤害

#define SOD_SHIP_DEFAULT_FIRE_DURATION 350//初始武器射速
#define SOD_MIN_FIRE_DURATION 100 //最小武器射速
#define SOD_STONE_GEN_SPACE 300 //生成间隔,请勿修改
#define SOD_STONE_DEFALUE_MIN_ROTATION_SPEED 10 //石头最小旋转速度
#define SOD_STONE_DEFALUE_MAX_ROTATION_SPEED 360 //石头最大旋转速度
#define SOD_GHOST_SEARCH_DISTANCE 200 //鬼魂搜索半径
#define SOD_ALIEN_SEARCH_DISTANCE 280 //外星人搜索半径
#define SOD_USER_SEARCH_DISTANCE 400 //user搜索半径
#define SOD_GHOST_SPEED 100//鬼魂速度
#define SOD_ALIEN_SPEED 150//外星人速度
#define SOD_STAR_SPEED  50 //星星速度
#define SOD_SHIP_FORCE_MAX_DISTANCE 200//飞船拉动距离
#define SOD_SHIP_DEFAULT_MAX_LIFE  100 //初始最大生命
#define SOD_SHIP_DEFAULT_MAX_ATOM  100 //初始最大子弹数量
#define SOD_SHIP_DEFAULT_MAX_FORCE 100 //初始最大拉力
#define SOD_SHIP_DEFAULT_MAX_UPDATE_FORCE 300//最大升级拉力
#define SOD_SHIP_DEFAULT_MAX_SPEED 300 //最大速度

#define SOD_STAR_BUFFER_ATOM    20 //星星补充弹药数量
#define SOD_STAR_BUFFER_HEALTH  20 //星星恢复生命数量
#define SOD_STAR_BUFFER_FORCE   10 //星星升级拉力
#define SOD_STAR_BUFFER_WEAPON  10 //星星武器速度升级数量

#define SOD_DROP_PERCENT 60//物品掉率
#define SOD_DEFAULT_STONE_GEN_DURATION 1000 //开局陨石生成时间
#define SOD_MIN_STONE_GEN_DURATION 500 //最小陨石生成时间


#define SOD_GHOST_GEN_SCORE  500//鬼魂开始出现的分数
#define SOD_DEFAULT_GHOST_GEN_DURATION 3000 //开局鬼魂生成时间
#define SOD_MIN_GHOST_GEN_DURATION 2000 //最小鬼魂生成时间

#define SOD_ALIEN_GEN_SCORE  1500//外星人开始出现的分数
#define SOD_DEFAULT_ALIEN_GEN_DURATION 4000 //开局外星人生成时间
#define SOD_MIN_ALIEN_GEN_DURATION 2500 //最小外星人生成时间

#define SOD_ALIEN_ATOM_ALIVE_TIME 8000 //外星人炮弹生存时间
#define SOD_ALIEN_ATOM_SPEED 80 //外星人炮弹最大速度
#define SOD_USER_ATOM_SPEED 180 //用户炮弹最大速度
#define SOD_DEFAULT_ATOM_SPEED 600 //默认子弹速度
#define SOD_FIRE_PARTICAL_ALIVE_TIME 300 //粒子生存时间 请勿修改
#define SOD_FIRE_SHIP_ATOM_ALIVE_TIME 2000 //子弹最大生存时间

#define SOD_PARTICAL_MP_FIRE 1024*1024 //1M 内存池空间给火焰粒子 请勿修改
#define SOD_PARTICAL_MP_EXPLODE 1024*1024 //1M 内存池空间给爆炸粒子 请勿修改

#define SOD_USEROBJECT_SPEED 60
#define SOD_USER_ATOM_ALIVE_TIME 8000 //user炮弹生存时间
//IMPACT
#define SOD_IMPACTTEST_OBJECTTYPE_USER			1
#define SOD_IMPACTTEST_OBJECTTYPE_ENEMY			2
#define SOD_IMPACTTEST_OBJECTTYPE_NEUTRALITY	8

//Layer
#define SOD_ATOM_Z 3
#define SOD_STAR_Z 6
#define SOD_PARTICAL_FIRE_Z 9
#define SOD_STONE_Z 20
#define SOD_GHOST_Z 18
#define SOD_ALIEN_Z 17
#define SOD_SHIP_Z 10
#define SOD_EXPLOSION_Z 6

//EVENTS
#define SOD_OBJECT_EVENT_DAMAGE 0x1001
#define SOD_OBJECT_EVENT_ATOM 0x1002
#define SOD_OBJECT_EVENT_HEALTH 0x1003
#define SOD_OBJECT_EVENT_ADDFORCE 0x1004
#define SOD_OBJECT_EVENT_ADDWEAPON 0x1005
#define SOD_OBJECT_EVENT_SHIELD 0x1006

typedef enum
{
	SOD_OBJECT_TYPE_SHIP,
	SOD_OBJECT_TYPE_SHIP_ATOM,
	SOD_OBJECT_TYPE_PARTICAL,
	SOD_OBJECT_TYPE_STONE,
	SOD_OBJECT_TYPE_GHOST,
	SOD_OBJECT_TYPE_ALIEN,
	SOD_OBJECT_TYPE_USEROBJECT,
	SOD_OBJECT_TYPE_ALIEN_ATOM,
	SOD_OBJECT_TYPE_USER_ATOM,
	SOD_OBJECT_TYPE_STAR,
}SOD_OBJECT_TYPE;

//用于描述飞船基本物理信息的结构体
typedef struct
{
	px_int life;//生命
	px_texture *shipObject;//飞船对象
	px_texture *shieldtex;
	px_point velocity;//当前速度
	px_point force;//当前受力
	px_point direction;//当前方向

	px_int atomcount;//当前子弹数量
	px_int max_atomcount;//最大子弹数量
	px_int max_life;//最大生命
	px_float max_force;//最大推动力
	px_float max_speed;//最大速度
	px_bool showHelpLine;//辅助线

	px_dword fire_duration;//发射子弹间隔
	px_dword fire_elpased;//距离上次发射子弹经历时间
	px_bool shield;
	px_char *Buffer_Text;
	px_dword Buffer_Text_elpased;

	px_point power_main_point,power_assist_L_point,power_assist_R_point;//主喷射器 辅助喷射器锚点
	px_point cannon_point1,cannon_point2;//炮塔锚点
}SOD_Play_Object_Ship;


//用于描述飞船开火时粒子系统,*需要快速的内存池释放分配,建议建立独立的内存池系统
typedef struct
{
	/*特殊参数*/
	px_dword alive;//粒子存在时间
	px_float range;//粒子发射时范围

	PX_Partical_Launcher launcher;
}SOD_Play_Object_ParitcalLauncher;


//用于描述飞船发射的子弹
typedef struct
{
	px_dword alive;//子弹存在时间
	px_dword show_impact_region;
	px_texture *tex;
	px_point velocity;//当前速度
}SOD_Play_Object_ShipFireAtom;

//用于描述外星飞船发射的子弹
typedef struct
{
	px_dword alive;//子弹存在时间
	px_animation af_animation;
	px_point velocity;//当前速度
}SOD_Play_Object_AlienFireAtom;


//用于描述陨石
typedef struct
{
	px_int life; //陨石生命
	px_float scale;//陨石缩放大小
	px_point velocity;//速度
	px_float rotation;//旋转角度
	px_float rotationSpeed;//旋转角度
	px_texture *pTexture;//纹理
	px_bool show_impact_region;
	px_dword beAttackElpased;//上一次被攻击时间
}SOD_Play_Object_Stone;


//用于描述鬼魂
typedef enum
{
	SOD_PLAY_GHOST_STATUS_NORMAL,
	SOD_PLAY_GHOST_STATUS_ANGRY,
}SOD_PLAY_GHOST_STATUS;

typedef struct
{
	px_int life;
	px_point velocity;//速度
	px_animation alien_animation;//动画集
	px_dword beAttackElpased;//上一次被攻击时间
	SOD_PLAY_GHOST_STATUS status;
}SOD_Play_Object_Ghost;


//用于描述外星人
typedef enum
{
	SOD_PLAY_ALIEN_STATUS_NORMAL,
	SOD_PLAY_ALIEN_STATUS_ANGRY,
	SOD_PLAY_ALIEN_STATUS_VERYANGRY,
}SOD_PLAY_ALIEN_STATUS;

typedef struct
{
	px_int life;
	px_int atom_count;
	px_dword atom_elpased;
	px_point velocity;//速度
	px_animation alien_animation;//动画集
	px_dword beAttackElpased;//上一次被攻击时间
	SOD_PLAY_ALIEN_STATUS status;
}SOD_Play_Object_Alien;



//用户自建
typedef struct
{
	px_int life;
	px_dword atom_elpased;
	px_point velocity;//速度
	px_animation user_animation;//动画集
	px_dword beAttackElpased;//上一次被攻击时间
}SOD_Play_Object_UserObject;

//用户自建发射物
typedef struct
{
	px_float rotation;
	px_float rotationSpeed;
	px_texture *tex;
	px_dword alive;//子弹存在时间
	px_point velocity;//当前速度
}SOD_Play_Object_UserBullet;

//用于星星
typedef enum
{
	SOD_PLAY_STAR_TYPE_HEAL = 0,
	SOD_PLAY_STAR_TYPE_ATOM,
	SOD_PLAY_STAR_TYPE_FORCE,
	SOD_PLAY_STAR_TYPE_WEAPON,
	SOD_PLAY_STAR_TYPE_SHIELD,
}SOD_PLAY_STAR_TYPE;

typedef struct
{
	SOD_PLAY_STAR_TYPE type;
	PX_TEXTURERENDER_BLEND blend;
	px_point velocity;//速度
	px_animation star_animation;//动画集
}SOD_Play_Object_Star;


//用于描述鼠标指针
typedef struct
{
	px_texture tex;
	px_float rotation;
}SOD_Play_Object_Cursor;


//用于记录IO控制输入
typedef struct
{
	px_int mouse_x,mouse_y;
	px_char mouse_ldown;
	px_char mouse_rdown;
}SOD_Play_Control;


typedef struct
{
	SOD_Play_Control control;
	px_memorypool fire_mp;//用于开火粒子系统的内存池
	px_memorypool explode_mp;//用于爆炸粒子系统的内存池

	px_int timeProcess;
	px_int showScore;
	px_int bossscore;
	px_int score;
	px_dword stone_genElpased;
	px_dword alien_genElpased;
	px_dword ghost_genElpased;
	px_int fps;
	px_dword gameover_waiting;
	px_dword fps_frame;
	px_dword fps_elpased;
	px_dword gametime;
	px_dword updatetime_reserved;
	SOD_Runtime *runtime;
	PX_World world;
	SOD_Play_Object_Cursor cursor;
	PX_Object *root;
	PX_Object *shipObject;
	PX_Object *ship_powerpartical_main,*ship_powerpartical_l1,*ship_powerpartical_l2;
}SOD_Play;



SOD_Play_Object_AlienFireAtom *SOD_Object_GetAlienFireAtom(PX_Object *pObject);
SOD_Play_Object_Alien *SOD_Object_GetAlien(PX_Object *pObject);
SOD_Play_Object_Ghost *SOD_Object_GetGhost(PX_Object *pObject);
SOD_Play_Object_Stone *SOD_Object_GetStone(PX_Object *pObject);
SOD_Play_Object_ShipFireAtom *SOD_Object_GetShipFireAtom(PX_Object *pObject);
SOD_Play_Object_ParitcalLauncher *SOD_Object_GetParticalLauncher(PX_Object *pObject);
SOD_Play_Object_Ship *SOD_Object_GetShip(PX_Object *pObject);


px_bool SOD_PlayInitialize(SOD_Runtime *runtime,SOD_Play *pPlay);
px_void SOD_PlayRenderUI(SOD_Play *pPlay);
px_void SOD_PlayReset(SOD_Play *pPlay);
px_void SOD_PlayUpdate(SOD_Play *pPlay,px_dword elpased);
px_void SOD_PlayRender(SOD_Play *pPlay,px_dword elpased);
px_void SOD_PlayPostEvent(SOD_Play *pPlay,PX_Object_Event e);

PX_Object* SOD_Object_ParticalLauncherCreate(SOD_Play *play,px_memorypool *mp,PX_ParticalLauncher_InitializeInfo init_info,px_dword alive);

#endif