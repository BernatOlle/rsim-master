//=================================
// include guard
#ifndef PACKET_BRS_NON_P_H
#define PACKET_BRS_NON_P_H

//=================================
// included dependencies
#include "headers.hpp"
#include "packet.hpp"
#include "global_params.hpp"

//=================================
class Packet_brs_non_p : public Packet {
private:
	int backoff_exp; // exponent of the backoff (backoff = 2^backoff_exp). This is increased every time a collision occur
	int cnt_backoff;
	int cycles_left; // This is initially set to tx_time, and decreased at every cycle during successful transmission
	bool header_sent; // this is to check when the header is sent
	Channel channel;
public:
	Packet_brs_non_p(int inj_time, int src_node);
	int get_cycles_left();
	bool is_header_sent();
	void set_header_sent();
	void reset_cycles_left();
	void decrease_cycles_left();
	int get_backoff_exp();
	int get_cnt_backoff();
	int get_channelID();
	void decrease_cnt_backoff(); // Decrease cnt_backoff by 1
	void reset_backoff_exp();
	void update_cnt_backoff(); // Increase backoff_exp and choose randomly new backoff between 1 and 2^backoff_exp
	void set_cnt_backoff_zero();
};


#endif // PACKET_BRS_NON_P_H
