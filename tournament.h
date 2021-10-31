#ifndef _TOURNAMENT_H
#define _TOURNAMENT_H
#include "chessSystem.h"

#include "map.h"
#include <stdlib.h>
#include <string.h>

typedef struct Tournament_t *Tournament;
#include "game.h"
#include "player.h"
typedef enum{
    INSERTION_SUCCEEC,
    TOURNAMENT_OPEN,
    TOURNAMENT_CLOSE,
}TournamentResult;

/*
 *tournamentCopy - function used by the map
*/
 MapDataElement tournamentCopy(MapDataElement tournamentData);



//@return the key of the tournament
/*
* tournamentJeyCopy- function that copy the key.
*/
 MapKeyElement tournamentKeyCopy(MapKeyElement tournament_key);

/*
 *freeTournament - function that free the elements of the tournament.
 */
void freeTournament(MapDataElement tournament_to_free);

/*
*freeTheKeyTournament
 * @param tourenament_to_free_key
 * @return void.
 */
//freeKeyElement - Function pointer to be used for removing key elements from the map
void freeTheKeyTournament(MapKeyElement tournament_to_free_key);

/*
 * compareIntKeys - function to compares the keys.
 * @param key1, key2
 * @return 0- if equals
 * else pos/neg number.
 */
int compareIntKeys(MapKeyElement key1, MapKeyElement key2);

/*
 * createTournamentMap - function to create Map of tournament
 *
 */
Map createTournamentMap();

/*
 * tournamentCreate - function that create tournament
 *@param  id -the id of our tournament
 * @param max_games_per_player - the total games that player can play in our tournament.
 * @param tournament_location - the place the tournamenttake place.
 * @return the tournament.
 */
Tournament tournamentCreate(int id, int max_games_per_player, const char* tournament_location);


/*
 * tournamentEnded - function that check if tournament ended.
 * @param t- tournament
 * @return true if ended , false- not ended.
 */
bool tournamentEnded(const Tournament t);

/*
 * getGames- function that return the map of games.
 * @param tournament.
 * @return - map of games.
 */
Map getGames(Tournament tournament);

/*
 * tournamentDestroy- function that destroy our tournament.
 * @param- tournament.
 */
void tournamentDestroy(Tournament tournament);

/*
 * countingGameForplayer - function that count how many games some player play in our tournament and check if he excceded the game.
 * @param tournament , id
 * @return true if the player not Exceeded the max games. else false.
 */
bool countingGameForPlayer(Tournament tournament, int id);

/*
 * setWinner- function that set the winner
 * @param tournament , id
 */
void setWinner(Tournament tournament, int id);

/*
    getLocation - function that return the location of our tournament.
    @param t- our tournament
    @return - location
*/
char* getLocation(Tournament t);

/*
 * getWinnerTournament - return the id of the winner
 * @param t
 * @return the id of the winner
 */
int getWinnerTournament(Tournament t);
bool checkIfPlayerAlreadyRemoved(Tournament t, int id);
void updateRemovedPlayers(Tournament t, int num);
int getRemovedPlayers(Tournament t);
int getTournamentId(Tournament t);
#endif
