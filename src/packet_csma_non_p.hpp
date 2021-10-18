//=================================
// include guard
#ifndef PACKET_CSMA_NON_P_H
#define PACKET_CSMA_NON_P_H

//=================================
// included dependencies
#include "headers.hpp"
#include "packet.hpp"

//=================================
class Packet_csma_non_p : public Packet {
private:
	int backoff_exp; // exponent of the backoff (backoff = 2^backoff_exp). This is increased every time a collision occur
	int cnt_backoff;
public:
	Packet_csma_non_p(int inj_time, int src_node);
	int get_backoff_exp();
	int get_cnt_backoff();
	void decrease_cnt_backoff();
	void increase_backoff_exp();
	void reset_backoff_exp();
	void update_cnt_backoff(); // Choose randomly a new backoff time between 1 and 2^backoff_exp
};


#endif // PACKET_CSMA_NON_P_H
