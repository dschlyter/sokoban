#ifndef BUCKETQUEUE
#define BUCKETQUEUE

#include <vector>
#include <deque>
#include "state.h"
typedef pair<int, State> intStatePair;

class BucketQueue {
	private:
		vector< deque<intStatePair>* > buckets;
		size_t noElements;
		int lowestHeuristic;
		bool grabFromFront;

		void findNextHeuristicValue() {
			if (noElements == 0) {
				lowestHeuristic = -1;
				return;
			}

			while (!buckets[lowestHeuristic] || buckets[lowestHeuristic]->size() == 0) {
				lowestHeuristic++;
			}
		}

	public:
		BucketQueue(const int maxHeuristic){
			this->buckets = vector< deque<intStatePair>* >();
			this->noElements = 0;
			this->lowestHeuristic = -1;
			this->grabFromFront = true;
		}
		~BucketQueue(){
			//delete buckets;
		}

		void printState() {
			for (size_t i = 0; i < buckets.size(); i++) {
				if (buckets[i]) {
					cout << i << ": bucket[" << buckets[i]->size() << "]" << endl;
				} else {
					cout << i << ": undefined" << endl;
				}
			}
		}
		
		void push(intStatePair p){
			//cout << "Want to PUSH heuristic: " << p.first << ", current size is " << noElements << ", buckets: " << buckets.size() << endl;
			int heuristic = p.first;
			if (lowestHeuristic == -1 || lowestHeuristic > heuristic) {
				lowestHeuristic = heuristic;
			}
			size_t size = buckets.size();
			
			if (heuristic >= size) {
				buckets.resize(heuristic+1, 0);

				//cout << "Not enough buckets pointers! Resized! Now size: " << noElements << ", buckets: " << buckets.size() << endl;
			}

			if (!buckets[heuristic]) {
				//cout << "Bucket " << heuristic << " didn't exist, so created! Size: " << noElements << ", buckets: " << buckets.size() << endl;
				buckets[heuristic] = new deque<intStatePair>();
			}
			
			deque<intStatePair> * bucket = buckets[heuristic];
			bucket->push_back(p);
			++noElements;


			//cout << "Added element to bucket " << heuristic << "! Size: " << noElements << ", buckets: " << buckets.size() << endl;
			//printState();
		}
		
		intStatePair pop(){
			//cout << "Want to POP, current size is " << noElements << ", buckets: " << buckets.size() << endl;
			deque<intStatePair> * bucket = buckets[lowestHeuristic];
			//cout << "Current lowest is " << lowestHeuristic << ", size: " << noElements << ", buckets: " << buckets.size() << endl;
			intStatePair ret;

			if (bucket->size() > 1) {

				//cout << "Bucket had more than 1 element in it! Size: " << noElements << ", buckets: " << buckets.size() << endl << flush;
				// Multiple elements in bucket
				// Grab one at random and place last element in its place.
				//int luckyElement = rand() % bucket->size();
				if (this->grabFromFront) {
					ret = bucket->front();
					bucket->pop_front();
				} else {
					ret = bucket->back();
					bucket->pop_back();
				}
				this->grabFromFront = !this->grabFromFront;

				/*
				ret = (*bucket)[luckyElement];
				intStatePair last = bucket->back();
				(*bucket)[luckyElement] = last;
				bucket->pop_back();
				*/
				--noElements;
				//printState();
				return ret;
			}

			//cout << "Bucket only had 1 element in it! Size: " << noElements << ", buckets: " << buckets.size() << endl << flush;
			// Last element in bucket
			// Need to find new lowest heuristic value
			ret = bucket->front();
			bucket->pop_front();
			--noElements;
			findNextHeuristicValue();

			//printState();
			return ret;
		}
		
		size_t size() const{
			return noElements;
		}
};

#endif

