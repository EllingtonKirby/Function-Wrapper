#include <iostream>
#include <typeinfo>
#include <cassert>
#include <cxxabi.h>
#include <memory>
#include <stdexcept>
#include <string>

auto demangle = [](const std::type_info &ti) {
	      int ec;
	          return abi::__cxa_demangle(ti.name(), 0, 0, &ec);
		      assert(ec == 0);
		        };

namespace cs540{
	//A lot of ideas for the design and implementation of this class were taken from this stackoverflow post 
	//http://stackoverflow.com/questions/14739902/is-there-a-standalone-implementation-of-stdfunction/14741161#14741161
	

	class BadFunctionCall : public std::runtime_error {
		public:
			BadFunctionCall(const std::string& message) : std::runtime_error(message) {};
	};

	template<typename> class Function{};

	template<typename ResultType, typename... ArgumentTypes>
	class abstract_callable{
		public:
			virtual ResultType operator()(ArgumentTypes... args) = 0;
			virtual abstract_callable *clone() const = 0;
			virtual ~abstract_callable(){};
	};

	template<typename FunctionType, typename ResultType, typename... ArgumentTypes>
	class callable : public abstract_callable<ResultType, ArgumentTypes...>{
		public:
			FunctionType func;
			callable(const FunctionType &f) : func(f){};
			ResultType operator()(ArgumentTypes... args){
				return func(args...);
			};	
			callable *clone() const{
				return new callable{func};
			};
	};

	template<typename FunctionType>
	struct typeset
	{
		typedef FunctionType type;
	};

	template<typename ResultType, typename ...ArgumentTypes>
	struct typeset<ResultType(ArgumentTypes...)>{
		typedef ResultType (*type)(ArgumentTypes...);
	};

	template<typename ResultType, typename... ArgumentTypes>
	class Function<ResultType(ArgumentTypes...)>{
		public:
			Function() : target(nullptr){};
			
			abstract_callable<ResultType, ArgumentTypes...> *target;
		
			template<typename FunctionType>
			Function(FunctionType func) : target(new callable<typename typeset<FunctionType>::type, ResultType, ArgumentTypes...>(func)){
			};

			Function(const Function &f) : target(f.target == nullptr ? nullptr : f.target->clone()){};
			
			Function &operator=(Function &f){
				if(this == &f){
					return *this;
				}	
				delete target;
				target = f.target->clone();
				return *this;
			};
			template<typename FunctionType> 
			Function &operator=(const FunctionType &f)
			{
		    		auto *temp = new callable<typename typeset<FunctionType>::type, ResultType, ArgumentTypes...>(f);
				delete target;
				target = temp;
				return *this;
			}

			~Function(){
				delete target;	
			};

			ResultType operator()(ArgumentTypes... args){
				if(target == nullptr){
					throw cs540::BadFunctionCall("cs540::BadFunctionCall, operator() called with no callable function");
				}	
				return (*target)(args...);
			};

			explicit operator bool() const{
				if(target == nullptr){
					return false;
				}
				try{
					(*target)();
				}
				catch(...){
					return false;
				}
				return true;
			};

	};

	
	template<typename ResultType, typename... ArgumentTypes>
	bool operator==(const Function<ResultType(ArgumentTypes...)> &f, std::nullptr_t){
		return f.target == nullptr;
	}
	template<typename ResultType, typename... ArgumentTypes>
	bool operator==(std::nullptr_t, const Function<ResultType(ArgumentTypes...)> &f){
		return f.target == nullptr;
	}
	template<typename ResultType, typename... ArgumentTypes>
	bool operator!=(const Function<ResultType(ArgumentTypes...)> &f, std::nullptr_t){
		return f.target != nullptr;
	}
	template<typename ResultType, typename... ArgumentTypes>
	bool operator!=(std::nullptr_t, const Function<ResultType(ArgumentTypes...)> &f) {
		return f.target != nullptr;
	}
	
}
