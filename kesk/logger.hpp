#pragma once

#include <string>
#include <queue>
#include <ostream>

namespace kesk {
	
	enum class vl : unsigned char {
		
		none,
		error,
		warning,
		note,
		info,
		step
	};
	
	struct log_message {
		
		log_message(vl v = vl::none, const std::string& m = "")
			: vlevel(v), message(std::string(m))
		{ }
		
		vl vlevel;
		std::string message;
	};
	
	struct logger {
		
		logger()
		{ }
		
		std::queue<log_message> logs;
		
		void log(vl v, const std::string& message) {
			
			logs.push(log_message(v, message));
		}
		
		bool pull(log_message* lm) {
			
			if (!logs.size()) return false;
			
			lm->vlevel = logs.front().vlevel;
			lm->message = logs.front().message;
			
			logs.pop();
			
			return true;
		}
	};
}