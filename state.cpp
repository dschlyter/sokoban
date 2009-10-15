#include "state.h"

State::State(const Coordinate playerPosition, const vector<Coordinate> boxPositions) : playerPos(playerPosition), boxes(boxPositions){
    history = string("");
    //pushes = 0;
}

State::State(const Coordinate playerPosition, const vector<Coordinate> boxPositions, const string stateHistory) : playerPos(playerPosition), boxes(boxPositions), history(stateHistory){
    
}

Coordinate State::getPlayerPosition() const{
	return playerPos;
}

vector<Coordinate> State::getBoxes() const{
	return boxes;
}

string State::getHistory() const{
    return history;
}

int State::getCost() const{
    return history.size();
    //return pushes;
}

bool State::isBox(Coordinate pos) const{
    for(size_t i=0; i<boxes.size(); i++){
        if(boxes[i] == pos){
            return true;
        }
    }
    return false;
}
