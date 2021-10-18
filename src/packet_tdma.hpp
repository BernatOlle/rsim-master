//=================================
// include guard
#ifndef PACKET_TDMA
#define PACKET_TDMA

//=================================
// included dependencies
#include "headers.hpp"
#include "packet.hpp"
#include "global_params.hpp"

//=================================
class Packet_tdma : public Packet {
private:
	int cycles_left; // This is initially set to tx_time, and decreased at every cycle during successful transmission
public:
	Packet_tdma(int inj_time, int src_node);
	int get_cycles_left();
	void decrease_cycles_left();
};


#endif // PACKET_TDMA
