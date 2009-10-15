#include "map.h"
#include "state.h"
#include <cstring>
#include <queue>


Map::Map(const string map){
	// Calc size of map
	map_width = map.find('\n'); 
	map_height=0;

	for(size_t i=0; (i=map.find('\n', i+1))!=string::npos ;++map_height){} 
	

	static_map = new char[map_height*map_width];
	deadLock = new bool[map_height*map_width];
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
				deadLock[row*map_width+col] = true;
				break;
			case GOAL_CHAR:
				static_map[row*map_width+col] = GOAL;
				goals.push_back(Coordinate(col, row));
				break;
			case PLAYER_START_CHAR:
				static_map[row*map_width+col] = EMPTY;
                playersStart = Coordinate(col,row);
				break;
			case BOX_ON_GOAL_CHAR:
				static_map[row*map_width+col] = GOAL;
				boxesStart.push_back(Coordinate(col, row));
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

	// Deadlock detection
	deadLock = new bool[map_height*map_width];
	for(size_t y=0; y<map_height-1 ;++y){
		for(size_t x=0; x<map_width-1 ;++x){
			int walls = 0;
			Coordinate empty;
			// x .
			// . .
			if(static_map[y*map_width+x] == WALL) ++walls;
			else if(static_map[y*map_width+x] == EMPTY) empty = Coordinate(x,y);
			// . x
			// . .
			if(static_map[y*map_width+x+1] == WALL) ++walls;
			else if(static_map[y*map_width+x+1] == EMPTY) empty = Coordinate(x+1,y);
			// . .
			// x .
			if(static_map[(y+1)*map_width+x] == WALL) ++walls;
			else if(static_map[(y+1)*map_width+x] == EMPTY) empty = Coordinate(x,y+1);
			// . .
			// . x
			if(static_map[(y+1)*map_width+x+1] == WALL) ++walls;
			else if(static_map[(y+1)*map_width+x+1] == EMPTY) empty = Coordinate(x+1,y+1);
			
			if(walls == 3) deadLock[empty.second*map_width+empty.first] = true;
		}
	}

	// advanced collumn deadlock detection!!!!!!!1111one
	for(size_t x=1; x<map_width-1 ;++x){
		size_t first_dead = 0;
		bool left_wall = true; 
		bool right_wall = true;
		for(size_t y=1; y<map_height-1 ;++y){
			if(first_dead==0 && deadLock[y*map_width+x]){
			       first_dead = y;
			       //std::cout<<"first_dead: "<<x<<"x"<<y<<std::endl;
			}
			else{
				if(deadLock[y*map_width+x]){
					//std::cout<<"found deadLock "<<x<<"x"<<y<<std::endl;
					for(size_t i=first_dead; i<y ;i++){
						deadLock[i*map_width+x] = true;
					}
					first_dead = y;
					left_wall = true;
					right_wall = true;
				}
				else if(static_map[y*map_width+x] == EMPTY){
					//std::cout<<"found empty "<<x<<"x"<<y<<std::endl;
					if(static_map[y*map_width+x+1] != WALL) right_wall = false;
					if(static_map[y*map_width+x-1] != WALL) left_wall = false;
					//if(!deadLock[y*map_width+x+1]) right_wall = false;
					//if(!deadLock[y*map_width+x-1]) left_wall = false;

					if(!left_wall && !right_wall){
						first_dead = 0;
						left_wall = true;
						right_wall =  true;
					}	
				}
				else{
					first_dead = 0;
					left_wall = true;
					right_wall = true;
				}
			}
		}
	}


        // advanced row deadlock detection!!!!!!!1111one
        for(size_t y=1; y<map_height-1 ;++y){
                size_t first_dead = 0;
                bool up_wall = true;
                bool down_wall = true;
                for(size_t x=1; x<map_width-1 ;++x){
                        if(first_dead==0 && deadLock[y*map_width+x]){
                               first_dead = x;
                               //std::cout<<"first_dead: "<<x<<"x"<<y<<std::endl;
                        }
                        else{
                                if(deadLock[y*map_width+x]){
                                        //std::cout<<"found deadLock "<<x<<"x"<<y<<std::endl;
                                        for(size_t i=first_dead; i<x ;i++){
                                                deadLock[y*map_width+i] = true;
                                        }
                                        first_dead = x;
                                        up_wall = true;
                                        down_wall = true;
                                }
                                else if(static_map[y*map_width+x] == EMPTY){
                                        //std::cout<<"found empty "<<x<<"x"<<y<<std::endl;
                                        if(static_map[(y+1)*map_width+x] != WALL) down_wall = false;
                                        if(static_map[(y-1)*map_width+x] != WALL) up_wall = false;
					//if(!deadLock[(y+1)*map_width+x]) down_wall = false;
					//if(!deadLock[(y-1)*map_width+x]) up_wall = false;

                                        if(!up_wall && !down_wall){
                                                first_dead = 0;
                                                up_wall = true;
                                                down_wall =  true;
                                        }
                                }
                                else{
                                        first_dead = 0;
                                        up_wall = true;
                                        down_wall = true;
                                }
                        }
                }
        }


	std::cout<<*this<<std::endl;
}

