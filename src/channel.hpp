#ifndef channel
#define channel

#include "headers.hpp"
#include "global_params.hpp"

class Channel : public Packet {
private:
	int id;
	int num;
public:
    channel(int id);
	int get_channel_Id();
	void set_channel_Id(int);
	int get_nchannel();
	int channel_function(protocol(string),step(string),node(int),packet(class packet);
};


node(int

#endif // channel
