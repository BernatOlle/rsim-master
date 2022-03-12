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
	int token_lenght;
	int token_current_node;
	std::vector<int> ids_concurrent_tx_nodes;

public:
	Channel(int id);
	int get_channel_id();
	void set_token_lenght(int);

	int get_token_current_node();
	void update_token_current_node();
	void set_token_current_node();

	int get_ids_concurrent_tx_nodes_size();
	void push_ids_concurrent_tx_nodes(int);
	void flush_ids_concurrent_tx_nodes();
	int get_unique_kids_concurrent_tx_nodes();
	std::vector<int>::const_iterator ids_concurrent_tx_nodes_begin();
	std::vector<int>::const_iterator ids_concurrent_tx_nodes_end();

};

#endif
