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
	int pos;
	int token_lenght;
	int token_current_node;
	std::vector<int> ids_concurrent_tx_nodes;
	std::vector<int> ids_nodes_vector; //Tots els node que volen transmetre pel canal

public:
	Channel(int id);
	int get_channel_id();
	void set_token_lenght(int);

	int get_token_current_node();
	void update_token_current_node(int);
	void set_token_current_node(int);

	int get_pos();
	void set_pos(int);

	int get_ids_concurrent_tx_nodes_size();
	void push_ids_concurrent_tx_nodes(int);
	void flush_ids_concurrent_tx_nodes();
	int get_unique_kids_concurrent_tx_nodes();
	std::vector<int>::const_iterator ids_concurrent_tx_nodes_begin();
	std::vector<int>::const_iterator ids_concurrent_tx_nodes_end();

	int get_nodes_vector_size();
	void push_nodes_vector(int);
	std::vector<int>  get_nodes_vector();

};

#endif
