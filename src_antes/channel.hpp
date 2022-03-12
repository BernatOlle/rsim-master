//=================================
// include guard
#ifndef CHANNEL_H
#define CHANNEL_H
//=================================
// included dependencies
#include "headers.hpp"
#include "global_params.hpp"
//=================================
//int rsim_params::total_served_packets_chip;

class Channel {
protected:
	int cid;
	std::vector<int> ids_concurrent_tx_nodes;

public: 
	Channel(int id);
	int get_channel_id();
	int get_ids_concurrent_tx_nodes_size();
	void push_ids_concurrent_tx_nodes(int);
	void flush_ids_concurrent_tx_nodes();
	int get_unique_kids_concurrent_tx_nodes();
	std::vector<int>::const_iterator ids_concurrent_tx_nodes_begin();
	std::vector<int>::const_iterator ids_concurrent_tx_nodes_end();

};

#endif