all: game

run: game
	./game

game: clean
	gcc game.c -lncurses -o game

debug: clean
	gcc -g game.c -lncurses -o game

clean:
	rm -f game
