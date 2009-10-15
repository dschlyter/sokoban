#ifndef BUCKETLIST
#define BUCKETLIST

#include <list>
#include "state.h"
#include "solver.h"

class BucketList{
	private:
		std::list<intStatePair> *buckets;
		size_t size_b;
		int min;
		int max;		
		size_t content;	
		
	public:
		BucketList(const int mi,const int ma,const size_t s){
			size_b = s;
			min = mi;
			max = ma;
			content = 0;
			buckets = new std::list<intStatePair>[size_b];
		}
		~BucketList(){
			delete [] buckets;
		}
		
		void push(intStatePair p){
			int cost = p.first;
			size_t index = (std::abs(min)+cost)% std::abs((long int)((max-min)/size_b));
			buckets[index].push_front(p);
			++content;
		}
		
		intStatePair& top(){
			for(size_t i=0; i<size_b ;++i){
				if(buckets[i].size() > 0){
					return buckets[i].front();
				}
			}
		}
		
		void pop(){
			for(size_t i=0; i<size_b ;++i){
				if(buckets[i].size() > 0){
					buckets[i].pop_front();
					--content;
				}
			}
		}
		
		size_t size() const{
			return content;
		}
};

#endif

