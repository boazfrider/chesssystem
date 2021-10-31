#ifndef _GAME_H
#define _GAME_H
#include<stdlib.h>
#include<stdbool.h>
#include<assert.h>
#include <stdio.h>
#include "map.h"
#include "chessSystem.h"
#include "tournament.h"
#include "player.h"



typedef struct game_t *Game;
typedef struct GameKey_t *GameKey;
typedef struct GameData_t *GameData;
/*
 *gameCreate : creating a struct game
 * @param game data , game key
 * @return game.
 */
Game gameCreate(GameData data,GameKey key);

/*
 * gameCopy: function to create a copy of game.
 *@param - game to copy
 * @return - game.
 */
Game gameCopy(Game game_to_copy);

/*
 * updateGameWhenRemovePlayer: function to change the statistics of player that we want to remove
 * if theplayer won the game , we dec his num of wins and add winning to the opposite player
 * and if in the game was a draw. we down the draw points and add and drc the right statistic.
 * @param game_data ,the game that the player is there.
 * @param id_to_remove : the id of player we need to remove
 * @return void.
 *
 */
void updateGameWhenRemovePlayer(GameData game_data, int id_to_remove);

/*
 * GameCopy: function that copy the element of game, this function used by the map.
 * @param game_to_copy
 * @return MapDataElement
 */
MapDataElement GameCopy(MapDataElement game_to_copy);

/*
 * gameDestroy: function to destroy our game
 * @param: game.
 * @return void.
 */
void gameDestroy(Game game);

/*
@getGameKey- function that return the key that associated with the game.
 @param - game
 @return - the key of the game.
 */
GameKey getGameKey(Game game);

/*
 * gameKeyCreate - function to create a new key of game.
 * @param first_player_id - the id of the first player
 * @param second_player_id - the id of the second player
 * @return - a new key og game.
 */
GameKey gameKeyCreate(int first_player_id, int second_player_id);

/*
 * keyDestroy- fonction to destroy our key.
 * @param key.
 */
void keyDestroy(GameKey Key);//destroys key

/*
 * isIdInGame : function to return true if the id in the game
 * @param game- the game we are check
 * @param id- the id we need to search in the game
 * @return true- the id is in game , else - return false
 */
bool isIdInGame(GameKey game, int id);

/*
 * getIdInGameKey - that return the id place one in the key, or place two.
 * @param key- the key.
 * @param num- 1 for first_player_id , 2 for second_player_id.
 * @return the id .
 */
int getIdInGameKey(GameKey key, int num);

/*
 * gameKeyCopy - function that the map use.
 * @param - game_key_to_copy - the relevant key that we need to copy
 * @return a copy of key.
 */
MapKeyElement gameKeyCopy(MapKeyElement game_key_to_copy);

/*
 * updateKey - function to update the key when we want to remove player.
 * @param key-the relevantkey
 * @param - id to remove
 * @return void
 */
void updateKey(GameKey key,int id_to_remove);

/*
 * gameDataCopy: fonction to copy the data element that used by the map.
 * @param data_copy
 * @return a mapDataElement .
 */
MapDataElement gameDataCopy(MapDataElement data_copy);


void printKey(GameKey key);


/*gameDataCreate: function to create a Data of our game
 * @param - the players in our game : player 1 and player 2 , winner , time.
 * @return GameData that include all our parameters.
 */
GameData gameDataCreate(Player player1 ,Player player2 ,int winner,int time);//

/*
 * getPlayer1 , getPlayer2 : return the "player1/2" that play in the game.
 * @param  data of game
 * @return player/
 */
Player getPlayer1(GameData data);//
Player getPlayer2(GameData data);//


/*
 *getTime - function that return the time of the game.
 * @param - data of game.
 * @return - the time that the game played.
 */
int getTime(GameData data);//

/*
 * getWinner - function that return the winner in the game.
 * @param - data of game
 * @return the winner in the game
 */
Winner getWinner(GameData game);//

/*
 *getData- function that return the data of the game.
 * @param- game
 * @return- data of the game.
 */
GameData getData(Game game);

/*
 * createGameMap - function that create a map of games.
 */
Map createGameMap();

/*
 * updatePointsInGame- function that update the scores.
 * @param game.
 * @return void.
 */
void updatePointsInGame(GameData game);


#endif
