//=================================
// include guard
#ifndef NODE_BURST_H
#define NODE_BURST_H

//=================================
// included dependencies
#include "headers.hpp"
#include "node.hpp"
#include "global_params.hpp"

//=================================
//int rsim_params::total_injected_packets_chip;

class Node_burst : public Node {
private:
	int cnt_on;
	int cnt_off;
	float a;
	float bon;
	float boff;
	int seed;
  	std::default_random_engine generator; // construct a trivial random generator engine from a time-based seed
	std::uniform_real_distribution<double> distribution; // uniform random generator between 0 and 1
public:
	Node_burst(int nid, float node_inj_rate);
	void check_if_injection();
	void update_cnt_on(); // calculate when the next packet will be injected
	void update_cnt_off(); // calculate when the next packet will be injected
	int get_cnt_on();
	int get_cnt_off();
	void decrease_cnt_on();
	void decrease_cnt_off();
};

#endif // NODE_BURST_H
