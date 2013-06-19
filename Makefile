# Makefile k semestralni praci PA2/2013, letni semestr.
# Autor: Jan Bobek
#
# all     vytvoří veškeré výstupy, generované ze zdrojového kódu.
# compile vytvoří binární podobu semestrální práce, výsledek bude
#         uložen do souboru <login>/<login>.
# run     spustí binární podobu semestrální práce.
# clean   smaže všechny vygenerované soubory a uvede adresář <login> do původního stavu.
# doc     vytvoří dokumentaci do adresáře <login>/doc. Dokumentaci musíte
#         vytvořit pomocí programu doxygen, berte to jako výhodu a zkuste
#         dokumentaci uplatnit v předmětu TED. Dokumentaci můžete vygenerovat
#         POUZE v HTML (tedy ne pdf/latex).

CC=g++
CFLAGS=-ggdb -O0 -ansi -pedantic -fno-rtti -Wall -Wextra -Werror -Wno-long-long
LDFLAGS=-lcurses -lmenu

all: doc compile

###########
# Soubory #
###########
util.o: src/Game.h src/util.h src/util.cpp
	$(CC) $(CFLAGS) -c src/util.cpp -o util.o

Socket.o: src/Game.h src/Socket.h src/Socket.cpp
	$(CC) $(CFLAGS) -c src/Socket.cpp -o Socket.o

GameCanvas.o: src/Game.h src/GameCanvas.h src/GameCanvas.cpp
	$(CC) $(CFLAGS) -c src/GameCanvas.cpp -o GameCanvas.o

GameController.o: src/Game.h src/GameController.h src/GameController.cpp
	$(CC) $(CFLAGS) -c src/GameController.cpp -o GameController.o

GameModel.o: src/Game.h src/GameCanvas.h src/GameModel.h src/util.h src/GameModel.cpp
	$(CC) $(CFLAGS) -c src/GameModel.cpp -o GameModel.o

GameLocalModel.o: src/Game.h src/GameCanvas.h src/GameController.h src/GameModel.h src/GameLocalModel.h src/util.h src/GameLocalModel.cpp
	$(CC) $(CFLAGS) -c src/GameLocalModel.cpp -o GameLocalModel.o

GameServerModel.o: src/Game.h src/GameController.h src/GameModel.h src/GameLocalModel.h src/GameServerModel.h src/Socket.h src/util.h src/GameServerModel.cpp
	$(CC) $(CFLAGS) -c src/GameServerModel.cpp -o GameServerModel.o

GameRemoteModel.o: src/Game.h src/GameController.h src/GameModel.h src/Socket.h src/util.h src/GameRemoteModel.cpp
	$(CC) $(CFLAGS) -c src/GameRemoteModel.cpp -o GameRemoteModel.o

GameModelLoader.o: src/Game.h src/GameModel.h src/GameLocalModel.h src/GameServerModel.h src/GameRemoteModel.h src/GameModelLoader.h src/util.h src/GameModelLoader.cpp
	$(CC) $(CFLAGS) -c src/GameModelLoader.cpp -o GameModelLoader.o

main.o: src/Game.h src/GameCanvas.h src/GameController.h src/GameModel.h src/GameLocalModel.h src/GameModelLoader.h src/util.h src/main.cpp
	$(CC) $(CFLAGS) -c src/main.cpp -o main.o

bobekja2: util.o Socket.o GameCanvas.o GameController.o GameModel.o GameLocalModel.o GameServerModel.o GameRemoteModel.o GameModelLoader.o main.o
	$(CC) $(LDFLAGS) util.o Socket.o GameCanvas.o GameController.o GameModel.o GameLocalModel.o GameServerModel.o GameRemoteModel.o GameModelLoader.o main.o -o bobekja2

###################
# Standardni cile #
###################
compile: bobekja2

run: bobekja2
	./bobekja2

clean:
	rm -rf *.o bobekja2 doc/

doc: Doxyfile
	doxygen

##########################
# Nestandardni rozsireni #
##########################
memcheck: bobekja2
	valgrind --tool=memcheck --leak-check=full --show-reachable=yes --log-file=valgrind.log ./bobekja2
