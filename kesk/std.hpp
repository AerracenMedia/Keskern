#pragma once

#include "runtime.hpp"
#include <string>
#include <vector>
#include <map>
#include <iostream>

namespace kesk {
	
	namespace type_functions {
		
		void collection(type_pack* tp) {
		
			tp->log(vl::info, "[INIT] Collection : " + *tp->p.source);
		
			std::string buffer = "";
			std::string path = "";
			size_t el = 0, ep = 0;
			
			while (!tp->p.end) {
				
				tp->p.chain("`s");
				tp->p.chain("`w_-", &buffer);
				
				if (tp->p.accept("/\\.")) {
					
					if (buffer.size()) {
						
						path += " " + buffer;
						buffer = "";
					}
				}
				else if (tp->p.accept(":=$")) {
					
					if (buffer.size()) {
						
						path += " " + buffer;
						buffer = "";
					}
					
					if (!path.size()) tp->error(tp->p.line_num, tp->p.line_pos, "Expected path before pathbreak.");
					
					tp->push("down" + path);
					path = "";
					
					tp->p.chain("`s");
					
					if (tp->p.accept('{')) {
						
						el = tp->p.line_num;
						ep = tp->p.line_pos;
						
						if (tp->p.close_bracket("{}", &buffer)) {
							
							buffer += ";";
							tp->push("type collection");
							tp->init("collection", tp->bc, tp->l, &buffer, 0);
							buffer = "";
							tp->push("back");
						}
						else tp->error(el, ep, "Expected '}' to end collection.");
					}
					else if (tp->p.accept("'\"")) {
						
						el = tp->p.line_num;
						ep = tp->p.line_pos;
						
						if (!tp->p.await(";,\n", &buffer)) tp->error(el, ep, "Expected ';' to end path value.");
					
						if (buffer.size()) {
							
							tp->push("type list");
							tp->init("string", tp->bc, tp->l, &buffer, 0);
							buffer = "";
							tp->push("back");
						}
					}
					else if (tp->p.accept('[')) {
						
						el = tp->p.line_num;
						ep = tp->p.line_pos;
						
						if (tp->p.close_bracket("[]", &buffer)) {
							
							buffer += ";";
							tp->push("type list");
							tp->init("list", tp->bc, tp->l, &buffer, 0);
							buffer = "";
							tp->push("back");
						}
						else tp->error(el, ep, "Expected ']' to end list.");
					}
					else if (tp->p.accept('&')) {
						
						el = tp->p.line_num;
						ep = tp->p.line_pos;
						
						if (!tp->p.await(";,\n", &buffer)) tp->error(el, ep, "Expected ';' to end path value.");
						
						if (buffer.size()) {
							
							tp->init("referance", tp->bc, tp->l, &buffer, 0);
							buffer = "";
							tp->push("back");
						}
					}
					else {
						
						el = tp->p.line_num;
						ep = tp->p.line_pos;
						
						if (!tp->p.await(";,\n", &buffer)) tp->error(el, ep, "Expected ';' to end path value.");
						
						if (buffer.size()) {
							
							tp->init("single", tp->bc, tp->l, &buffer, 0);
							buffer = "";
							tp->push("back");
						}
					}
				}
				else tp->p.next();
			}
		}
		
		void string(type_pack* tp) {
			
			tp->log(vl::info, "[INIT] String : " + *tp->p.source);
			
			std::string str = *tp->p.source;
			bool control = false;
			unsigned char c;
			
			for (std::string::size_type i = 0; i < str.size(); i++) {
				
				tp->push("down " + std::to_string(i));
				
				if (control == false) {
					
					if (str[i] == '"' || str[i] == '\'') return;
					else if (str[i] == '\\') control = true;
					else c = str[i];//tp->push("set " + std::to_string(str[i]));
				}
				else {
					
					switch (str[i]) {
						
						case '\'': c = '\''; break;
						case '"': c = '"'; break;
						case 'n': c = '\n'; break;
						case 'r': c = '\r'; break;
						case 't': c = '\t'; break;
						case 'v': c = '\v'; break;
						case '\\': c = '\\'; break;
						default: c = str[i];
							tp->log(vl::warning, "Warning: Unknown Escape Sequence \"\\" + std::string(1, str[i]) + "\".");
					}
				}
				
				tp->push("set " + std::to_string(c));
				tp->push("type integer");
				tp->push("back");
			}
		}
		
		void list(type_pack* tp) {
			
			tp->log(vl::info, "[INIT] List : " + *tp->p.source);
			
			size_t iteration = 0;
			std::vector<std::string> bc_tp;
			std::string buffer = "";
			
			size_t el = 0, ep = 0;
			
			while (!tp->p.end) {
				
				tp->p.chain("`s");
				
				if (tp->p.accept('{')) {
					
					el = tp->p.line_num;
					ep = tp->p.line_pos;
					
					if (tp->p.close_bracket("{}", &buffer)) {
							
						buffer += ";";
						bc_tp.push_back("type collection");
						tp->init("collection", &bc_tp, tp->l, &buffer, 0);
						buffer = "";
					}
					else tp->error(el, ep, "Expected '}' to end collection.");
				}
				else if (tp->p.accept("'\"")) {
					
					el = tp->p.line_num;
					ep = tp->p.line_pos;
					
					if (!tp->p.await(";,\n", &buffer)) tp->error(el, ep, "Expected ';' to end string.");
					
					if (buffer.size()) {
						
						bc_tp.push_back("type list");
						tp->init("string", &bc_tp, tp->l, &buffer, 0);
						buffer = "";
					}
				}
				else if (tp->p.accept('[')) {
					
					if (tp->p.close_bracket("[]", &buffer)) {
							
						buffer += ";";
						bc_tp.push_back("type list");
						tp->init("list", &bc_tp, tp->l, &buffer, 0);
						buffer = "";
					}
					else tp->error(el, ep, "Expected ']' to end list.");
				}
				else if (tp->p.accept('&')) {
					
					if (!tp->p.await(";,\n", &buffer)) tp->error(el, ep, "Expected ';' to end path value.");
						
					if (buffer.size()) {
						
						tp->init("referance", tp->bc, tp->l, &buffer, 0);
						buffer = "";
					}
				}
				else {
					
					if (!tp->p.await(";,\n", &buffer)) tp->error(el, ep, "Expected ';' to end path value.");
						
					if (buffer.size()) {
						
						tp->init("single", &bc_tp, tp->l, &buffer, 0);
						buffer = "";
					}
				}
				
				if (bc_tp.size()) {
					
					tp->push("down " + std::to_string(iteration));
					tp->bc->insert(tp->bc->end(), bc_tp.begin(), bc_tp.end());
					tp->push("back");
					bc_tp.clear();
					
					iteration++;
				}
			}
		}
		
