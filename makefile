CC = gcc
OBJS = player.o game.o tournament.o chessSystem.o
EXEC = chess
DEBUG_FLAG = -g -DNDEBUG
COMP_FLAG = -c -std=c99 -Wall -pedantic-errors -Werror
chess: $(OBJS)
	$(CC) -std=c99  $(OBJS)  ./tests/chessSystemTestsExample.c -L. libmap.a -o chess
chessSystemTestsExample.o: ./tests/chessSystemTestsExample.c ../chessSystem.h ../test_utilities.h 
	$(CC) $(COMP_FLAG) $(DEBUG_FLAG) ./tests/chessSystemTestsExample.c
player.o: player.c player.h map.h
	$(CC) $(COMP_FLAG)  $(DEBUG_FLAG) player.c 
game.o: game.c game.h player.h map.h 
	$(CC) $(COMP_FLAG) $(DEBUG_FLAG) game.c
tournament.o: tournament.c tournament.h game.h player.h map.h
	$(CC) $(COMP_FLAG) $(DEBUG_FLAG) tournament.c
chessSystem.o: chessSystem.c chessSystem.h tournament.h game.h player.h map.h 
	$(CC) $(COMP_FLAG) $(DEBUG_FLAG) chessSystem.c
clean:
	rm -f $(OBJS) $(EXEC)
