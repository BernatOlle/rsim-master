#include "packet_tdma.hpp"

Packet_tdma::Packet_tdma(int inj_time, int src_node) : Packet(inj_time, src_node) {
	cycles_left = Global_params::Instance()->get_tx_time();
}

int Packet_tdma::get_cycles_left() {
	return cycles_left;
}

void Packet_tdma::decrease_cycles_left() {
	cycles_left--;
}
