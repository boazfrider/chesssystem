


#include "map.h"
#include <stdlib.h>
#include <string.h>
#include "tournament.h"
#include "player.h"
#include "game.h"



struct game_t{
    GameKey key;
  GameData data;
};
struct GameData_t {
    Player player_one;
    Player player_two;
    Winner winner;
    int play_time;
};
struct GameKey_t {
    int first_player_id;
    int second_player_id;
} ;
GameData gameDataCreate(Player player1 ,Player player2 ,int winner,int time)
{
    if(player1== NULL || player2==NULL || winner < 0 ||time < 0 )
    {
        return NULL;
    }
    GameData data=(GameData)malloc(sizeof(struct GameData_t));
    if(!data) {
        return NULL;
    }
    data->player_one=player1;
    data->player_two=player2;
    data->winner=winner;
    data->play_time=time;
    return data;
}

Game gameCreate(GameData new_data,GameKey key)//replacing
{
    Game new_game=(Game)malloc(sizeof(struct GameData_t)+sizeof(struct GameKey_t));
    if (new_game == NULL)
    {
        return NULL;
    }
    new_game->key=key;
    new_game->data=new_data;
//    new_game->data->player_one = new_data->player_one ;
//    new_game->data->player_two =new_data-> player_two ;
//    new_game->data->winner =new_data-> winner ;

    updatePlayerTime(new_data->player_one,new_data->play_time);
    updatePlayerTime(new_data->player_two,new_data->play_time);
    if(new_data->winner==FIRST_PLAYER)
    {
        updatePoints(new_data->player_one,1);
        updatePoints(new_data->player_two,-1);
    }
    if(new_data->winner==SECOND_PLAYER)
    {
        updatePoints(new_data->player_one,-1);
        updatePoints(new_data->player_two,1);
    }
    if(new_data->winner==DRAW)
    {
        updatePoints(new_data->player_one,0);
        updatePoints(new_data->player_two,0);
    }

    new_game->data->play_time = new_data->play_time ;

    return new_game;
}
void updatePointsInGame(GameData game)
{
    //and dec total play time.

    updatePlayerTime(game->player_one, (game->play_time) * -1);
    updatePlayerTime(game->player_two, (game->play_time) * -1);//add the negative time.

    if(game->winner == FIRST_PLAYER)
    {
        downOneWin(game->player_one);
        downOneLoss(game->player_two);
    }
    if(game->winner == SECOND_PLAYER)
    {
        downOneWin(game->player_two);
        downOneLoss(game->player_one);
    }
    if(game->winner == DRAW)
    {
        downOneDraw(game->player_one);
        downOneDraw(game->player_two);
    }
}
GameKey gameKeyCreate(int first_player_id, int second_player_id) { //gamekey constructor
    GameKey key = (GameKey)malloc(sizeof(*key));
    if (!key) {
        return NULL;
    }
    key->first_player_id = first_player_id;
    key->second_player_id = second_player_id;
    return key;
}

    Game gameCopy(Game game_to_copy)//  MAPCOPY
{
    Game game=(Game)game_to_copy;
    GameKey key=gameKeyCreate(game->key->first_player_id,game->key->second_player_id);
    if(!key){
        return NULL;
    }
    GameData data= gameDataCreate(game->data->player_one,game->data->player_two,game->data->winner,game->data->play_time);
    if(!data) {
        free(key);
        return NULL;
    }
    Game new_game= gameCreate(data,key);
    if(!new_game)
    {
        free(key);
        free(data);
         return NULL;
    }
    return new_game;
}
  MapKeyElement gameKeyCopy(MapKeyElement game_key_to_copy)
{
    GameKey game_key_copy=(GameKey)game_key_to_copy;
    GameKey new_key= gameKeyCreate(game_key_copy->first_player_id,game_key_copy->second_player_id);
    if(!new_key)
    {
        return NULL;
    }

    return new_key;
}
static int compareGameKeys(MapKeyElement key1,MapKeyElement key2)
{
   // printf("in compatr keys: \n");
  //  printf("addres before: %p , %p\n",key1,key2);
    GameKey GameKey1 = (GameKey)key1;
    GameKey GameKey2 = (GameKey)key2;
    // printf("addres after: %p , %p\n",GameKey1,GameKey2);
   //  printf("keys: %d,%d,  %d,%d\n",GameKey1->first_player_id,GameKey2->second_player_id,GameKey1->second_player_id,GameKey2->first_player_id);

    if((GameKey1->first_player_id == GameKey2->first_player_id && GameKey1->second_player_id ==GameKey2->second_player_id)|| (GameKey1->first_player_id == GameKey2->second_player_id && GameKey1->second_player_id == GameKey2->first_player_id))
    {
        return 0; //might be the problem.
    }
    return 1;
}