Map::~Map(){
	delete [] static_map;
	delete [] deadLock;
}
Map::Map(const Map & other){
	map_width = other.map_width;
	map_height = other.map_height;
    playersStart = other.playersStart;
    boxesStart = other.boxesStart;
    goals = other.goals;

	static_map = new char[map_height*map_width];
    memcpy(static_map, other.static_map, map_height*map_width*sizeof(char));

	deadLock = new bool[map_height*map_width];
	memcpy(deadLock, other.deadLock, map_height*map_width*sizeof(char));
}
Map & Map::operator=(const Map & other){
	map_width = other.map_width;
	map_height = other.map_height;
    playersStart = other.playersStart;
    boxesStart = other.boxesStart;
    goals = other.goals;

    delete[] static_map;
	static_map = new char[map_height*map_width];
    memcpy(static_map, other.static_map, map_height*map_width*sizeof(bool));
	
	delete[] deadLock;
	deadLock = new bool[map_height*map_width];
	memcpy(deadLock, other.deadLock, map_height*map_width*sizeof(bool));
    
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

bool Map::isGoal(const Coordinate pos) const{
	return static_map[pos.second*map_width+pos.first] == GOAL;
}
bool Map::isWall(const Coordinate pos) const{
	return static_map[pos.second*map_width+pos.first] == WALL;
}
bool Map::isDeadLock(const Coordinate pos) const{
	return deadLock[pos.second*map_width+pos.first];
}

//For calculating the normalized player position in the initial state
Coordinate Map::calcNormalizedPosition() const{
    bool boxMap[map_height*map_width];
    bool visitMap[map_height*map_width];
    memset(boxMap, 0, sizeof(bool)*map_width*map_height);
    memset(visitMap, 0, sizeof(bool)*map_width*map_height);

    for(int i=0; i<boxesStart.size(); i++){
        Coordinate box = boxesStart[i];
        boxMap[box.second * map_width + box.first] = true;
    }

    return calcNormalizedPosition(playersStart, boxMap, visitMap);

}


//Calcs the normalized player position using bfs
//Uses sent in maps for computational efficiency
Coordinate Map::calcNormalizedPosition(const Coordinate startPos, const bool * boxMap, bool * visitMap) const{
    static int moveX[] = { -1, 0, 1, 0 };
    static int moveY[] = { 0, -1, 0, 1 };

    int minX = map_width+1;
    int minY = map_height+1;

    //kör bfs för att hitta tillåtna moves
    queue<Coordinate> q; 
    q.push(startPos);
    while(!q.empty()){
        Coordinate tmp = q.front();
        q.pop();
        if(tmp.second < minY || (tmp.second == minY && tmp.first < minX)){
            minX = tmp.first;
            minY = tmp.second;
        }

        for(int i=0; i<4; i++){
            Coordinate moveCoord = Coordinate(tmp.first + moveX[i], tmp.second + moveY[i]);
            int moveArrayIndex = moveCoord.second * map_width + moveCoord.first;
            if(!visitMap[moveArrayIndex] && static_map[moveArrayIndex] != WALL && !boxMap[moveArrayIndex]){
                visitMap[moveArrayIndex] = 1;
                q.push(Coordinate(moveCoord.first,moveCoord.second));
            }
        }
    }

    return Coordinate(minX,minY);
}

vector<State> Map::getSuccessorStates(const State state) const {
    vector<State> ret;
    Coordinate playerPos = state.getPlayerPosition();
    vector<Coordinate> boxes = state.getBoxes();
    int cost = state.getCost();

    //placera ut boxar, och initiera visited för sökning
    bool boxMap[map_height*map_width];
    bool visitMap[map_height*map_width];
    memset(boxMap, 0, sizeof(bool)*map_width*map_height);
    memset(visitMap, 0, sizeof(bool)*map_width*map_height);
    for(int i=0; i<boxes.size(); i++){
        boxMap[boxes[i].second*map_width+boxes[i].first] = true;
    }

    //TODO LATER: Kolla PI-corrals
    //TODO make parent matrix and backtrack
    
    static string move[] = { "L", "U", "R", "D" };
    static int moveX[] = { -1, 0, 1, 0 };
    static int moveY[] = { 0, -1, 0, 1 };
    
    static int aroundX[] = { 0,  1, 1, 1, 0, -1, -1, -1};
    static int aroundY[] = {-1, -1, 0, 1, 1,  1,  0, -1};

    int minX = map_width+1;
    int minY = map_height+1;

    typedef pair<Coordinate, int> boxPush;
    vector<boxPush> pushes;

    //kör bfs för att hitta tillåtna moves
    queue<Coordinate> q; 
    q.push(playerPos);
    visitMap[playerPos.second * map_width + playerPos.first] = true;
    while(!q.empty()){
        Coordinate tmp = q.front();
        q.pop();
        if(tmp.second < minY || (tmp.second == minY && tmp.first < minX)){
            minX = tmp.first;
            minY = tmp.second;
        }
        for(int i=0; i<4; i++){
            Coordinate moveCoord = Coordinate(tmp.first + moveX[i], tmp.second + moveY[i]);
            int moveArrayIndex = moveCoord.second * map_width + moveCoord.first;
            if(!visitMap[moveArrayIndex] && static_map[moveArrayIndex] != WALL){
                if(boxMap[moveArrayIndex]){
                    int boxArrayIndex = moveArrayIndex + moveY[i]*map_width + moveX[i];
                    //Check if box can be pushed
                    if(static_map[boxArrayIndex] == WALL || boxMap[boxArrayIndex]){
                        continue;
                    }
                    //If destination square isnt a goal, preform simple deadlock checking
                    if(static_map[boxArrayIndex] != GOAL){
                        //Abort if square is precalculated deadlock
                        if(deadLock[boxArrayIndex]){
                            continue;
                        }

                        //check if box is Pushed into a 2x2 cube of blocks or walls (always unsolveable)
                        //examine squares around in clockwise order
                        //7 0 1
                        //6 B 2
                        //5 4 3

                        bool occupied[8];
                        for(int j=0; j<8; j++){
                            occupied[j] = false;
                            int aroundArrayIndex = boxArrayIndex + aroundY[j] * map_width + aroundX[j];
                            if(aroundArrayIndex != moveArrayIndex && (static_map[aroundArrayIndex] == WALL || boxMap[aroundArrayIndex])){
                                occupied[j] = true; 
                            }
                        }
                        if(occupied[0] && occupied[1] && occupied[2]){
                            continue;
                        }
                        if(occupied[2] && occupied[3] && occupied[4]){
                            continue;
                        }
                        if(occupied[4] && occupied[5] && occupied[6]){
                            continue;
                        } 
                        if(occupied[6] && occupied[7] && occupied[0]){
                            continue;
                        }
                    }
                    pushes.push_back(boxPush(Coordinate(moveCoord.first,moveCoord.second), i));
                } else {
                    visitMap[moveArrayIndex] = 1;
                    q.push(Coordinate(moveCoord.first,moveCoord.second));
                }
            }
        }
    }

    bool visitMap2[map_width * map_height]; 
    for(int i=0; i<pushes.size(); i++){
        boxPush tmp = pushes[i];
        Coordinate newPlayerPos = tmp.first;
        int moveType = tmp.second;
        Coordinate newBoxPos = Coordinate(newPlayerPos.first + moveX[moveType], newPlayerPos.second + moveY[moveType]);
        int playerArrayIndex = newPlayerPos.second * map_width + newPlayerPos.first;
        int boxArrayIndex = newBoxPos.second * map_width + newBoxPos.first;
        //changeBoxMap
        boxMap[boxArrayIndex] = true;
        boxMap[playerArrayIndex] = false;
        //TODO opt if(!visitMap[(y + moveY[moveType]) * map_width + (x + moveX[moveType])] || 
        //memcpy + call
        //else
        memset(visitMap2, 0, sizeof(bool)*map_width*map_height);
        Coordinate normalizedPosition = calcNormalizedPosition(newPlayerPos, boxMap, visitMap2);
        //reset boxMap
        boxMap[boxArrayIndex] = false;
        boxMap[playerArrayIndex] = true;
        //change box location and push
        //TODO optimize O(n) loop
        vector<Coordinate> newBoxes = vector<Coordinate>(boxes);
        for(size_t j=0; j<newBoxes.size(); j++){
            if(newBoxes[j] == newPlayerPos){
                newBoxes[j] = newBoxPos;
            }
        }
        ret.push_back(State(normalizedPosition,newBoxes, cost+1));
    }
    //normalisera player state (kan kräva omräkning)
    //TODO uppdatera history (backtrack eller array av strings?)
    //TODO LATER: (beräkna hash, kolla duplicate state här istället)
    return ret;
}
    
//Function used for debugging
void Map::printState(const State & state) const {
	cout << "Depth: " << state.getCost() << endl;
	for (int y = 0; y < height(); y++) {
		for (int x = 0; x < width(); x++) {
			Coordinate tmp;
			tmp.first = x;
			tmp.second = y;
			if (isWall(tmp)) {
				cerr << "#";
			}
			else if (state.getPlayerPosition() == tmp) {
				cerr << "x";
			}
			else {
				vector<Coordinate> boxes = state.getBoxes();
				bool done = false;
				for (int i = 0; i < boxes.size(); i++) {
					if (boxes[i].first == x && boxes[i].second == y) {
						done = true;
                        if(isGoal(tmp)){
                            cerr << "¤";
                        } else {
                            cerr << "o";
                        }
						break;
					}
				}
			    if (!done) {
                    if (isGoal(tmp)){
                        cerr << "/";
                    } else {
			            cerr << ".";
                    }
				}
			}
		}
		cerr << endl;
	}
	cerr << endl;
	cerr << endl;
	for (int i = 0; i < 1000; i++) {
		for (int j = 0; j < 1000; j++) {
			if (j % 12 == 132) {
				cerr << endl;
			}
		}	
	}
}

ostream& operator<<(ostream &out, const Map &a ){
	for(size_t row=0; row<a.height() ;++row){
		for(size_t col=0; col<a.width() ;col++){
			switch(a.static_map[row*a.width()+col]){
				case EMPTY: 
					if(a.deadLock[row*a.width()+col]) out << DEADLOCK_CHAR;
					else out << EMPTY_CHAR; 
					break;
				case WALL:  out << WALL_CHAR;  break;
				case GOAL:  out << GOAL_CHAR;  break;
				default:    out << '?';        break;
			}
		}
		out << endl;
	}
	return out;
}
