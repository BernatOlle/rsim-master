#ifndef channel
#define channel

#include "headers.hpp"
#include "global_params.hpp"

class channel : public Packet {
private:
	int id;
	int nchannels;
public:
    channel(int inj_time, int src_node);
	int get_channel_Id();
	void set_channel_Id();
};




#endif // channel
