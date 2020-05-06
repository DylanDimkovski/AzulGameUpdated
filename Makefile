.default : all

all: azul

clean:
	@echo "cleaning up..."
	rm *.o main.bin

azul: Azul.o Factory.o GameEngine.o Line.o Menu.o Mosaic.o Node.o Player.o TileList.o
	g++ -Wall -Werror -std=c++14 -O -g -o main.bin $^
	
%.o: %.cpp
	g++ -Wall -Werror -std=c++14 -O -g -c $^