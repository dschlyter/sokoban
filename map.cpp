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
	for(size_t y=0; y<map_height-1 ;++y){
		for(size_t x=0; x<map_width-1 ;++x){
			int walls = 0;
			Coordinate empty = Coordinate(0,0);
			// x .
			// . .
			if(static_map[y*map_width+x] == WALL && static_map[(y+1)*map_width+x+1] == WALL){
				if(static_map[y*map_width+x+1] == EMPTY) deadLock[y*map_width+x+1] = true;
				if(static_map[(y+1)*map_width+x]== EMPTY) deadLock[(y+1)*map_width+x] = true;
			}
			// . x
			// . .
			else if(static_map[y*map_width+x+1] == WALL && static_map[(y+1)*map_width+x] == WALL){
				if(static_map[(y+1)*map_width+x+1] == EMPTY) deadLock[(y+1)*map_width+x+1] = true;
				if(static_map[y*map_width+x] == EMPTY) deadLock[y*map_width+x] = true;
			}
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
			else if(first_dead!=0){
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
			else if(first_dead!=0){
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


    std::cout << "Precalculated deadlock positions marked with " << DEADLOCK_CHAR << endl << endl;
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
                    //Add to list of possible pushes
                    pushes.push_back(boxPush(moveCoord, i));
                } else {
                    //If empty square, keep on searching on neighbours by pushing into queue
                    visitMap[moveArrayIndex] = 1;
                    q.push(moveCoord);
                }
            }
        }
    }

    //finding PI-corrals
    //flag for finding a corral
    bool foundPICorral = false;
    int corralIdentifier = 1;
    int corralMap[map_width * map_height];
    memset(corralMap, 0, sizeof(int)*map_width*map_height);

    //Iterate over all possible pushes
    for(int i=0; i<pushes.size() && !foundPICorral; i++){
        Coordinate square = pushes[i].first;
        int moveType = pushes[i].second;
        int pushArrayIndex = (square.second + moveY[moveType]) * map_width + (square.first + moveX[moveType]);
        if(!visitMap[pushArrayIndex] && !corralMap[pushArrayIndex]){
            //Destination for push is inside an unexplored corral, begin search
            foundPICorral = true;
            bool allGoalStates = true;
            queue<Coordinate> q;
            q.push(square);
            while(!q.empty()){
                Coordinate tmp = q.front();
                int tmpArrayIndex = tmp.second * map_width + tmp.first;
                q.pop();
                Coordinate moveCoord[4];
                int moveArrayIndex[4];
                for(int i=0; i<4; i++){
                    moveCoord[i] = Coordinate(tmp.first + moveX[i], tmp.second + moveY[i]);
                    moveArrayIndex[i] = moveCoord[i].second * map_width + moveCoord[i].first;
                }
                //If a box is found, validate that it can only be pushed into the corral
                if(boxMap[tmpArrayIndex]){
                    //We want to verify that at least one box is not a goal
                    if(static_map[tmpArrayIndex] != GOAL){
                        allGoalStates = false;
                    }
                    //If any box can be visited from right and left or visited from up and down, it can be pushed to another part of the corral
                    if((visitMap[moveArrayIndex[0]] && visitMap[moveArrayIndex[2]]) || (visitMap[moveArrayIndex[1]] && visitMap[moveArrayIndex[3]])){
                        //Mark corral as a non-PI-corral, but continue search in order to mark entire corral as visited
                        foundPICorral = false;
                    }
                }
                for(int i=0; i<4; i++){
                    //Continue exploring the corral (unvisited nodes or boxes)
                    if(!visitMap[moveArrayIndex[i]] && !corralMap[moveArrayIndex[i]] && static_map[moveArrayIndex[i]] != WALL){
                        corralMap[moveArrayIndex[i]] = corralIdentifier;
                        q.push(moveCoord[i]);
                    }
                }
            }
            //A PI-corral with all boxes on the goal does not nessessarily require pruning
            if(allGoalStates){
                foundPICorral = false;
            }
            if(foundPICorral){
                break;
            } else {
                corralIdentifier++;
            }
        }
    }

    //Node pruning
    vector<boxPush> successorPushes;
    for(int i=0; i<pushes.size(); i++){
        Coordinate pos = pushes[i].first;
        int moveType = pushes[i].second;
        //If a PI-corral has been found, prune all nodes that are not PI-corrals
        if(foundPICorral && corralMap[pos.second*map_width + pos.first] != corralIdentifier){
            continue;
        }
        int moveArrayIndex = pos.second * map_width + pos.first;
        int boxArrayIndex = (pos.second + moveY[moveType]) * map_width + (pos.first + moveX[moveType]);
        //If destination square isnt a goal, preform some simple deadlock checking
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
        successorPushes.push_back(pushes[i]);
    }

    bool visitMap2[map_width * map_height]; 
    for(int i=0; i<successorPushes.size(); i++){
        boxPush tmp = successorPushes[i];
        Coordinate newPlayerPos = tmp.first;
        int moveType = tmp.second;
        Coordinate newBoxPos = Coordinate(newPlayerPos.first + moveX[moveType], newPlayerPos.second + moveY[moveType]);
        int playerArrayIndex = newPlayerPos.second * map_width + newPlayerPos.first;
        int boxArrayIndex = newBoxPos.second * map_width + newBoxPos.first;
        //changeBoxMap
        boxMap[boxArrayIndex] = true;
        boxMap[playerArrayIndex] = false;
        Coordinate normalizedPosition;
        //If newBoxPos is unvisited or if box is has blocked squares to the left and right, we can reuse the visitmap
        int rightArrayIndex = boxArrayIndex + moveY[(moveType+1)%4] * map_width + moveX[(moveType+1)%4];
        int leftArrayIndex = boxArrayIndex + moveY[(moveType+3)%4] * map_width + moveX[(moveType+3)%4];
        if(!visitMap[boxArrayIndex] || ((boxMap[leftArrayIndex] || static_map[leftArrayIndex] == WALL) && (boxMap[rightArrayIndex] || static_map[rightArrayIndex] == WALL))){
            memcpy(visitMap2, visitMap, sizeof(bool)*map_width*map_height);
            normalizedPosition = calcNormalizedPosition(newPlayerPos, boxMap, visitMap2);
            //Check if newfound position is better than the old one
            if(normalizedPosition.second > minY || (normalizedPosition.second == minY && normalizedPosition.first < minX)){
                normalizedPosition = Coordinate(minX, minY);
            }
        } else {
            //Otherwise it needs to be calculated from scratch
            memset(visitMap2, 0, sizeof(bool)*map_width*map_height);
            normalizedPosition = calcNormalizedPosition(newPlayerPos, boxMap, visitMap2);
        }
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
        ret.push_back(State(normalizedPosition,newBoxes, cost+1,newPlayerPos,tmp.second));
    }
    return ret;
}

