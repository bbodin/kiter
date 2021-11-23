/*
 * buffersizing.h
 *
 *  Created on: Feb 25, 2021
 *      Author: toky
 */

#ifndef SRC_LIBKITER_ALGORITHMS_BUFFERSIZING_H_
#define SRC_LIBKITER_ALGORITHMS_BUFFERSIZING_H_

#include <commons/basic_types.h>
#include <functional>

namespace models {
class Dataflow;
}

class BufferSizingResult {

	bool       _valid;
	TOKEN_UNIT _total_size;
	std::map<ARRAY_INDEX, double> _idv_size;
public:
	BufferSizingResult(TOKEN_UNIT t) : _valid (true), _total_size(t) {}
	BufferSizingResult() : _valid (false), _total_size(0) {}
	bool is_valid() const {return _valid;}
	TOKEN_UNIT total_size() const {return _total_size;}
	void set_validity(bool validity){_valid = validity;}
	void set_total_size(TOKEN_UNIT token){_total_size = token;}
	void add_edge_size(ARRAY_INDEX edge, double size) {_idv_size.insert({edge, size});}
	double get_edge_size (ARRAY_INDEX edge) {return _idv_size[edge];}
};

typedef std::function< BufferSizingResult(models::Dataflow* const, TIME_UNIT) > BufferSizingFun;

#endif /* SRC_LIBKITER_ALGORITHMS_BUFFERSIZING_H_ */
