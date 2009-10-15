#ifndef STATE
#define STATE
#include <vector>
#include <string>

using namespace std;

typedef pair<int, int> Coordinate;

class State {
	private:
		Coordinate playerPos;
		vector<Coordinate> boxes;
	    string history;	
        int pushes;
	public:
		State(){}
	
		State(const Coordinate playerPosition, const vector<Coordinate> boxPositions);
		State(const Coordinate playerPosition, const vector<Coordinate> boxPositions, const int pushCount);
		Coordinate getPlayerPosition() const;
		vector<Coordinate> getBoxes() const;
        int getCost() const;
        bool isBox(const Coordinate) const;
};

#endif

