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

void Channel::push_ids_concurrent_tx_nodes(int nid){
	ids_concurrent_tx_nodes.push_back(nid);
}

void Channel::flush_ids_concurrent_tx_nodes(){
	ids_concurrent_tx_nodes.erase(ids_concurrent_tx_nodes.begin(), ids_concurrent_tx_nodes.end());
}

int Channel::get_unique_kids_concurrent_tx_nodes() {
    if (ids_concurrent_tx_nodes.size() == 1) {
        return ids_concurrent_tx_nodes.front();
    }
    else {
        std::cout << "ERROR: none or multiple concurrent tx nodes still in vector, but you are asking for only one" << std::endl;
        abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
    }
}

std::vector<int>::const_iterator Channel::ids_concurrent_tx_nodes_begin() {
    return ids_concurrent_tx_nodes.begin();
}

std::vector<int>::const_iterator Channel::ids_concurrent_tx_nodes_end() {
    return ids_concurrent_tx_nodes.end();
}