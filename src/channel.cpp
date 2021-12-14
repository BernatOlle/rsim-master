#include "channel.hpp"
#include "utilities.hpp"
#include <cstdlib>
#include <vector>
#include <algorithm>


Channel::Channel(int id): cid(id){
	std::vector<int> ids_concurrent_tx_nodes;
}


int Channel::get_channel_id(){
	return cid;
}

int Channel::get_ids_concurrent_tx_nodes_size(){
	return ids_concurrent_tx_nodes.size();

}

void Channel::push_ids_concurrent_tx_nodes(int){
	ids_concurrent_tx_nodes.push_back(nid);
}

void Channel::flush_ids_concurrent_tx_nodes(){
	ids_concurrent_tx_nodes.erase(ids_concurrent_tx_nodes.begin(), ids_concurrent_tx_nodes.end());
}