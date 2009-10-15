#include "state.h"

State::State(const Coordinate playerPosition, const vector<Coordinate> boxPositions) : playerPos(playerPosition), boxes(boxPositions){
    pushes = 0;
}

State::State(const Coordinate playerPosition, const vector<Coordinate> boxPositions, const int pushCount) : playerPos(playerPosition), boxes(boxPositions), pushes(pushCount){
    
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

bool State::isBox(Coordinate pos) const{
    for(size_t i=0; i<boxes.size(); i++){
        if(boxes[i] == pos){
            return true;
        }
    }
    return false;
}
