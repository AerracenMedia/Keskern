#pragma once

#include "kesk/logger.hpp"
#include "kesk/node.hpp"
#include "kesk/parser.hpp"
#include "kesk/runtime.hpp"
#include "kesk/std.hpp"

kesk::runtime* keskern(unsigned char vrb = 1, bool compile_stdos = false, bool compile_stdes = true, bool runtime_stdos = false, bool runtime_stdes = true) {
	
	kesk::runtime* r = new kesk::runtime();
	
	r->allow_compile_stdos = compile_stdos;
	r->allow_compile_stdes = compile_stdes;
	
	r->allow_runtime_stdos = runtime_stdos;
	r->allow_runtime_stdes = runtime_stdes;
	
	r->verbosity = vrb;
	
	kesk::std(r);
	
	return r;
}

kesk::runtime* keskern(kesk::vl v, bool compile_stdos = false, bool compile_stdes = true, bool runtime_stdos = false, bool runtime_stdes = true) {
	
	return keskern((unsigned char)v, compile_stdos, compile_stdes, runtime_stdos, runtime_stdes);	
}