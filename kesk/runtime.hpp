#pragma once

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <queue>
#include "node.hpp"
#include "parser.hpp"
#include "logger.hpp"

namespace kesk {
	
	struct type_pack;
	typedef void (*type_function) (type_pack*);
	
	struct handler_pack;
	typedef void (*handler_function) (handler_pack*);
	
	struct post_pack;
	
	struct runtime {
			
		runtime () : root("root") {
			
			current_node = &root;
			verbosity = 0;
			
			allow_compile_stdos = false;
			allow_runtime_stdos = false;
			allow_compile_stdes = false;
			allow_runtime_stdes = false;
		}
		
		node root;
		std::map<std::string, handler_function> handlers;
		std::map<std::string, type_function> types;
		std::queue<post_pack*> post_handlers;
		
		std::vector<node*> previous_nodes;
		node* current_node;
		
		bool
			allow_compile_stdos,
			allow_runtime_stdos,
			allow_compile_stdes,
			allow_runtime_stdes;
			
		unsigned char verbosity;
		
		bool init(const std::string&, type_pack*);
		bool init(const std::string&, std::vector<std::string>*, logger*, std::string*, size_t);
		bool execute(std::string, logger*);
		std::vector<std::string> compile(std::string&, logger*);
		void bytecode(const std::string&);
		void post(const std::string&, handler_pack*);
		void execute_posts();
	};
	
	struct type_pack {
		
		std::vector<std::string>* bc;
		parser p;
		runtime* r;
		logger* l;
		bool post_build;
		
		void push(const std::string& bytecode) {
			
			bc->push_back(bytecode);
		}
		
		void log(vl v, const std::string& message) {
			
			if (r->allow_compile_stdos && r->verbosity <= (unsigned char)v) std::cout << message << std::endl;
			l->log(v, message);
		}
		
		void error(const std::string& message) {
			
			if (r->allow_compile_stdes && r->verbosity >= (unsigned char)vl::error) std::cerr << "Error: " << message << std::endl;
			l->log(vl::error, message);
		}
		
		void error(size_t line, size_t pos, const std::string& message) {
			
			if (r->allow_compile_stdes && r->verbosity >= (unsigned char)vl::error)
				std::cerr << "@" << std::to_string(line) << ":" << std::to_string(pos) << " Error: " << message << std::endl;
				
			l->log(vl::error, message);
		}
		
		bool init(const std::string& type, type_pack* tp) {
			
			return r->init(type, tp);
		}
		
		bool init(const std::string& type, std::vector<std::string>* bc, logger* l, std::string* src, size_t i) {
			
			return r->init(type, bc, l, src, i);
		}
	};
	
	struct handler_pack {
		
		std::vector<std::string> args;
		std::vector<node*>* previous_nodes;
		node* current_node;
		runtime* r;
		bool post;
		
		void push(node* n) {
			
			previous_nodes->push_back(n);
		}
		
		void pop() {
			
			previous_nodes->pop_back();
		}
		
		void set(node* n) {
			
			current_node = n;
		}
		
		void error(const std::string& message) {
			
			if (r->allow_runtime_stdes) std::cerr << "Runtime Error: " << message << std::endl;
		}
	};
	
	struct post_pack {
		
		std::vector<std::string> args;
		node* current_node;
		runtime* r;
		handler_function hf;
	};
	
	bool runtime::init(const std::string& type, type_pack* tp) {
		
		return init(type, tp->bc, tp->l, tp->p.source, tp->p.index);
	}
	
	bool runtime::init(const std::string& type, std::vector<std::string>* bc, logger* l, std::string* src, size_t i) {
		
		auto it = types.find(type);
		
		if (it != types.end()) {
			
			type_pack* tp = new type_pack;
			
			tp->p = parser(src, i);
			tp->bc = bc;
			tp->r = this;
			tp->l = l;
			
			it->second(tp);
			
			delete tp;
			
			return true;
		}
		else return false;
	}
	
	bool runtime::execute(std::string scr, logger* l = nullptr) {
		
		std::vector<std::string> bc = compile(scr, l);
		
		log_message lm;
		while (l->pull(&lm)) if (lm.vlevel == vl::error) return false;
		
		for (auto bcl:bc) bytecode(bcl);
		
		execute_posts();
		
		return true;
	}
	
	std::vector<std::string> runtime::compile(std::string& scr, logger* l = nullptr) {
		
		bool log = true;
		if (l == nullptr) log = false;
		
		std::vector<std::string> bc;
		
		init("collection", &bc, l, &scr, 0);
		
		return bc;
	}
	
	void runtime::bytecode(const std::string& bc) {
		
		if (allow_runtime_stdos) std::cout << bc << std::endl;
		
		std::vector<std::string> tokens { "" };
		
		for (auto c:bc) {
			
			if (c == ' ') {
				
				tokens.push_back("");
			}
			else tokens.back() += c;
		}
		
		auto it = handlers.find(tokens[0]);
		
		if (it != handlers.end()) {
			
			handler_pack* hp = new handler_pack;
			
			if (tokens.size() > 1) hp->args = std::vector<std::string> (tokens.begin() + 1, tokens.end());
			hp->previous_nodes = &previous_nodes;
			hp->current_node = current_node;
			hp->r = this;
			hp->post = false;
			
			it->second(hp);
			
			current_node = hp->current_node;
			
			delete hp;
		}
	}
	
	void runtime::post(const std::string& handler, handler_pack* hp) {
		
		post_pack* pp = new post_pack;
		
		pp->args = hp->args;
		pp->r = hp->r;
		pp->current_node = hp->current_node;
		pp->hf = hp->r->handlers[handler];
		
		post_handlers.push(pp);
	}
	
	void runtime::execute_posts() {
		
		while (post_handlers.size()) {
			
			handler_pack* hp = new handler_pack;
			
			post_pack* pp = post_handlers.front();
			post_handlers.pop();
			
			hp->args = pp->args;
			hp->r = pp->r;
			hp->current_node = pp->current_node;
			hp->post = true;
			
			if (pp->hf != nullptr) {
				
				pp->hf(hp);
				
				delete hp;
			}
			
			delete pp;
		}
	}
}
