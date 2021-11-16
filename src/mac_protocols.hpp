//=================================
// We get an int vector with the IDs of the nodes that have
// packets to transmit. Each protocol will specify how to deal
// with them.

//=================================
// include guard
#ifndef MAC_PROTOCOLS_H
#define MAC_PROTOCOLS_H

//=================================
// included dependencies
#include "headers.hpp"
#include "node.hpp"

//=================================
// Specification of CSMA non-persistant
void protocol_csma_non_p(int curr_cycle, const std::vector<int>& nodes_ready, std::vector<Node*>& chip, int nchannels);

// Specification of BRS-MAC non-persistent
int protocol_brs_non_p(int curr_cycle, const std::vector<int>& nodes_ready, std::vector<Node*>& chip, int nchannels);

// Specification of TDMA for both fixed and weighted schemes
void protocol_tdma(int curr_cycle, const std::vector<int>& nodes_ready, std::vector<Node*>& chip, std::vector<float>& hotspotness_weights, int nchannels);

// Specification of Fuzzy Token
void protocol_fuzzy_token(int curr_cycle, const std::vector<int>& nodes_ready, std::vector<Node*>& chip, std::vector<float>& hotspotness_weights, int nchannels);

// Specification of Token
void protocol_token(int curr_cycle, const std::vector<int>& nodes_ready, std::vector<Node*>& chip, std::vector<float>& hotspotness_weights, int nchannels);


#endif // MAC_PROTOCOLS_H
