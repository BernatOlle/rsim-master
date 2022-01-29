//=================================
// include guard
#ifndef NODE_H
#define NODE_H

//=================================
// included dependencies
#include "headers.hpp"
#include "packet.hpp"
#include "global_params.hpp"
#include "packet_csma_non_p.hpp"
#include "packet_brs_non_p.hpp"
#include "packet_tdma.hpp"

//=================================
//int rsim_params::total_served_packets_chip;

class Node {
protected:
	int nid; // node ID
	int channelID;
	float node_inj_rate; // average injection rate for this node
	std::queue<Packet*> in_buffer; // each node has an input buffer of max_buffer_size for packets
	int total_injected_packets_node; // counter of how many packets each node has injected so far
	int total_served_packets_node; // counter of how many packets each node has served (correctly transmitted) so far
	int seed_fuzzy_weights;
	std::default_random_engine generator_fuzzy_weights;
	std::uniform_real_distribution<double> distribution_fuzzy_weights; // we prepare a uniform distribution of real numbers between 0 and 1
public:
	Node(int id, float inj_rate); // declare constructor of each node
	Node(int id); // declare constructor of each node (for those node types that don't have injection rate, like a trace)
	std::vector<double> pckt_latencies_node; // TODO: THIS SHOULD BE PRIVATE. here we store the cycles that each packet it takes to be transmitted
	int get_id(); // get node ID
	float get_node_inj_rate();
	int get_channel_id();
	void set_channel_id(int);
	void channel_function(std::string protocol, std::string step,int nchannels, int reason, int assig, int channel_id_gravity);
	int get_in_buffer_size();
	bool in_buffer_empty();
	Packet* get_in_buffer_front();
	void push_new_packet(int curr_cycle, int nid);
	virtual void check_if_injection() = 0; // check_if_injection is a pure virtual function, so Node is an abstract base class
	double get_randomly_uniform_value(); // pick a random number (uniformly distributed) between 0 and 1
	int get_total_injected_packets_node();
	int get_total_served_packets_node();
	double get_mean_stationary_pckt_latencies_node(); // mean of the packet latencies for this node in the stationary range
	int get_total_stationary_packets_node(); // number of packets that fall within the stationary period
	void pop_packet_buffer(int curr_cycle); // when a packet has been successfully transmitted, update the statistics
	void plot_pckt_latencies_node(); // plot distribution of packet latencies por this node
};

#endif // NODE_H
