#include "map.h"
#include "state.h"
#include <cstring>
#include <queue>
#include <iostream>

using namespace std;

// Debug statistics for different cases of normalized position calculation
int aCount;
int bCount;
int cCount;
int dCount;
bool printCounts = false;

Map::Map(const string map){
	// Calc size of map
	map_width = 0;
	map_height = 0;

    aCount = 0;
    bCount = 0;
    cCount = 0;
    dCount = 0;

    size_t nl = 0;
    while(true) {
        size_t next_nl = map.find('\n', nl+1);
        if(next_nl == string::npos) break;

        ++map_height;
        size_t width = next_nl - nl;
        if(width > map_width) map_width = width;

        nl = next_nl;
    }

    long memsize = map_height * map_width;
	static_map = new char[memsize];
	deadLock = new bool[memsize];
	goalDistance = new int[memsize];
	startDistance = new int[memsize];

    memset(static_map, 0, sizeof(char)*memsize);
    memset(deadLock, 0, sizeof(bool)*memsize);
    memset(goalDistance, 0, sizeof(int)*memsize);
    memset(startDistance, 0, sizeof(int)*memsize);

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
			case PLAYER_ON_GOAL_CHAR:
				static_map[row*map_width+col] = GOAL;
				goals.push_back(Coordinate(col, row));
                playersStart = Coordinate(col,row);
				break;
			case PLAYER_CHAR:
				static_map[row*map_width+col] = EMPTY;
                playersStart = Coordinate(col,row);
				break;
			case BOX_ON_GOAL_CHAR:
				static_map[row*map_width+col] = GOAL;
				boxesStart.push_back(Coordinate(col, row));
				goals.push_back(Coordinate(col, row));
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
	
	// Range calculation
	
	for (size_t i = 0; i < map_height; i++) {
		for (size_t j = 0; j < map_width; j++) {
			int min = 10000000;
			for (size_t b = 0; b < goals.size(); b++) {
				Coordinate t;
				t.first = j;
				t.second = i;
				int tmp = manhattanDistance(t, goals[b]);
				if (tmp < min) {
					min = tmp;
				}
			}
			goalDistance[i*map_width+j] = min;
		}
	}

	for (size_t i = 0; i < map_height; i++) {
		for (size_t j = 0; j < map_width; j++) {
			int min = 10000000;
			for (size_t b = 0; b < boxesStart.size(); b++) {
				Coordinate t;
				t.first = j;
				t.second = i;
				int tmp = manhattanDistance(t, boxesStart[b]);
				if (tmp < min) {
					min = tmp;
				}
			}
			startDistance[i*map_width+j] = min;
		}
	}

	// Deadlock detection
	for(size_t y=0; y<map_height-1 ;++y){
		for(size_t x=0; x<map_width-1 ;++x){
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
	
	
	// column deadlock detection
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


	// row deadlock detection
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


    //std::cout << "Precalculated deadlock positions marked with " << DEADLOCK_CHAR << endl << endl;
	//std::cout<<*this<<std::endl;
}

Map::~Map(){
	delete [] static_map;
	delete [] deadLock;
	delete [] goalDistance; 
    delete [] startDistance;
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

    for(size_t i=0; i<boxesStart.size(); i++){
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

// There can be multiple possible player positions for an end state
// This calculates all of them
vector<State> Map::getAllEndStates() const {
    vector<State> ret;
    vector<Coordinate> boxes = this->getBoxes();

    bool boxMap[map_height*map_width];
    bool visitMap[map_height*map_width];
    memset(boxMap, 0, sizeof(bool)*map_width*map_height);
    memset(visitMap, 0, sizeof(bool)*map_width*map_height);
    for(size_t i=0; i<goals.size(); i++){
        boxMap[goals[i].second*map_width+goals[i].first] = true;
    }

    // Moves: L U R D
    static int moveX[] = { -1, 0, 1, 0 };
    static int moveY[] = { 0, -1, 0, 1 };

    Coordinate dummyPosition = Coordinate(-1,-1);

    for(size_t y=0; y<map_height; y++) {
        for(size_t x=0; x<map_width; x++) {
            size_t mapIndex = y * map_width + x;
            if(boxMap[mapIndex]) {
                for(int i=0; i<4; i++){
                    Coordinate moveCoord = Coordinate(x + moveX[i], y + moveY[i]);
                    int moveArrayIndex = moveCoord.second * map_width + moveCoord.first;
                    if(!visitMap[moveArrayIndex] && static_map[moveArrayIndex] != WALL 
                            && !boxMap[moveArrayIndex]){
                        Coordinate normalizedPosition = calcNormalizedPosition(moveCoord, boxMap, visitMap);

                        vector<Coordinate> newBoxes = vector<Coordinate>(boxes);

                        ret.push_back(State(normalizedPosition, goals, 0, dummyPosition, -1));
                    }
                }
                 
            }
        }
    }

    return ret;
}

vector<State> Map::getPredecessorStates(const State state) const {
    vector<State> ret;
    Coordinate playerPos = state.getPlayerPosition();
    vector<Coordinate> boxes = state.getBoxes();
    int cost = state.getCost();

    bool boxMap[map_height*map_width];
    bool visitMap[map_height*map_width];
    memset(boxMap, 0, sizeof(bool)*map_width*map_height);
    memset(visitMap, 0, sizeof(bool)*map_width*map_height);
    for(size_t i=0; i<boxes.size(); i++){
        boxMap[boxes[i].second*map_width+boxes[i].first] = true;
    }

    // Moves: L U R D
    static int moveX[] = { -1, 0, 1, 0 };
    static int moveY[] = { 0, -1, 0, 1 };
    
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
                    Coordinate pullCoord = Coordinate(tmp.first - moveX[i], tmp.second - moveY[i]);
                    int pullArrayIndex  = pullCoord.second * map_width + pullCoord.first;
                    //Check if box can be pulled
                    if(static_map[pullArrayIndex] == WALL || boxMap[pullArrayIndex]){
                        continue;
                    }
                    //Add to list of possible pushes
                    pushes.push_back(boxPush(pullCoord, i));
                } else {
                    //If empty square, keep on searching on neighbours by pushing into queue
                    visitMap[moveArrayIndex] = 1;
                    q.push(moveCoord);
                }
            }
        }
    }


    // Add all new states to the priority queue, and calculate their normalized position
    bool visitMap2[map_width * map_height]; 
    for(size_t i=0; i<pushes.size(); i++){
        boxPush tmp = pushes[i];
        Coordinate newPlayerPos = tmp.first;
        int moveType = tmp.second;
        Coordinate newBoxPos = Coordinate(newPlayerPos.first + moveX[moveType], newPlayerPos.second + moveY[moveType]);
        Coordinate oldBoxPos = Coordinate(newPlayerPos.first + 2*moveX[moveType], newPlayerPos.second + 2*moveY[moveType]);
        
        // TODO remove
        //int playerArrayIndex = newPlayerPos.second * map_width + newPlayerPos.first;
        int boxArrayIndex = newBoxPos.second * map_width + newBoxPos.first;
        int oldBoxArrayIndex = oldBoxPos.second * map_width + oldBoxPos.first;

        //temporary change of boxmap for the new stage
        boxMap[boxArrayIndex] = true;
        boxMap[oldBoxArrayIndex] = false;

        Coordinate normalizedPosition;
        memset(visitMap2, 0, sizeof(bool)*map_width*map_height);
        normalizedPosition = calcNormalizedPosition(newPlayerPos, boxMap, visitMap2);

        //reset boxMap
        boxMap[boxArrayIndex] = false;
        boxMap[oldBoxArrayIndex] = true;

        //change box location and push
        vector<Coordinate> newBoxes = vector<Coordinate>(boxes);
        for(size_t j=0; j<newBoxes.size(); j++){
            if(newBoxes[j] == oldBoxPos){
                newBoxes[j] = newBoxPos;
            }
        }

        ret.push_back(State(normalizedPosition, newBoxes, cost+1, newBoxPos, tmp.second));
    }

    return ret;
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
    for(size_t i=0; i<boxes.size(); i++){
        boxMap[boxes[i].second*map_width+boxes[i].first] = true;
    }

    // Moves: L U R D
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
    for(size_t i=0; i<pushes.size() && !foundPICorral; i++){
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
    for(size_t i=0; i<pushes.size(); i++){
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

    // Add all new states to the priority queue, and calculate their normalized position
    bool visitMap2[map_width * map_height]; 
    for(size_t i=0; i<successorPushes.size(); i++){
        boxPush tmp = successorPushes[i];
        Coordinate newPlayerPos = tmp.first;
        int moveType = tmp.second;
        Coordinate newBoxPos = Coordinate(newPlayerPos.first + moveX[moveType], newPlayerPos.second + moveY[moveType]);
        int playerArrayIndex = newPlayerPos.second * map_width + newPlayerPos.first;
        int boxArrayIndex = newBoxPos.second * map_width + newBoxPos.first;

        //temporary change of boxmap for the new stage
        boxMap[boxArrayIndex] = true;
        boxMap[playerArrayIndex] = false;

        // The calcNormalizedPosition is the most intensive operation in the algorithm, since it is called before pushing every new state, and thus it benefits from some heavy optimization. We would prefer not to run this calculation if it can be avoided.
        
        // If a box has been pushed from @ to $, and the player is now standing on @
        // 1@7
        // 2$6
        // 345
        
        // A new path has opened iff 1 or 7 is unvisited
        
        // A path has closed iff box position is visied and one of the below is true
        // * 2 and 6 are visited, and there is no path between them
        // * 2 and 4 are visited, and there is no path between them
        // * 4 and 6 are visited, and there is no path between them

        // TODO try if statement with hardcoded values instead of array lookups
        int p1 = playerArrayIndex + moveY[(moveType+1)%4] * map_width + moveX[(moveType+1)%4];
        int p7 = playerArrayIndex + moveY[(moveType+3)%4] * map_width + moveX[(moveType+3)%4];

        int p2 = boxArrayIndex + moveY[(moveType+1)%4] * map_width + moveX[(moveType+1)%4];
        int p6 = boxArrayIndex + moveY[(moveType+3)%4] * map_width + moveX[(moveType+3)%4];

        int p4 = boxArrayIndex + moveY[moveType] * map_width + moveX[moveType];
        int p3 = p4 + moveY[(moveType+1)%4] * map_width + moveX[(moveType+1)%4];
        int p5 = p4 + moveY[(moveType+3)%4] * map_width + moveX[(moveType+3)%4];

        bool p1isUnvisitedPath = !visitMap[p1] && static_map[p1] != WALL && !boxMap[p1];
        bool p7isUnvisitedPath = !visitMap[p7] && static_map[p7] != WALL && !boxMap[p7];
        bool newPathOpen = p1isUnvisitedPath || p7isUnvisitedPath;

        bool existingPathClosed = false;
        if(visitMap[boxArrayIndex]) {
            bool p1v = visitMap[p1];
            bool p7v = visitMap[p7];
            bool p2v = visitMap[p2];
            bool p6v = visitMap[p6];
            bool p3v = visitMap[p3];
            bool p4v = visitMap[p4];
            bool p5v = visitMap[p5];

            if((p2v && p4v && !p3v) || (p4v && p6v && !p5v) 
                || (p2v && p6v && (!p1v || !p7v) && (!p3v || !p4v || !p5v)) ) {
                existingPathClosed = true;
            }

            bool boxPushedOntoNormalizedPosition = newBoxPos.first == minX && newBoxPos.second == minY;
            if(boxPushedOntoNormalizedPosition) {
                existingPathClosed = true;
            }
        }

        Coordinate normalizedPosition;
        if(!newPathOpen && !existingPathClosed) {
            // No paths have been changed, reuse existing position
            normalizedPosition = Coordinate(minX, minY);
            aCount++;
        } else if(newPathOpen && !existingPathClosed) {
            // A new path has opened, only examine new locations and compare with existing position
            // We reuse the exiting visited map to avoid visiting new states
            memcpy(visitMap2, visitMap, sizeof(bool)*map_width*map_height);
            normalizedPosition = calcNormalizedPosition(newPlayerPos, boxMap, visitMap2);
            
            //Check if newfound position is better than the old one
            if(normalizedPosition.second > minY 
                    || (normalizedPosition.second == minY && normalizedPosition.first > minX)){
                normalizedPosition = Coordinate(minX, minY);
            }

            bCount++;
        } else if(!newPathOpen && existingPathClosed) {
            // TODO Calculate new position, but abort if we find our way to the closed paths
            memset(visitMap2, 0, sizeof(bool)*map_width*map_height);
            normalizedPosition = calcNormalizedPosition(newPlayerPos, boxMap, visitMap2);
            cCount++;
        } else {
            // A combination of closed and opened paths
            // Calculate everything from scratch (this should be rare)
            memset(visitMap2, 0, sizeof(bool)*map_width*map_height);
            normalizedPosition = calcNormalizedPosition(newPlayerPos, boxMap, visitMap2);
            dCount++;
        }

        // The new player position can be the new normalized position, 
        // but will not be reached by some optimized searches,
        // make a special case check for it
        if (normalizedPosition.second > newPlayerPos.second 
                || (normalizedPosition.second == newPlayerPos.second 
                    && normalizedPosition.first > newPlayerPos.first)) {
            normalizedPosition = newPlayerPos;
        }

        //reset boxMap
        boxMap[boxArrayIndex] = false;
        boxMap[playerArrayIndex] = true;

        //change box location and push
        vector<Coordinate> newBoxes = vector<Coordinate>(boxes);
        for(size_t j=0; j<newBoxes.size(); j++){
            if(newBoxes[j] == newPlayerPos){
                newBoxes[j] = newBoxPos;
            }
        }

        ret.push_back(State(normalizedPosition, newBoxes, cost+1, newPlayerPos, tmp.second));
    }
    return ret;
}

// TODO move to solver, because this is method for solving
string Map::backtrack(const parentState finalMove, unordered_map<U64, parentState> * parentStates) const{
    if(printCounts) {
        cerr << "Normalized position stats:" << endl;
        cerr << "No new path: " << aCount << endl;
        cerr << "Open path:   " << bCount << endl;
        cerr << "Closed path: " << cCount << endl;
        cerr << "Both:        " << dCount << endl;
    }

    //Thus function builds a string of moves
    string ret = "";

    //Initialize box state from winning state
    vector<Coordinate> boxes = this->getGoals();
    bool boxMap[map_height*map_width];
    memset(boxMap, 0, sizeof(bool)*map_width*map_height);
    for(size_t i=0; i<boxes.size(); i++){
        boxMap[boxes[i].second*map_width+boxes[i].first] = true;
    }

    //Desclare other maps (initialized later)
    bool visitMap[map_height*map_width];
    string historyMap[map_height*map_width];

    //Declare and init a few variables used in the backtrack
    U64 hash = (*parentStates)[finalMove.first].first;
    Coordinate currentPos = (*parentStates)[finalMove.first].second.first;
    int moveType = (*parentStates)[finalMove.first].second.second;
    Coordinate goal;

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
        for(size_t i=0; i<map_height*map_width; i++){
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

//For calculating the normalized player position in the initial state
bool Map::verifySolution(char *sol) const{
    bool boxMap[map_height*map_width];
    memset(boxMap, 0, sizeof(bool)*map_width*map_height);

    for(size_t i=0; i<boxesStart.size(); i++){
        Coordinate box = boxesStart[i];
        boxMap[box.second * map_width + box.first] = true;
    }

    int x = playersStart.first;
    int y = playersStart.second;

    for(int i = 0; sol[i] != '\0'; i++) {
        char move = sol[i];
        int dx = 0;
        int dy = 0;
        
        if(move == 'L') dx = -1;
        else if (move == 'U') dy = -1;
        else if (move == 'R') dx = 1;
        else if (move == 'D') dy = 1;
        else {
            cerr << "Invalid move: " << move << endl;
            return false;
        }

        int newX = x + dx;
        int newY = y + dy;
        int newIndex = newY * map_width + newX;
        if(static_map[newIndex] == WALL) {
            cerr << "Move " << i << " invalid, tried to move into wall" << endl;
            return false;
        }

        if(boxMap[newIndex]) {
            int boxNewX = newX + dx;
            int boxNewY = newY + dy;
            int boxIndex = boxNewY * map_width + boxNewX;

            if(static_map[newIndex] == WALL) {
                cerr << "Move " << i << " invalid, tried to move box into wall" << endl;
                return false;
            }

            if(boxMap[boxIndex]) {
                cerr << "Move " << i << " invalid, tried to move box into another box" << endl;
                return false;
            }

            boxMap[newIndex] = false;
            boxMap[boxIndex] = true;
        }

        x = newX;
        y = newY;
    }

    bool success = true;
    // All moves done, check for valid solution
    for(size_t i=0; i< map_width * map_height; i++) {
        if(static_map[i] == EMPTY && boxMap[i]) {
            success = false;
            int x = i / map_width;
            int y = i % map_width; 
            cerr << "Invalid solution, box is on empty space at X: " << x << ", Y: " << y << endl;
        }

        if(static_map[i] == GOAL && !boxMap[i]) {
            success = false;
            int x = i / map_width;
            int y = i % map_width; 
            cerr << "Invalid solution, no box on goal at X: " << x << ", Y: " << y << endl;
        }
    }

    return success;
}
    
//Function used for debugging
void Map::printState(const State & state) const {
	cout << "Pushes: " << state.getCost() << endl << endl;
	for (size_t y = 0; y < height(); y++) {
		for (size_t x = 0; x < width(); x++) {
			Coordinate tmp;
			tmp.first = x;
			tmp.second = y;
			if (isWall(tmp)) {
				cout << WALL_CHAR;
			}
			else if (state.getPlayerPosition() == tmp) {
                if (isGoal(tmp)){
                    cout << PLAYER_ON_GOAL_CHAR;
                } else {
				    cout << PLAYER_CHAR;
                }
			}
			else {
				vector<Coordinate> boxes = state.getBoxes();
				bool done = false;
				for (size_t i = 0; i < boxes.size(); i++) {
					if (boxes[i].first == x && boxes[i].second == y) {
						done = true;
                        if(isGoal(tmp)){
                            cout << BOX_ON_GOAL_CHAR;
                        } else {
                            cout << BOX_CHAR;
                        }
						break;
					}
				}
			    if (!done) {
                    if (isGoal(tmp)){
                        cout << GOAL_CHAR;
                    } else {
			            cout << EMPTY_CHAR;
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

int Map::manhattanDistance(const Coordinate first, const Coordinate second) const {
	return abs(first.first - second.first) + abs(first.second - second.second);	
}

int Map::distanceGoal(const Coordinate box) const {
	return goalDistance[box.second*map_width+box.first];
}

int Map::distanceStart(const Coordinate box) const {
	return startDistance[box.second*map_width+box.first];
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
