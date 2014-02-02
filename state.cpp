#include "state.h"

int State::map_width = 0;
int State::secondStart = 0;
U64 * State::zobrist = 0;

//Normalized player pos, vector of boxes and start position
State::State(const Coordinate playerPosition, const vector<Coordinate> boxPositions, const Coordinate loc) : playerPos(playerPosition), boxes(boxPositions), moveLoc(loc){
    pushes = 0;
    moveType = -1;
    zobristHash();
}

//Normalized player pos, vector of boxes, pushCount, location after boxpush and type of move
State::State(const Coordinate playerPosition, const vector<Coordinate> boxPositions, const int pushCount, const Coordinate loc, const int move) : playerPos(playerPosition), boxes(boxPositions), pushes(pushCount), moveLoc(loc), moveType(move){
    zobristHash();
}

Coordinate State::getPlayerPosition() const{
	return playerPos;
}

vector<Coordinate> State::getBoxes() const{
	return boxes;
}

int State::getCost() const{
    return pushes;
}

Coordinate State::getMoveLoc() const {
    return moveLoc;
}

int State::getMoveType() const {
    return moveType;
}

U64 State::getHash() const{
    return hash;
}

bool State::isBox(Coordinate pos) const{
    for(size_t i=0; i<boxes.size(); i++){
        if(boxes[i] == pos){
            return true;
        }
    }
    return false;
}

void State::initZobristHash(int width, int height){
    map_width = width;
    int totalSize = width * height;
    secondStart = totalSize;
	zobrist = new U64[totalSize*2];

	for (int type = 0; type < 2; type++) {
		for (int coord = 0; coord < totalSize; coord++) {
			zobrist[type*totalSize+coord] = rand64();
		}
	}
}

void State::disposeZobristHash() {
    if(zobrist) delete[] zobrist;
}

// Generate 64-bit random number (naively)
U64 State::rand64() {
	return rand() ^ ((U64) rand() << 15) ^ ((U64) rand() << 30) ^ ((U64) rand() << 45) ^ ((U64) rand() << 60);
}

void State::zobristHash() {
	U64 key = 0;

	// Hash in player into key
	key ^= zobrist[playerPos.second*map_width+playerPos.first];

	// Hash in boxes into key
	for (int i = 0; i < boxes.size(); i++) {
		key ^= zobrist[secondStart + boxes[i].second*map_width+boxes[i].first];	
	}

    hash = key;
}
