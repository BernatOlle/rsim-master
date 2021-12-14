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

class Channel {
protected:
	int cid;
	std::vector<int> ids_concurrent_tx_nodes;

public: 
	int get_channel_id();
	int get_ids_concurrent_tx_nodes();
	void push_ids_concurrent_tx_nodes(int);
	void flush_ids_concurrent_tx_nodes();

}