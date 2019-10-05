all: game

run: game
	./game

game: clear
	gcc game.c -lncurses -o game

debug: clear
	gcc -g game.c -lncurses -o game

clear:
	rm -f game
