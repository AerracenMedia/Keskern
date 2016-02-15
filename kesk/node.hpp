#pragma once

#include <string>

namespace kesk {
	
	struct node {
	
		node(const std::string& n, node* p = nullptr, std::string t = "null", int v = 0, node* c = nullptr, node* s = nullptr)
			: name(n), parent(p), type(std::string(t)), value(v), child(c), sibling(s)
		{ }
		
		const std::string name;
		std::string type;
		
		node* sibling;
		node* parent;
		node* child;
		
		int value;
		
		node* get_child(const std::string& n) {
			
			if(child == nullptr) return nullptr;
			
			if (child->name == n) {
				
				return child;
			}
			else return child->get_sibling(n);
		}
		
		node* get_sibling(const std::string& n) {
			
			if (sibling == nullptr) return nullptr;
			
			if (sibling->name == n) {
				
				return sibling;
			}
			else return sibling->get_sibling(n);
		}
		
		node* add_child(const std::string& n) {
			
			if (child == nullptr) {
				
				child = new node(n, this);
				return child;
			}
			else {
				
				if (child->name == n) {
					
					return child;
				}
				else return child->add_sibling(n);
			}
		}
		
		node* add_sibling(const std::string& n) {
			
			if (sibling == nullptr) {
				
				sibling = new node(n, parent);
				return sibling;
			}
			else {
				
				if (sibling->name == n) {
					
					return sibling;
				}
				else return sibling->add_sibling(n);
			}
		}
	};
}