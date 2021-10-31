#include "chessSystem.h"
#include <stdlib.h>
#include "map.h"
#include <string.h>
#include "tournament.h"
#include "player.h"
#define TWO_POINTS 2
#define ONE_POINT 1


static int CountLegalGamesInTournament(Tournament tournament);

struct chess_system_t {
    Map tournaments_map; //key-> tournament_id, element->Tournament
    Map players; //key-> player_id, element->Player

};

static bool checkValidLocation(const char *tournament_location) {
    if (tournament_location == NULL) {
        return false;
    }
    //int size=(int)strlen(tournament_location);
    int size = 0;
    while (tournament_location[size]) {
        size++;

    }
    if (!(tournament_location[0] >= 'A' && tournament_location[0] <= 'Z')) {
        return false;
    }
    for (int i = 1; i < size; i++) {
        if (!(((tournament_location[i] >= 'a') && tournament_location[i] <= 'z') || tournament_location[i] == ' ')) {
            return false;
        }
    }
    return true;
}

ChessSystem chessCreate() {
    ChessSystem chess = (ChessSystem) malloc(sizeof(*chess));
    if (chess == NULL) {
        return NULL;
    }

    chess->tournaments_map = createTournamentMap();
    if (chess->tournaments_map == NULL) {
        mapDestroy(chess->tournaments_map);
        free(chess);
        return NULL;
    }

    chess->players = createPlayerMap();
    if (chess->players == NULL) {
        mapDestroy(chess->tournaments_map);
        free(chess);
        return NULL;
    }
    return chess;
}

void chessDestroy(ChessSystem chess) {
    if (chess == NULL) {
        return;
    }
    mapDestroy(chess->tournaments_map);
    mapDestroy(chess->players);
    free(chess);

}

ChessResult
chessAddTournament(ChessSystem chess, int tournament_id, int max_games_per_player, const char *tournament_location) {
    if (!chess) {
        return CHESS_NULL_ARGUMENT;
    }
    if (tournament_id <= 0) {
        return CHESS_INVALID_ID;
    }
    if (mapContains(chess->tournaments_map, &tournament_id)) {
        return CHESS_TOURNAMENT_ALREADY_EXISTS;
    }
    if (!tournament_location) {
        return CHESS_NULL_ARGUMENT;
    }
    if (checkValidLocation(tournament_location) == false) {
        return CHESS_INVALID_LOCATION;
    }
    if (max_games_per_player <= 0) {
        return CHESS_INVALID_MAX_GAMES;
    }
    Tournament new_tournament = tournamentCreate(tournament_id, max_games_per_player, tournament_location);
    if (new_tournament == NULL) {
        return CHESS_OUT_OF_MEMORY;
    }

    MapResult res = mapPut(chess->tournaments_map, &tournament_id, new_tournament);

    if (res != MAP_SUCCESS) {

        tournamentDestroy(new_tournament);
        return CHESS_OUT_OF_MEMORY;
    }

    tournamentDestroy(new_tournament);
    return CHESS_SUCCESS;

}

