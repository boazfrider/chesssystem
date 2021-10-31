#include "tournament.h"
#include "game.h"
#include "map.h"
#include <stdlib.h>
#include <string.h>


struct Tournament_t
{
    int tournament_id;
    char* tournament_location;
    int max_games_per_player;
    int winner_id;
    bool ended;
    Map games;
    int playersRemoved;

};

 MapDataElement tournamentCopy(MapDataElement tournamentData){

    if(!tournamentData)
    {
        return NULL;
    }
    Tournament t=(Tournament)tournamentData;
    Tournament new_tournament=tournamentCreate(t->tournament_id,t->max_games_per_player,t->tournament_location);
    new_tournament->playersRemoved=t->playersRemoved;
    return new_tournament;

}

 MapKeyElement tournamentKeyCopy(MapKeyElement tournament_key){


    if(tournament_key == NULL)
    {
        return NULL;
    }

    int *copy=malloc(sizeof(int));
    if(!copy)
    {
        return NULL;
    }

    *copy=*(int*)tournament_key;
    return copy;
}
int compareIntKeys(MapKeyElement key1, MapKeyElement key2)
{
    return *((int*)key2)-*((int*)key1);
}
 void freeTournament(MapDataElement tournament_to_free)
{

    Tournament tournament=(Tournament)tournament_to_free;
    free(tournament->tournament_location);
    mapDestroy(tournament->games);
    free(tournament_to_free);

}

void freeTheKeyTournament(MapKeyElement tournament_to_free_key){

  free(tournament_to_free_key);

}

Map createTournamentMap()
{
    Map tournament=mapCreate(tournamentCopy,tournamentKeyCopy,freeTournament,freeTheKeyTournament,compareIntKeys);
    if(!tournament)
    {
        return NULL;
    }

    return tournament;
}

Tournament tournamentCreate(int id, int max_games_per_player, const char* tournament_location) {
    Tournament new_tournament=(Tournament)malloc(sizeof(*new_tournament));
    if(new_tournament == NULL)
    {
        return NULL;
    }
    new_tournament->tournament_id=id;
    new_tournament->tournament_location = (char*)malloc(strlen(tournament_location) + 1);
    if (new_tournament->tournament_location == NULL) {
        free(new_tournament);
        return NULL;
    }
    strcpy(new_tournament->tournament_location, tournament_location);
    new_tournament->playersRemoved=0;
    new_tournament->winner_id =0;
    new_tournament->ended = false;
    new_tournament->max_games_per_player=max_games_per_player;


    new_tournament->games= createGameMap();
    if(new_tournament->games == NULL)
    {
        tournamentDestroy(new_tournament);
        return NULL;
    }

    return new_tournament;
}

/*
 * tournamentEnded - function that check if tournament ended.
 * @param tournament - our tournament.
 * @return true-the tournament ended, else - return false.
 */
bool tournamentEnded(const Tournament t) {

    return (t->ended);
}

/*
 * getGames-function that return the num of
 *@param tournament
 * @return the map of games.
 */
Map getGames(Tournament tournament)
{
    return tournament->games;
}

/*
 *
 */
void KillMapOfGames(Tournament tournament)
{
    mapDestroy(tournament->games);
}
void tournamentDestroy(Tournament tournament)
{
    free(tournament->tournament_location);
    KillMapOfGames(tournament);
    free(tournament);
}
bool countingGameForPlayer(Tournament tournament, int id)
{
   // printf("the maximum for play is : %d \n ",tournament->max_games_per_player);
    int count=0;
    if(tournament->games==NULL)
    {
        return true;
    }
    MAP_FOREACH(GameKey, gamekey ,tournament->games)
    {
        if (isIdInGame(gamekey, id)) {
            count += 1;
        }
        free(gamekey);
    }
    if(count>=tournament->max_games_per_player)
    {
        return false;
    }

    return true;
}
void setWinner(Tournament tournament, int id)
{
    tournament->winner_id=id;
    tournament->ended=true;
}

char* getLocation(Tournament t)
{
    return t->tournament_location;
}
int getWinnerTournament(Tournament t)
{
    return t->winner_id;
}
int getTournamentId(Tournament t)
{
    return t->tournament_id;
}
 bool checkIfPlayerAlreadyRemoved(Tournament t, int id) {
     MAP_FOREACH(GameKey, gamekey, getGames(t)) {
         if (isIdInGame(gamekey, -id)) {
             free(gamekey);
             return true;
         }
         free(gamekey);
     }
     return false;
 }
 void updateRemovedPlayers(Tournament t, int num)
{
    t->playersRemoved+=num;
}
 int getRemovedPlayers(Tournament t){
    return t->playersRemoved;
}
