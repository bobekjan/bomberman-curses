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

all: compile run

compile: src/Game.h src/GameModel.h src/GameModel.cpp src/util.h
	g++ -ggdb -O0 -ansi -pedantic -Wall -Wextra -Werror -Wno-long-long src/GameModel.cpp -o ./bobekja2

run: compile
	./bobekja2

clean:
	[ ! -e ./bobekja2 ] || rm ./bobekja2

doc:
	echo "To be implemented"