		void referance(type_pack* tp) {
			
			tp->log(vl::info, "[INIT] Referance : " + *tp->p.source);
			
			std::string buffer = "";
			std::string path = "";
			
			while (!tp->p.end || tp->p.accept(";,\n")) {
				
				if (tp->p.accept("/\\.")) {
					
					if (buffer.size()) {
						
						path += " " + buffer;
						buffer = "";
					}
				}
				else if (!tp->p.accept("`w_-", &buffer)) tp->p.next();
			}
			
			if (buffer.size()) {
				
				path += " " + buffer;
				buffer = "";
			}
			
			if (path.size()) {
				
				tp->push("ref" + path);
			}
		}
		
		void single(type_pack* tp) {
			
			tp->log(vl::info, "[INIT] Single : " + *tp->p.source);
			
			std::string buffer = "";
			std::string ntype = "";
						
			if (tp->p.accept("`d", &buffer)) {
				
				while (!tp->p.end) {
					
					tp->p.chain("`s");
					
					if (tp->p.accept("`d")) {
						
						if (!ntype.size()) {
							
							buffer += tp->p.sym;
						}
						else tp->init("string", tp->bc, tp->l, tp->p.source, 0);
					}
					else if (!tp->p.accept("`a", &ntype)) tp->p.next();
				}
				
				tp->push("type number");
				tp->push("set " + buffer);
				
				if (ntype.size()) {
					
					tp->push("down suffix");
					tp->init("string", tp->bc, tp->l, &ntype, 0);
					tp->push("back");
				}
			}
			else if (tp->p.accept("`a./\\_-", &buffer)) {
				
				tp->init("referance", tp->bc, tp->l, tp->p.source, 0);
			}
		}
	}
	
	namespace handler_functions {
		
		void down(handler_pack* hp) {
			
			if (!hp->args.size()) return;
			
			hp->push(hp->current_node);
			
			for (size_t i = 0; i < hp->args.size(); i++) {
				
				node* target = hp->current_node->get_child(hp->args[i]);
				
				if (target == nullptr) {
					
					target = hp->current_node->add_child(hp->args[i]);
				}
				
				hp->set(target);
			}
		}
		
		void back(handler_pack* hp) {
			
			if (hp->previous_nodes->size()) {
				
				hp->set(hp->previous_nodes->back());
				hp->pop();
			}
			else hp->error("No node in previous node stack to go back too.");
		}
		
		void set(handler_pack* hp) {
			
			if (!hp->args.size()) return;
			
			try {
				
				hp->current_node->value = std::stoi(hp->args[0]);
			}
			catch (const std::invalid_argument& e) {
				
				hp->error("\"Invalid Argument\" in \"set " + hp->args[0] + "\"\n");
			}
		}
		
		void type(handler_pack* hp) {
			
			if (!hp->args.size()) return;
			
			hp->current_node->type = hp->args[0];
		}
		
		void ref(handler_pack* hp) {
			
			if (!hp->args.size()) return;
			
			if (!hp->post) {
				
				hp->r->post("ref", hp);
				return;
			}
			
			node* last_target = hp->current_node;
			node* target = hp->current_node;
			
			while (target->name != hp->args[0]) {
				
				last_target = target;
				
				if (target->parent != nullptr) {
					
					target = target->parent->get_child(hp->args[0]);
				}
				else break;
				
				if (target == nullptr) break;
			}
			
			std::string path = "";
			
			if (target != nullptr && target->name == hp->args[0]) {
				
				path = target->name;
				
				for (size_t i = 1; i < hp->args.size(); i++) {
				
					path += " " + hp->args[i];
					target = target->get_child(hp->args[i]);
					
					if (target == nullptr) break;
				}
			}
			
			if (target == nullptr) {
				
				hp->error("Failed to find node at \"" + path + "\".");
				return;
			}
			
			hp->current_node->value = target->value;
			hp->current_node->type = target->type;
			
			if (target->child != nullptr) {
				
				hp->current_node->child = target->child;
				hp->current_node->child->parent = hp->current_node;
			}
		}
	}
	
	void std(runtime* r) {
		
		r->handlers["down"] = handler_functions::down;
		r->handlers["back"] = handler_functions::back;
		r->handlers["set"] = handler_functions::set;
		r->handlers["type"] = handler_functions::type;
		r->handlers["ref"] = handler_functions::ref;
		
		r->types["collection"] = type_functions::collection;
		r->types["string"] = type_functions::string;
		r->types["list"] = type_functions::list;
		r->types["referance"] = type_functions::referance;
		r->types["single"] = type_functions::single;
	}
}