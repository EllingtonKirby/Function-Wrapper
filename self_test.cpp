#include "Function.hpp"

using namespace cs540;

int ret_100(){
	return 100;
}

struct ret_two_hundred_functor_t {
  int operator()(){
    return 200;
  }
};

int main(int argc, char** argv){
	
  	auto ret_three_hundred_lambda_func = [](){
    		return 300;
  	};

	Function<int()> f1(ret_100);
	Function<int()> f2(ret_three_hundred_lambda_func);
	Function<int()> f3(ret_two_hundred_functor_t);
}
