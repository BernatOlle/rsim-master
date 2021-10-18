//=================================
// include guard
#ifndef NODE_POISSON_H
#define NODE_POISSON_H

//=================================
// included dependencies
#include "headers.hpp"
#include "node.hpp"

//=================================
//int rsim_params::total_injected_packets_chip;

class Node_poisson : public Node {
private:
	int cnt_next_packet; // number of cycles left until injection of new packet for this node
	int seed;
  	std::default_random_engine generator; // construct a trivial random generator engine from a time-based seed
	std::exponential_distribution<double> inj_distribution; // we prepare the exponential distribution according to our mean
public:
	Node_poisson(int nid, float node_inj_rate);
	void update_cnt_next_packet(); // calculate when the next packet will be injected
	int get_cnt_next_packet();
	void decrease_cnt_next_packet();
	void check_if_injection();

};

#endif // NODE_POISSON_H
