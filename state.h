#ifndef STATE
#define STATE
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>

#define U64 unsigned long long int

using namespace std;

typedef pair<int, int> Coordinate;

class State {
	private:
		Coordinate playerPos;
		vector<Coordinate> boxes;
        int pushes;
        Coordinate moveLoc;
        int moveType;
        U64 hash;
        
        static int map_width;
        static int secondStart;
        static U64 * zobrist;
        static U64 rand64();
        
        void zobristHash();
	public:
		State(){}

        static void initZobristHash(int width, int height);
        static void disposeZobristHash();
	
		State(const Coordinate playerPosition, const vector<Coordinate> boxPositions, const Coordinate loc);
		State(const Coordinate playerPosition, const vector<Coordinate> boxPositions, const int pushCount, const Coordinate loc, const int move);
		Coordinate getPlayerPosition() const;
		vector<Coordinate> getBoxes() const;
        int getCost() const;
        int getMoveType() const;
        Coordinate getMoveLoc() const;
        U64 getHash() const;
        bool isBox(const Coordinate) const;
};

#endif

