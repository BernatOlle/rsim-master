//=================================
// include guard
#ifndef NODE_TRACE_H
#define NODE_TRACE_H

//=================================
// included dependencies
#include "headers.hpp"
#include "node.hpp"

//=================================
//int rsim_params::total_injected_packets_chip;

class Node_trace : public Node {
public:
	Node_trace(int nid);
	void check_if_injection();
};

#endif // NODE_TRACE_H
