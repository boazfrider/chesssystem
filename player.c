

#include "player.h"
#include "map.h"
#include <stdlib.h>
#include <string.h>
#include "chessSystem.h"
#include "tournament.h"
#include "game.h"

 struct player_t{
    int player_id;
    int num_wins;
    int num_losses;
    int num_draw;
    int total_play_time;
};

Player createPlayer(int player_id)
{
    Player new_player=(Player)malloc(sizeof(*new_player));
    if(!new_player)
    {
        return NULL;
    }
        
    new_player->player_id=player_id;//key
    new_player->num_draw=0;//data
    new_player->num_losses=0;//data
    new_player->num_wins=0;//data
    new_player->total_play_time=0;//data

    return new_player;
}
void updatePoints(Player player,int point)
{
    if(point > 0)
    {
        player->num_wins+=1;
    }
    if(point==0)
    {
        player->num_draw+=1;
    }
    if(point<0)
    {
        player->num_losses+=1;
    }


}
void updatePlayerTime(Player player,int time)
{
    player->total_play_time+=time;
}

 MapDataElement playerCopy(MapDataElement player_to_copy)
{
    if(!player_to_copy) {
        return NULL;
    }
    Player copy_player= (Player)player_to_copy; // casting
    Player player=(Player)malloc(sizeof(struct player_t));
    if(!player)
    {
        return NULL;
    }
    player->num_draw=copy_player->num_draw;
    player->num_losses=copy_player->num_losses;
    player->num_wins=copy_player->num_wins;
    player->player_id=copy_player->player_id;
    player->total_play_time=copy_player->total_play_time;
    return player;
}
static MapKeyElement playerKeyCopy(MapKeyElement player_key)
{   
    Player player=(Player)player_key;
    int *key=(int*)malloc(sizeof(int));
    if(!key)
    {
        return NULL;
    }
    *key=player->player_id;
    return key;
}
static int comparePlayerKey(MapKeyElement key1,MapKeyElement key2)
{
    //if zero - Keys are equals
    //else = diffrent players.
    return *((int*)key2)-*((int*)key1);
}
static void freePlayer(MapDataElement player_to_free)
{
    Player player_free=(Player)player_to_free;
    free(player_free);
}
static void freeKeyPlayer(MapKeyElement key_to_free)
{
    free(key_to_free);
}
Map createPlayerMap()
{
    Map players_map= mapCreate(playerCopy, playerKeyCopy, freePlayer, freeKeyPlayer, comparePlayerKey);
    if(!players_map)
    {
        mapDestroy(players_map);
        return NULL;
    }
    return players_map;
}
int getNumberOfGame(Player player)
{
    int a=player->num_draw;
    int b=player->num_losses;
    int c= player->num_wins;
    return a+b+c;
}
void downOneDraw(Player player)
{
    player->num_draw--;
}
void downOneWin(Player player)
{
    player->num_wins --;
}
void downOneLoss(Player player)
{
    player->num_losses --;
}

int getPlayerId(Player player)
{
    return player->player_id;
}

void setIdRemove(Player player)
{
    player->player_id=-1*player->player_id;
}
void playerRemove(Player player_remove)
{
    if(player_remove == NULL ) {
        return;
    }
    freePlayer(player_remove);
    player_remove=NULL;
}
int getWins(Player player)
{
    return player->num_wins;
}
int getLoss(Player player)
{
    return player->num_losses;
}
int getDraw(Player player)
{
    return player->num_draw;
}
int getTotalPlayTime(Player player)
{
    return player->total_play_time;
}