ChessResult
chessAddGame(ChessSystem chess, int tournament_id, int first_player, int second_player, Winner winner, int play_time) {

    if (!chess) {

        return CHESS_NULL_ARGUMENT;
    }
    if (tournament_id <= 0 || first_player <= 0 || second_player <= 0 || first_player == second_player) {
        return CHESS_INVALID_ID;
    }
    if (play_time < 0) {
        return CHESS_INVALID_PLAY_TIME;
    }

    Tournament aux_tournament = mapGet(chess->tournaments_map, &tournament_id);


    if (aux_tournament == NULL) {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    //tournamentGetId(aux_tournament);
    if (tournamentEnded(aux_tournament) == true) {
        return CHESS_TOURNAMENT_ENDED;
    }

    GameKey game_key = gameKeyCreate(first_player, second_player);
    if (!game_key) {

        return CHESS_OUT_OF_MEMORY;
    }

    if (mapContains(getGames(aux_tournament), game_key)) {

        free(game_key);
        return CHESS_GAME_ALREADY_EXISTS;
    }

    if (countingGameForPlayer(aux_tournament, first_player) == false || countingGameForPlayer(aux_tournament, second_player) == false) {

        free(game_key);
        return CHESS_EXCEEDED_GAMES;
    }

    //in this line, we have our tournament .
    // now we need to insert the game.
    //game:= player1,player2,winner , time.
    //winner= FIRST_PLAYER,SECONDE_PLAYER,DRAW.
    Player first = NULL, second = NULL;
    //Looking for player one and two
    bool player_one_exist = false, player_two_exist = false;
    MAP_FOREACH(int*, p_it, chess->players) {
        Player player = mapGet(chess->players, p_it);
        if (getPlayerId(player) == first_player) {
            first = player;
            player_one_exist = true;
        }
        free(p_it);
    }
    MAP_FOREACH(int*, p_it, chess->players) {
        Player player = mapGet(chess->players, p_it);
        if (getPlayerId(player) == second_player) {
            second = player;
            player_two_exist = true;
        }
        free(p_it);
    }

    if (player_one_exist == false) {
        //CREATE FIRST PLAYER
        first = createPlayer(first_player);
        if (!first) {
            free(game_key);
            return CHESS_OUT_OF_MEMORY;
        }
        GameKey remove_dummy = gameKeyCreate(-first_player, -first_player);//we're creating "fake" game to make the
        //tournament check if we already removed the player before.
        MAP_FOREACH(int*, tournament_key, chess->tournaments_map) {
            Tournament current_tournament = mapGet(chess->tournaments_map, tournament_key);
            MapResult removed = mapRemove(getGames(current_tournament), remove_dummy);
            if (removed == MAP_SUCCESS) {
                updateRemovedPlayers(current_tournament, -1);
            }
            free(tournament_key);
        }
        //insert the new layer to the map of players.
        free(remove_dummy);
        mapPut(chess->players, &first_player, first);
        playerRemove(first);
    }
    if (player_two_exist == false) {
        //call for create player
        second = createPlayer(second_player);
        if (!second) {
            free(game_key);
            return CHESS_OUT_OF_MEMORY;
        }
        GameKey remove_dummy = gameKeyCreate(-second_player, -second_player);
        if (!remove_dummy) {
            free(game_key);
            free(second);
            return CHESS_OUT_OF_MEMORY;
        }
        MAP_FOREACH(int*, tournament_key, chess->tournaments_map) {
            Tournament current_tournament = mapGet(chess->tournaments_map, tournament_key);
            MapResult removed = mapRemove(getGames(current_tournament), remove_dummy);
            if (removed == MAP_SUCCESS) {

                updateRemovedPlayers(current_tournament, -1);
            }
            free(tournament_key);
        }

        //insert the new player to the map of players.
        mapPut(chess->players, &second_player, second);
        free(remove_dummy);
        playerRemove(second);
    }
    GameData data = gameDataCreate(mapGet(chess->players, &first_player), mapGet(chess->players, &second_player),
                                   winner, play_time);
    if (!data) {
        free(game_key);
        return CHESS_OUT_OF_MEMORY;
    }
    //If they exist, we can create games with them.
    Game game = gameCreate(data, game_key);
    if (game == NULL) {
        free(data);
        free(game_key);
        return CHESS_OUT_OF_MEMORY;
    }
    mapPut(getGames(aux_tournament), getGameKey(game), getData(game));
    gameDestroy(game);
    return CHESS_SUCCESS;
}

ChessResult chessRemoveTournament(ChessSystem chess, int tournament_id) {
    if (tournament_id <= 0) {
        return CHESS_INVALID_ID;
    }
    if (mapContains(chess->tournaments_map, &tournament_id) == false) {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }

    Tournament aux = mapGet(chess->tournaments_map, &tournament_id);
    if (!aux) {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    Map games_map = getGames(aux);
    MAP_FOREACH(GameKey, game_key, games_map) {
        GameData game_to_remove = gameDataCopy(mapGet(getGames(aux), game_key));
        if (!game_to_remove) {
            free(game_key);
            return CHESS_OUT_OF_MEMORY;
        }
        //iterate over all the games , update recors before del.
        updatePointsInGame(game_to_remove);
        free(game_to_remove);
        free(game_key);

    }

    mapRemove(chess->tournaments_map, &tournament_id);
    return CHESS_SUCCESS;
}

ChessResult chessRemovePlayer(ChessSystem chess, int player_id)//add when: when there wasn't before don't when:
{

    if (player_id <= 0) {
        return CHESS_INVALID_ID;
    }
    if (!mapContains(chess->players, &player_id)) {
        return CHESS_PLAYER_NOT_EXIST;
    }
    //for each tournament :

    MAP_FOREACH(int*, current_tournament_key, chess->tournaments_map) {
        Tournament current_tournament = mapGet(chess->tournaments_map, current_tournament_key);

        if (tournamentEnded(current_tournament) == false) // if the tournament not finished
        {

            MAP_FOREACH(GameKey, game_key, getGames(current_tournament)) {
                GameData game_data = gameDataCopy(mapGet(getGames(current_tournament), game_key));
                if (!game_data) {
                    free(game_key);
                    free(current_tournament_key);
                    return CHESS_OUT_OF_MEMORY;
                }
                if (isIdInGame(game_key, player_id)) {
                    GameKey old = gameKeyCopy(game_key);
                    if (!old) {
                        free(game_data);
                        free(game_key);
                        free(current_tournament_key);
                        return CHESS_OUT_OF_MEMORY;
                    }
                    updateGameWhenRemovePlayer(game_data, player_id);
                    updateKey(game_key, player_id);
                    mapRemove(getGames(current_tournament), old);
                    mapPut(getGames(current_tournament), game_key, game_data);
                    free(game_key);
                    game_key = mapGetFirst(getGames(current_tournament));
                    free(old);
                }
                free(game_data);
                free(game_key);
            }

            if (checkIfPlayerAlreadyRemoved(current_tournament, player_id) == true) {
                updateRemovedPlayers(current_tournament, 1);
            }

        }
            //for each guy we want to remove,we need to check for all tournaments that were ended if he was there,
            //if he was there,we add 1 to removedPlayers
            //we have player_id,and we need to check for every ended tournament
        else//tournament already ended.we now count him as "removed"
        {
            MAP_FOREACH(GameKey, game_key, getGames(current_tournament)) {
                //if(mapcontain ) -id,-id -->the way we will mark
                if (isIdInGame(game_key, player_id)) {
                    GameKey fake_key = gameKeyCreate(-player_id, -player_id);
                    if (!fake_key) {

                        free(game_key);
                        free(current_tournament_key);
                        return CHESS_OUT_OF_MEMORY;
                    }
                    if (!mapContains(getGames(current_tournament), fake_key)) {
                        Player dummy = createPlayer(-player_id);
                        if (!dummy) {
                            free(game_key);
                            free(current_tournament_key);
                            return CHESS_OUT_OF_MEMORY;
                        }
                        updateRemovedPlayers(current_tournament, 1);
                        //creating a dummy that would make the system "remember" that we counted him as removed,the game is going to be illegel ,and then we will count the legal games only.

                        GameData fake_data = gameDataCreate(dummy, dummy, FIRST_PLAYER, 0);
                        if (!fake_data) {
                            free(game_key);
                            free(current_tournament_key);
                            free(dummy);
                            return CHESS_OUT_OF_MEMORY;
                        }
                        mapPut(getGames(current_tournament), fake_key, fake_data);
                        {
                            free(fake_key);
                            free(fake_data);
                            free(game_key);
                            free(dummy);
                        }
                        continue;
                    }

                    free(fake_key);
                }
                free(game_key);
            }
        }
        free(current_tournament_key);
    }
    mapRemove(chess->players, &(player_id));
    return CHESS_SUCCESS;
}

ChessResult chessEndTournament(ChessSystem chess, int tournament_id) {
    if (!chess) {
        return CHESS_NULL_ARGUMENT;
    }
    if (tournament_id <= 0) {
        return CHESS_INVALID_ID;
    }
    if (mapContains(chess->tournaments_map, &tournament_id) == false) {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    Tournament aux_tournament = mapGet(chess->tournaments_map, &tournament_id);
    if (tournamentEnded(aux_tournament)) {
        return CHESS_TOURNAMENT_ENDED;
    }
    if ((CountLegalGamesInTournament(aux_tournament)) == 0) {
        return CHESS_NO_GAMES;
    }
    int current_winner_point = 0;
    int current_player_point = 0;//current_player_point
    int current_winner_id = 0; //for keep id
    int current_player_id = 0;//current_player_id
    int current_winner_loss = 0;//winner loss
    int current_player_loss = 0;//current player loss
    int current_winner_wins = 0;//winner_max_wins
    int current_player_wins = 0;//current_player_wins



    MAP_FOREACH(int*, p_it, chess->players) {

        Player player = mapGet(chess->players, p_it);
        current_player_id = getPlayerId(player);

        current_player_point = 0;
        current_player_loss = 0;
        current_player_wins = 0;

        MAP_FOREACH(GameKey, game_key, getGames(aux_tournament)) {
            GameData game_data = mapGet(getGames(aux_tournament), game_key);
            if (isIdInGame(game_key, current_player_id)) {
                // if Id in the game_data, we need to know if he player1 or 2, and if he win/loss.
                //if(getIdInGameKey(gamekey, 1) == id_player || getIdInGameKey(gamekey, 1) == - id_player)
                if (getIdInGameKey(game_key, 1) == current_player_id) // if he player one.
                {
                    if (getWinner(game_data) == FIRST_PLAYER)//todo
                    {
                        current_player_point += 2; //add point
                        current_player_wins += 1;
                    }

                    if (getWinner(game_data) == DRAW) {
                        current_player_point += 1;
                    }
                    if (getWinner(game_data) == SECOND_PLAYER) {
                        current_player_loss += 1;
                    }
                }
                if (getIdInGameKey(game_key, 2) == current_player_id) {
                    // else- he player two. !
                    if (getWinner(game_data) == SECOND_PLAYER) {
                        current_player_point += 2;
                        current_player_wins += 1;
                    }
                    if (getWinner(game_data) == DRAW) {
                        current_player_point += 1;
                    }
                    if (getWinner(game_data) == FIRST_PLAYER) {
                        current_player_loss += 1;
                    }
                }
            }//end of if "idInPlayer"
            //moving to next game_data
            free(game_key);
        }//end-map foreach game

        if (current_winner_id == 0)
        {

            current_winner_id = current_player_id;
            current_winner_wins = current_player_wins;
            current_winner_loss = current_player_loss;
            current_winner_point = current_player_point;

        }
        if (current_player_point > current_winner_point) {
            current_winner_id = current_player_id;
            current_winner_wins = current_player_wins;
            current_winner_loss = current_player_loss;
            current_winner_point = current_player_point;
        } else {
            if (current_player_point == current_winner_point) {
                if (current_player_loss < current_winner_loss) {
                    current_winner_id = current_player_id;
                    current_winner_wins = current_player_wins;
                    current_winner_loss = current_player_loss;
                    current_winner_point = current_player_point;
                } else {
                    if (current_player_loss == current_winner_loss) {
                        if (current_player_wins > current_winner_wins) {
                            current_winner_id = current_player_id;
                            current_winner_wins = current_player_wins;
                            current_winner_loss = current_player_loss;
                            current_winner_point = current_player_point;
                        } else {
                            if (current_player_wins == current_winner_wins) {
                                if (current_player_id < current_winner_id) {
                                    current_winner_id = current_player_id;
                                    current_winner_wins = current_player_wins;
                                    current_winner_loss = current_player_loss;
                                    current_winner_point = current_player_point;

                                }
                            }
                        }

                    }//
                }
            }
        }

        free(p_it);
    }//end of mapforeach player.

    setWinner(mapGet(chess->tournaments_map, &tournament_id), current_winner_id);
    return CHESS_SUCCESS;

}

double chessCalculateAveragePlayTime(ChessSystem chess, int player_id, ChessResult *chess_result) {
    if (chess == NULL) {
        *chess_result = CHESS_NULL_ARGUMENT;
        return 0;
    }
    if (player_id <= 0) {
        *chess_result = CHESS_INVALID_ID;
        return 0;
    }

    if (mapContains(chess->players, &player_id) == false) {
        *chess_result = CHESS_PLAYER_NOT_EXIST;
        return 0;
    }

    Player player = mapGet(chess->players, &player_id);

    double total_games = (double) (getNumberOfGame(player));
    double total_time = (double) (getTotalPlayTime(player));
    *chess_result = CHESS_SUCCESS;
    // double result=total_time/total_games;
    return total_time / total_games;
}

ChessResult chessSavePlayersLevels(ChessSystem chess, FILE *file) {
    if (chess == NULL) {
        return CHESS_NULL_ARGUMENT;
    }
    if (file == NULL) {
        return CHESS_NULL_ARGUMENT;
    }
    typedef struct final_t {

        int id;
        double level;
    } Final;

    double level_calc;
    int total_players = mapGetSize(chess->players);
    Final *final_arr = (Final *) malloc(sizeof(Final) * total_players);
    if (!final_arr) {
        return CHESS_OUT_OF_MEMORY;
    }
    int index = 0;
    MAP_FOREACH(int*, p_it, chess->players) {

        Player player = mapGet(chess->players, p_it);
        int id_player = getPlayerId(player);
        int total_games = getNumberOfGame(player);
        if (total_games == 0) {
            //if the player have zero games. we do not need to print him.
            free(p_it);
            index++;
            continue;
        }
        int scores = 6 * getWins(player) - 10 * getLoss(player) + 2 * getDraw(player);
        level_calc = (double) ((double) scores / (double) total_games);
        Final player_data;
        player_data.id = id_player;
        player_data.level = level_calc;
        //insert to array .
        final_arr[index] = player_data;
        index++;
        free(p_it);
    }
    // now sort the array.
    for (int i = 0; i < total_players; i++) {
        for (int j = i + 1; j < total_players; j++) {

            if (final_arr[j].level == final_arr[i].level && final_arr[j].id < final_arr[i].id) {
                Final tmp = final_arr[i];
                final_arr[i] = final_arr[j];
                final_arr[j] = tmp;
            }
            if (final_arr[j].level > final_arr[i].level) {
                Final tmp = final_arr[j];
                final_arr[j] = final_arr[i];
                final_arr[i] = tmp;
            }

        }
    }
    for (int i = 0; i < total_players; i++) {
        if (fprintf(file, "%d %.2f\n", final_arr[i].id, final_arr[i].level) < 0) {
            free(final_arr);
            return CHESS_SAVE_FAILURE;
        }
        //this line for us. for review
    }
    free(final_arr);
    return CHESS_SUCCESS;
}

ChessResult chessSaveTournamentStatistics(ChessSystem chess, char *path_file) {
    if (chess == NULL) {
        return CHESS_NULL_ARGUMENT;
    }
    FILE *file = fopen(path_file, "w");
    if (file == NULL) {
        return CHESS_NULL_ARGUMENT;
    }
    bool zero_tournament_ended = true; // flage for know if zero tournament ended. if he stay true.
    /*
     * int array of keys.- and insert for each tournament.
     *sort
     * for
     */
    int size = mapGetSize(chess->tournaments_map);
    if (size < 0) {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    int *array_keys = (int *) malloc(sizeof(int) * size);
    if (array_keys == NULL) {

        return CHESS_OUT_OF_MEMORY;
    }

    int i = 0;
    MAP_FOREACH(int*, current_tournament_key, chess->tournaments_map) {
        array_keys[i] = *current_tournament_key;
        i++;
        free(current_tournament_key);
    }
    //we have an array of keys, now we need to sort.
    for (int k = 0; k < size - 1; k++) {
        for (int j = k + 1; j < size; j++) {
            if (array_keys[j] < array_keys[k]) {
                int tmp = array_keys[k];
                array_keys[k] = array_keys[j];
                array_keys[j] = tmp;
            }
        }
    }
    for (int k = 0; k < size; k++) {
        Tournament current_tournament = mapGet(chess->tournaments_map, &array_keys[k]);
        int longest_game = 0;
        int count_total_time = 0;
        //int num_of_games=0;
        int num_of_games = CountLegalGamesInTournament((current_tournament));//change it to CountLegalGames.
        int current_time = 0;
        double average;

        if (tournamentEnded(current_tournament) == true) {
            zero_tournament_ended = false;
            // for each game in our tournament, count statistics.            
            MAP_FOREACH(GameKey, game_key, getGames(current_tournament)) {
                GameData game_data = gameDataCopy(mapGet(getGames(current_tournament), game_key));
                current_time = getTime(game_data);
                count_total_time += current_time;
                if (current_time > longest_game) {
                    longest_game = current_time;
                }
                free(game_data);
                free(game_key);
            }
            average = (double) ((double) count_total_time / (double) num_of_games);
            if (fprintf(file, "%d\n", (int) getWinnerTournament(current_tournament)) < 0)//1
            {
                fclose(file);
                free(array_keys);
                return CHESS_SAVE_FAILURE;
            }

            if (fprintf(file, "%d\n", (int) longest_game) < 0)//2
            {
                fclose(file);
                free(array_keys);
                return CHESS_SAVE_FAILURE;
            }
            if (fprintf(file, "%.2f\n", average) < 0)//3
            {
                fclose(file);
                free(array_keys);
                return CHESS_SAVE_FAILURE;
            }
            if (fprintf(file, "%s\n", getLocation(current_tournament)) < 0)//4
            {
                fclose(file);
                free(array_keys);
                return CHESS_SAVE_FAILURE;
            }

            if (fprintf(file, "%d\n", (int) num_of_games) < 0)//5
            {
                fclose(file);
                free(array_keys);
                return CHESS_SAVE_FAILURE;
            }

            // counting how many diffrent players we have in our Tournament.
            int different_player = 0;
            MAP_FOREACH(int*, p_it, chess->players) {
                MAP_FOREACH(GameKey, game_key, getGames(current_tournament)) {
                    if (isIdInGame(game_key, *p_it)) {

                        different_player += 1;
                        free(game_key);
                        break;
                    }
                    free(game_key);
                }
                free(p_it);
            }

            different_player += getRemovedPlayers(current_tournament);//we include the removed players also
            if (fprintf(file, "%d\n", different_player) < 0)//6
            {
                fclose(file);
                free(array_keys);
                return CHESS_SAVE_FAILURE;
            }
        }
    }
    free(array_keys);
    fclose(file);
    if (zero_tournament_ended == true) {
        return CHESS_NO_TOURNAMENTS_ENDED;
    }

    return CHESS_SUCCESS;
}

static int CountLegalGamesInTournament(Tournament tournament) {
    int size = 0;
    Map games_map = getGames(tournament);
    MAP_FOREACH(GameKey, game_key, games_map) {
        if (!(getIdInGameKey(game_key, 1) == getIdInGameKey(game_key, 2))) {
            size++;
        }
        free(game_key);
    }
    return size;

}
