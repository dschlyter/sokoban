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
	public:
		State(const Coordinate playerPosition, const vector<Coordinate> boxPositions);
		State(const Coordinate playerPosition, const vector<Coordinate> boxPositions, const string stateHistory);
		Coordinate getPlayerPosition() const;
		vector<Coordinate> getBoxes() const;
        string getHistory() const;
        int getCost() const;
        bool isBox(const Coordinate) const;
};

#endif