static void freeGameElement(MapDataElement game_to_free)//free game data
{
        free(game_to_free);
}
static void freeKeyGame(MapKeyElement game_to_free_key)//FREE'S GAMEKEY
{
    free(game_to_free_key);
}


bool isIdInGame(GameKey key, int id)
{
    //return (((key->first_player_id)==id ||(key->second_player_id)==id||(key->second_player_id)==-id)||(key->first_player_id==-id));
    //printf("is id in game ? \n the id in game are :first- %d , second- %d\n our ID is : %d \n",key->first_player_id,key->second_player_id,id);
    return ((((key->first_player_id)==id) ||((key->second_player_id)==id)));
    //return(getPlayerId(game->player_one)==id ||getPlayerId(game->player_two)==id);
}

void updateGameWhenRemovePlayer(GameData game_data, int id_to_remove)
{
   //if(getPlayerId(game->player_one)==id_to_remove)
    if(getPlayerId(game_data->player_one) == id_to_remove)
    {

        if(game_data->winner == FIRST_PLAYER)
        {
            downOneWin(game_data->player_one); // cancle his win.
            updatePoints(game_data->player_two, 1);// add win.
            updatePoints(game_data->player_one, -1);
            downOneLoss(game_data->player_two);
        }
        if(game_data->winner == DRAW)
        {
            downOneDraw(game_data->player_one);
            downOneDraw(game_data->player_two);
            updatePoints(game_data->player_two, 1);
            updatePoints(game_data->player_one, -1);
        }
        game_data->winner=SECOND_PLAYER;
        //updateKey(game_data,id_to_remove);
//        printf("@@@ %d @@@\n",game->key->first_player_id);
    }
    // our player to remove is player 2.
    else
    {
        if(game_data->winner == SECOND_PLAYER)
        {
            downOneWin(game_data->player_two);
            updatePoints(game_data->player_one, 1);
            updatePoints(game_data->player_two, -1);
            downOneLoss(game_data->player_one);
        }
        if(game_data->winner == DRAW)
        {
            downOneDraw(game_data->player_one);
            downOneDraw(game_data->player_two);
            updatePoints(game_data->player_one, 1);
            updatePoints(game_data->player_two, -1);
        }
        game_data->winner=FIRST_PLAYER;
    }
}
void updateKey(GameKey key,int id_to_remove)
{

    if(key->first_player_id==id_to_remove)
        {
            key->first_player_id=-id_to_remove;
        }
    else {
        key->second_player_id = -id_to_remove;
    }
}
GameKey getGameKey(Game game)
{
    return game->key;
}
int getIdInGameKey(GameKey key, int num)
{
    if (num==1) {
        return key->first_player_id;
    }
    return key->second_player_id;
}
void printKey(GameKey key)
{
    printf("id 1: %d \n id 2: %d\n",key->first_player_id,key->second_player_id);

}
Winner getWinner(GameData game)
{
    return game->winner;
}
void keyDestroy(GameKey key)
{
    free(key);
}
void gameDestroy(Game game)
{
    keyDestroy(getGameKey(game));
    free(game->data);
    free(game);

}

 MapDataElement gameDataCopy(MapDataElement data_copy) {
    if(!data_copy) {
        return NULL;
    }
    GameData data_to_copy=(GameData)data_copy;
    GameData copy= gameDataCreate(data_to_copy->player_one,data_to_copy->player_two, data_to_copy->winner, data_to_copy->play_time);
    if(!copy) {
        return NULL;
    }
    return copy;
}

void gameDataDestroy(GameData data) {
    free(data);
}

Player getPlayer1(GameData data) {
    return data->player_one;
}

Player getPlayer2(GameData data) {
    return data->player_two;
}

int getTime(GameData data) {
    return data->play_time;
}

Map createGameMap()
{
    Map games= mapCreate(gameDataCopy, gameKeyCopy, freeGameElement, freeKeyGame, compareGameKeys);
    if(!games)
    {
        mapDestroy(games);
        return NULL;
    }
    return games;
}

GameData getData(Game game) {
    return game->data;
}
