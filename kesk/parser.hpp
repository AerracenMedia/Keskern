#pragma once

#include <string>
#include <ctype.h>

namespace kesk {
	
	struct parser {
		
		parser(std::string* src = nullptr, size_t i = 0)
			: index(i), sym(0), next_sym(0), end(true), line_num(1), line_pos(0)
		{ start(src); }
		
		unsigned char sym;
		unsigned char next_sym;
		size_t index;
		size_t line_num;
		size_t line_pos;
		std::string* source;
		bool end;
		
		bool start(std::string* src) {
			
			if (src == nullptr || !src->size()) return false;
			
			end = false;
			
			source = src;
			next_sym = source[0][0];
			
			return true;
		}
		
		unsigned char next(std::string* buffer = nullptr) {
			
			if (end) return 0;
			
			if (index < source->size() - 1) {
				
				index++;
				line_pos++;
			}
			else end = true;
				
			sym = next_sym;
			next_sym = source[0][index];
			
			if (sym == '\n') {
				
				line_num++;
				line_pos = 1;
			}
			
			if (buffer != nullptr) *buffer += sym;
			
			return sym;
		}
		
		bool accept(unsigned char query, std::string* buffer = nullptr) {
			
			if (end) return false;
			
			if (query == next_sym) {
				
				next(buffer);
				return true;
			}
			else return false;
		}
		
		bool accept(const std::string& query, std::string* buffer = nullptr) {
			
			if (end) return false;
			
			bool control = false;
			
			for (auto sc:query) {
            
				if (!control && sc == '`') {
					
					control = true;
				}
				else if (control) {
					
					switch (sc) {
							
						case 'w': if (isalnum(next_sym))   	goto csucc; break;
						case 'a': if (isalpha(next_sym))                 		goto csucc; break;
						case 'd': if (isdigit(next_sym))                 		goto csucc; break;
						case 's': if (isspace(next_sym))                 		goto csucc; break;
							
						default: if (next_sym == sc) { next(buffer); return true; } break;
					}
					
					continue;
					
					csucc:
					next(buffer);
					control = false;
					return true;
				} 
				else if (sc == next_sym) {
					
					next(buffer);
					return true;
				}
			}
			
			return false;
		}
		
		size_t chain(const std::string& query, std::string* buffer = nullptr, size_t iteration = 0) {
        
			if (end) return 0;
			
			if (accept(query)) {
				
				if (buffer != nullptr) *buffer += sym;
				
				return chain(query, buffer, iteration + 1);
			}
			else return iteration;
		}
		
		bool await(unsigned char query, std::string* buffer = nullptr) {
			
			if (end) return false;
			
			if (accept(query)) return true;
			else { next(buffer); return await(query, buffer); }
		}
		
		bool await(const std::string& query, std::string* buffer = nullptr) {
			
			if (end) return false;
			
			if (accept(query)) return true;
			else { next(buffer); return await(query, buffer); }
		}
		
		bool await(const std::string& query, const std::string& breaks, std::string* buffer = nullptr) {
			
			if (end) return false;
			
			if (accept(query)) return true;
			else {
				
				if (breaks.size() && accept(breaks)) return false;
				else {
				
					next(buffer);
					return await(query, breaks, buffer);
				}
			}
		}
		
		bool string(const std::string& query) {
			
			if (end) return false;
			
			for (auto c:query) {
				
				if (!accept(c)) return false;
			}
			
			return true;
		}
		
		bool close_bracket(const std::string& main, std::string* buffer = nullptr) {
			
			size_t ignore = 0;
			
			if (main.size() != 2) return false;
			
			while (!end) {
							
				if (accept(main[1])) {
					
					if (ignore == 0) {
						
						if (sym == main[1]) return true;
					}
					else {
						
						ignore--;
						if (buffer != nullptr) *buffer += sym;
					}
				}
				else if (accept(main[0], buffer)) ignore++;
				else next(buffer);
			}
			
			return false;
		}
	};
}