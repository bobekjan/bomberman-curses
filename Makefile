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

all: compile doc
compile: bobekja2

GameCanvas.o: src/util.h src/Game.h src/GameCanvas.cpp
	$(CC) $(CFLAGS) -c src/GameCanvas.cpp -o GameCanvas.o

GameController.o: src/util.h src/Game.h src/GameController.h src/GameController.cpp
	$(CC) $(CFLAGS) -c src/GameController.cpp -o GameController.o

GameModel.o: src/util.h src/Game.h src/GameCanvas.h src/GameModel.h src/GameModel.cpp
	$(CC) $(CFLAGS) -c src/GameModel.cpp -o GameModel.o

main.o: src/util.h src/Game.h src/GameCanvas.h src/GameModel.h src/GameModelLoader.h src/GameModelLoader.ipp src/main.cpp
	$(CC) $(CFLAGS) -c src/main.cpp -o main.o

util.o: src/util.h src/Game.h src/util.cpp
	$(CC) $(CFLAGS) -c src/util.cpp -o util.o

bobekja2: GameCanvas.o GameController.o GameModel.o main.o util.o
	$(CC) $(LDFLAGS) GameCanvas.o GameController.o GameModel.o main.o util.o -o bobekja2

run: bobekja2
	./bobekja2

memcheck: bobekja2
	valgrind --tool=memcheck --leak-check=full ./bobekja2

clean:
	rm -rf *.o bobekja2

doc:
	echo "To be implemented"