string Map::backtrack(const State * winningState, map<U64, parentState> * parentStates) const{
    //Thus function builds a string of moves
    string ret = "";

    //Initialize box state from winning state
    vector<Coordinate> boxes = winningState->getBoxes();
    bool boxMap[map_height*map_width];
    memset(boxMap, 0, sizeof(bool)*map_width*map_height);
    for(int i=0; i<boxes.size(); i++){
        boxMap[boxes[i].second*map_width+boxes[i].first] = true;
    }

    //Desclare other maps (initialized later)
    bool visitMap[map_height*map_width];
    string historyMap[map_height*map_width];

    //Declare and init a few variables used in the backtrack
    Coordinate currentPos = winningState->getMoveLoc();
    U64 hash = (*parentStates)[winningState->getHash()].first;
    Coordinate goal;
    int moveType = winningState->getMoveType();

    //Some static variables used for the search
    static string move[] = { "L", "U", "R", "D" };
    static int moveX[] = { -1, 0, 1, 0 };
    static int moveY[] = { 0, -1, 0, 1 };

    while(true){
        //undo the push preformed to get to the state
        boxMap[currentPos.second*map_width+currentPos.first] = true;
        boxMap[(currentPos.second + moveY[moveType]) * map_width + (currentPos.first + moveX[moveType])] = false;
        //also undo the move made to this state
        currentPos = Coordinate(currentPos.first - moveX[moveType], currentPos.second - moveY[moveType]);
        //and update string accordingly
        ret = move[moveType] + ret;

        //Find the previous state using hash
        //parentState = pair: (U64,(Coordinate,int))
        parentState p = (*parentStates)[hash];
        //and uppdate variables to reflect this
        //hash of parent
        hash = p.first;
        //end location of previous push
        goal = p.second.first;
        //type of move
        moveType = p.second.second;

        //reset visited map and history
        memset(visitMap, 0, sizeof(bool)*map_width*map_height);
        visitMap[currentPos.second * map_width + currentPos.first] = true;
        for(int i=0; i<map_height*map_width; i++){
            historyMap[i] = "";
        }

        //Prepare and preform bfs
        queue<Coordinate> q;
        q.push(currentPos);
        while(!q.empty()){
            //Pop a square
            Coordinate square = q.front();
            q.pop();
            int arrayIndex = square.second * map_width + square.first;
            if(square == goal){
                //if finished update string and break
                currentPos = goal;
                ret = historyMap[arrayIndex] + ret;
                break;
            }
            for(int i=0; i<4; i++){
                Coordinate moveCoord = Coordinate(square.first - moveX[i], square.second - moveY[i]);
                int moveArrayIndex = moveCoord.second * map_width + moveCoord.first;
                //Check for valid moves
                if(!visitMap[moveArrayIndex] && static_map[moveArrayIndex] != WALL && !boxMap[moveArrayIndex]){
                    //Set visited, update history and push
                    visitMap[moveArrayIndex] = true;
                    historyMap[moveArrayIndex] = move[i] + historyMap[arrayIndex];
                    q.push(Coordinate(moveCoord.first,moveCoord.second));
                }
            }
        }

        //If moveType is -1 we have reached the beginning, abort
        if(moveType == -1){
            break;
        }
    }
    return ret;
}
    
//Function used for debugging
void Map::printState(const State & state) const {
	cout << "Pushes: " << state.getCost() << endl << endl;
	for (int y = 0; y < height(); y++) {
		for (int x = 0; x < width(); x++) {
			Coordinate tmp;
			tmp.first = x;
			tmp.second = y;
			if (isWall(tmp)) {
				cout << "#";
			}
			else if (state.getPlayerPosition() == tmp) {
				cout << "x";
			}
			else {
				vector<Coordinate> boxes = state.getBoxes();
				bool done = false;
				for (int i = 0; i < boxes.size(); i++) {
					if (boxes[i].first == x && boxes[i].second == y) {
						done = true;
                        if(isGoal(tmp)){
                            cout << "O";
                        } else {
                            cout << "o";
                        }
						break;
					}
				}
			    if (!done) {
                    if (isGoal(tmp)){
                        cout << "/";
                    } else {
			            cout << ".";
                    }
				}
			}
		}
		cout << endl;
	}
	cout << endl;
	for (int i = 0; i < 1000; i++) {
		for (int j = 0; j < 1000; j++) {
			if (j % 12 == 132) {
				cout << endl;
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
