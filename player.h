#ifndef _PLAYER_H
#define _PLAYER_H
#include "map.h"
#include "chessSystem.h"

#include <stdlib.h>
#include <string.h>


typedef struct player_t *Player;


//function for create a newPlayer
//@param Player id
//@return - new player.
Player createPlayer(int player_id);

/*
updatePoints - fonction for updating the points of player
@param the player and point.
if point biggger then zero , then we have
if point is zero - its a draw
point less then zero - its a loss !
@return void , only update player statistic.
 */
void updatePoints(Player player,int point);

/*
    playerCopy- function for create a copy of the player
    @param player_to_copy
    @return MapDataElement.
*/
 MapDataElement playerCopy(MapDataElement player_to_copy);

//function for copy key
// @param playerkey
////static MapKeyElement playerKeyCopy(MapKeyElement playerkey);


/*
*function for creating a map of Playera.
@return NULL- if failed.
*/
Map createPlayerMap();


// function that decrease one for players.
//we use this functions when we delete players / tournament
void downOneDraw(Player player);
void downOneWin(Player player);
void downOneLoss(Player player);

/*
 * updatePlayerTime- function that update the total time that the player play.
 * @param player
 * @param time.
 * @return void.
 */
void updatePlayerTime(Player player,int time);
/*
 * getPlayerId- function that return the id of the player.
 * @param player
 * @return id of player
 */
int getPlayerId(Player player);


void setIdRemove(Player player);

/*
 * getNumberOfGames- function that return the total numbers of games that the player played.
 * @param - player
 * @return the total numbers of games.
 */
int getNumberOfGame(Player player);

/*
 *playerRemove-function that remove our player and free the element.
 * @param player to remove.
 */
void playerRemove(Player player_remove);

/*
 * getWins-function that return the num of wins of player.
 * @param player
 * @return num of wins.
 */
int getWins(Player player);

/*
 * getLoss- function that return the num of loss of player
 * @param player
 * @return num of loss
 */
int getLoss(Player player);

/*
 * getDraw - function that return the num of draws of player
 * @param player
 * @return num of draws.
 */
int getDraw(Player player);

/*
 * getTotalPlayTime - function that return the total play time of player
 * @param player
 * @return total game time.
 */
int getTotalPlayTime(Player player);

#endif
