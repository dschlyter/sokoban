#include "map.h"
#include "state.h"
#include <cstring>

Map::Map(const string map){
	// Calc size of map
	map_width = map.find('\n'); 
	map_height=0;

	for(size_t i=0; (i=map.find('\n', i+1))!=string::npos ;++map_height){} 
	

	static_map = new char[map_height*map_width];
	/*for (size_t i = 0; i < map_height; ++i) {
		static_map[i] = new char[map_width];
	}*/

	// Allocation
	/*static_map = new char*[map_height];
	static_map[0] = new char[map_height*map_width];
	for (size_t i = 1; i < map_height; ++i)
		static_map[i] = static_map[i-1] + map_width;
	*/
	// Parse
	size_t row = 0;
	size_t col = 0;
	for(size_t i = 0; i < map.length(); i++){
		switch( map[i] ){
			case EMPTY_CHAR:
				static_map[row*map_width+col] = EMPTY;
				break;
			case WALL_CHAR:
				static_map[row*map_width+col] = WALL;
				break;
			case GOAL_CHAR:
				static_map[row*map_width+col] = GOAL;
				goals.push_back(Coordinate(col, row));
				break;
			case PLAYER_START_CHAR:
				static_map[row*map_width+col] = EMPTY;
                playersStart = Coordinate(col,row);
				break;
			case BOX_CHAR:
				static_map[row*map_width+col] = EMPTY;
				boxesStart.push_back(Coordinate(col, row));
				break;
			case '\n':
				++row;
				col = -1;
				break;
		}
		++col;
	}
}
Map::~Map(){
	delete[] static_map;
}
Map::Map(const Map & other){
	map_width = other.map_width;
	map_height = other.map_height;
    playersStart = other.playersStart;
    boxesStart = other.boxesStart;
    goals = other.goals;

	static_map = new char[map_height*map_width];
    memcpy(static_map, other.static_map, map_height*map_width*sizeof(char));
}
Map & Map::operator=(const Map & other){
	map_width = other.map_width;
	map_height = other.map_height;
    playersStart = other.playersStart;
    boxesStart = other.boxesStart;
    goals = other.goals;

    delete[] static_map;
	static_map = new char[map_height*map_width];
    memcpy(static_map, other.static_map, map_height*map_width*sizeof(char));

    return *this;
}

vector<Coordinate> Map::getBoxes() const{
	return boxesStart;
}
vector<Coordinate> Map::getGoals() const{
	return goals;
}
size_t Map::width() const{
	return map_width;
}
size_t Map::height() const{
	return map_height;
}

Coordinate Map::getStart() const {
	return playersStart;
}

bool Map::isWall(const Coordinate pos) const{
	return static_map[pos.second*map_width+pos.first] == WALL;
}

vector<State> Map::getSuccessorStates(const State state) const {
   vector<State> ret;
   Coordinate playerPos = state.getPlayerPosition();
   vector<Coordinate> boxes = state.getBoxes();
   string history = state.getHistory();
   int x = 0, y = 0;
   for(int i=0;i<4;i++){
        //Some magic to generate sequence x = -1,0,1,0 and y = 0,-1,0,1
        
	   x = (-1 + i) * ((i + 1) % 2);
        y = (-2 + i) * (i % 2);

		cout << "WHAT" << endl;
        Coordinate newPos = Coordinate(playerPos.first+x,playerPos.second+y);
        static string move[] = { "L", "D", "R", "U" };
        
		cout << "Before.." << endl;
		if(newPos.first < 0 || newPos.second < 0 || newPos.first >= map_width || newPos.second >= map_height || isWall(newPos)) {
            //Impossible to move into a wall or outside the map, next
            continue;
        }
		cout << "After!" << endl;
       
		cout << "And I am still alive..." << endl;
		if(state.isBox(newPos)){
            //Try box push
            Coordinate newBoxPos = Coordinate(newPos.first+x,newPos.second+y);
		cout << "Second Map-alive!" << endl;
            if (!(newBoxPos.first < 0 || newBoxPos.second < 0 || newBoxPos.first >= map_width || newBoxPos.second >= map_height) && (!isWall(newBoxPos) && !state.isBox(newBoxPos))){
		cout << "Third Map-alive!" << endl;
                //If this move is possible, change location of the box 
                //TODO could be optimized
                vector<Coordinate> newBoxes = vector<Coordinate>();
                for(size_t j=0; j<newBoxes.size(); j++){
                    if(newBoxes[j] == newPos){
                        newBoxes[j] = newBoxPos;
                    }
                }
                ret.push_back(State(newPos,newBoxes, history+move[i])); 
            }
        } else { 
            //Player moves into empty square
		cout << "And I am still alive..." << endl;
            ret.push_back(State(newPos,boxes, history+move[i]));
        }

		cout << "And I am still alive..." << endl;
    }
   return ret;
    //TODO optimize: check for duplicated states
}

ostream& operator<<(ostream &out, const Map &a ){
	for(size_t row=0; row<a.height() ;++row){
		for(size_t col=0; col<a.width() ;col++){
			switch(a.static_map[row*a.width()+col]){
				case EMPTY: out << EMPTY_CHAR; break;
				case WALL:  out << WALL_CHAR;  break;
				case GOAL:  out << GOAL_CHAR;  break;
				default:    out << '?';        break;
			}
		}
		out << endl;
	}
	return out;
}
