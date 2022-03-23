#include "channel.hpp"
#include "utilities.hpp"
#include <cstdlib>
#include <vector>
#include <algorithm>


Channel::Channel(int id): cid(id){
	std::vector<int> ids_concurrent_tx_nodes;
}

void Channel::update_token_current_node(int assig) {
	if(assig == 1){
    token_current_node=(token_lenght*cid)+(token_current_node+1)%token_lenght;

	}
	if(assig == 2||assig == 3){
		token_current_node=	(token_current_node+1)%token_lenght;
	
	}
}

void Channel::set_token_current_node(int assig) { // Initialize the position of the first token
	if(assig == 1){
    token_current_node = token_lenght*cid;
		}
		if(assig == 2||assig == 3){
			int nodes_token = Global_params::Instance()->get_ncores();
			int number_channels=Global_params::Instance()->get_nchannels();
			int NxC = ceil(nodes_token/number_channels);
			token_current_node=cid*NxC;
		}
}

int Channel::get_token_current_node() {
    return token_current_node;
}

int Channel::get_channel_id(){
	return cid;
}

void Channel::set_token_lenght(int NxC){
	token_lenght = NxC;
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
