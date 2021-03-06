#include "node.hpp"
#include "utilities.hpp"
#include <cstdlib>
#include <vector>
#include <algorithm>

Node::Node(int id, float inj_rate) : nid(id),channelID(-1), node_inj_rate(inj_rate), total_injected_packets_node(0), total_served_packets_node(0) {
    seed_fuzzy_weights = std::chrono::system_clock::now().time_since_epoch().count();
    generator_fuzzy_weights.seed(seed_fuzzy_weights);
	distribution_fuzzy_weights.param(std::uniform_real_distribution<double>(0.0,1.0).param());
}

Node::Node(int id) : nid(id), channelID(-1), total_injected_packets_node(0), total_served_packets_node(0) {
    seed_fuzzy_weights = std::chrono::system_clock::now().time_since_epoch().count();
    generator_fuzzy_weights.seed(seed_fuzzy_weights);
    distribution_fuzzy_weights.param(std::uniform_real_distribution<double>(0.0, 1.0).param());
}

int Node::get_id() {
    return nid;
}

int Node::get_in_buffer_size() {
    return in_buffer.size();
}

float Node::get_node_inj_rate() {
    return node_inj_rate;
}

int Node::get_channel_id() {
    return channelID;
}

void Node::set_channel_id(int channel_id) {
    Node::channelID = channel_id;
}

std::vector<int> Node::get_channel_array(){
  return channel_array;

}

void Node::set_channel_array(int x){

  Node::channel_array.push_back(x);
}

int Node::get_channel_node(int x){
  return Node::channel_array[x];
}

void Node::set_prob_channel_array(long double x){
  Node::prob_channel_array.push_back(x);
}

 std::vector<long double> Node::get_prob_channel_array(){
  return prob_channel_array;
}

long double Node::get_prob_channel_node(int x){
  return prob_channel_array[x];
}

// function to show the change of vector values and reasons
void Node::channel_function(std::string protocol, std::string step, int number_channels, int reason, int assig, int channel_id_gravity) {
    // TODO can we recuperate the node as so ?

    int nodeId = this->get_id();
    int channelId =this->get_channel_id();
    //std::cout<<"Channel initial:" << channelId<<std::endl;
    int new_channelId = channelId;
    //std::cout<<"Assig :: "<<assig<<std::endl;
    if (protocol =="brs"){
     if(assig == 1){
        if (channelId == -1) {
            new_channelId = rand() % number_channels;
            Node::set_channel_id(new_channelId);
            //std::cout << "Protocol:" << protocol << "Step:" << step << "Node: " << nodeId << "ChannelID:"
                      //<< new_channelId
                      //<< "\n";
        }else if(step=="colision"){
            while(new_channelId==channelId){
                new_channelId = rand() % number_channels;

            }
            //std::cout<<"New channel id = "<<new_channelId<<std::endl;
            Node::set_channel_id(new_channelId);
        }
        //std::cout<<"Channel initial:" << new_channelId<<std::endl;
        //std::cout << "Protocol:" << protocol << "Step:" << step << "Node: " << nodeId << "ChannelID:" << channelId
                  //<< "\n";
  } if(assig == 2){
  //std::cout<<"hola"<<std::endl;
            if (channelId == -1) {
        new_channelId=0;
        Node::set_channel_id(new_channelId);
        //std::cout << "New Channel id: "<< new_channelId<< std::endl;

    }else if (step=="colision"){
        new_channelId=channelId+1;
        if(new_channelId>=number_channels){
          new_channelId=number_channels-1;
        }
        //std::cout << "New Channel id: "<< new_channelId<< std::endl;
        Node::set_channel_id(new_channelId);

    }



}else if(assig == 3){

  //std::cout<<"AAAAAAAAAA  "<< this->get_channel_node(ind)<<std::endl;
  //
  int i=0;

  std::vector<long double> v= this->get_prob_channel_array();
  std::vector<long double> v_real;
  float suma =0;
  //std::cout << "Size: " << v.size()<<'\n';
  for(int h=0;h<v.size();h++){
    suma= suma + v[h];
    //std::cout << " "<< this->get_prob_channel_array()[h];
  }
//std::cout<<"\n";
if(suma == 0){
  i=0;

}else{
  float suma_tot=0;
  for(int h=0;h<v.size();h++){
    suma_tot = suma_tot + (v[h]/(suma))*100;
    v_real.push_back(suma_tot);
    //std::cout << " "<< v_real[h];


  }

//std::cout<<"\n";
  float ind = rand()%100;
  float o=v_real[0];
  //std::cout <<"Ind: "<<ind;
  //std::cout << " V_real: " << o<<'\n';
  while(ind>=o){
    i++;
    //std::cout << "+++++++++++++++++++++++++++++++++++++++++++++O: "<<v_real[i-1] << '\n';
    o=v_real[i];

  }
  //std::cout<<"IND  "<<std::endl;
//std::cout <<"Node ID: "<<nodeId<<"  Channel assignment: "<< this->get_channel_node(i) << '\n';
}
//std::cout << "????????????????????????????????????????????????????????????????????????????????????????????" << '\n';
if (channelId == -1) {
//std::cout <<"AAA " << '\n';

Node::set_channel_id(this->get_channel_node(i));

}else if (step=="colision"){
  Node::set_channel_id(this->get_channel_node(i));
}



    //std::cout<<"new channel"<<channel_id_gravity<<"Node id: "<<nodeId<<std::endl;
  //Node::set_channel_id(channel_id_gravity);
//}

}// assig 3
}//brs
}



bool Node::in_buffer_empty() {
    return in_buffer.empty();
}

Packet *Node::get_in_buffer_front() {
    return in_buffer.front();
}

int Node::get_total_injected_packets_node() {
    return total_injected_packets_node;
}

double Node::get_randomly_uniform_value() {
    double value = distribution_fuzzy_weights(generator_fuzzy_weights);
    return value;
}

int Node::get_total_served_packets_node() {
    return total_served_packets_node;
}

double Node::get_mean_stationary_pckt_latencies_node() {
    std::vector<double> stationary_pckt_latencies_node;
    int size_vector = pckt_latencies_node.size();
    int i = 0;
    if (size_vector > 0) {
        for (auto &element: pckt_latencies_node) {
            // we discard the statistics of the warm-up and cool-down packets
            if (i >= floor(0.1 * size_vector) && i < ceil(0.8 * size_vector)) {
                stationary_pckt_latencies_node.push_back(element);
            }
            i++;
        }
    } else {
        // std::cout << "!!! ERROR: Packet latencies vector per node shouldn't be empty" << std::endl;
        // abort(); // TODO: THIS IS NOT THE RIGHT WAY TO EXIT A PROGRAM. USE EXCEPTIONS OR JUST ERROR CODES
        return 0;
    }
    // return geometric_mean(stationary_pckt_latencies_node); // we do the geometric mean (this showed avg latency smaller than expected)
    return arithmetic_mean(stationary_pckt_latencies_node); // we do the arithmetic mean
}

int Node::get_total_stationary_packets_node() {
    int size_vector = pckt_latencies_node.size();
    return (ceil(0.8 * size_vector) - floor(0.1 * size_vector));
}

void Node::push_new_packet(int curr_cycle, int nid) {
    switch(Global_params::Instance()->get_chosen_mac()) {
        case Mac_protocols::csma_non_p      : in_buffer.push(new Packet_csma_non_p(curr_cycle, nid)); break;
        case Mac_protocols::brs_non_p       : in_buffer.push(new Packet_brs_non_p(curr_cycle, nid)); break;
        case Mac_protocols::tdma_fixed      : in_buffer.push(new Packet_tdma(curr_cycle, nid)); break;
        case Mac_protocols::tdma_weighted   : in_buffer.push(new Packet_tdma(curr_cycle, nid)); break;
        case Mac_protocols::fuzzy_token     : in_buffer.push(new Packet_brs_non_p(curr_cycle, nid)); break;
        case Mac_protocols::token           : in_buffer.push(new Packet_tdma(curr_cycle, nid)); break;
    }

    if (Global_params::Instance()->is_debugging_on()) {
        std::cout << "Node " << nid << " injected a packet" << std::endl;
    }
}

void Node::pop_packet_buffer(int curr_cycle) { // when a packet has been successfully transmitted, update the statistics
    Global_params::Instance()->pckt_latencies_chip.push_back(curr_cycle - get_in_buffer_front()->get_inj_time() + 1); // we save the latency for every tx packet into the chip vector
    pckt_latencies_node.push_back(curr_cycle - get_in_buffer_front()->get_inj_time() + 1); // we save the latency for every tx packet into this particular node vector
    total_served_packets_node++;
    Global_params::Instance()->increase_total_served_packets_chip();
    in_buffer.pop();
}

void Node::plot_pckt_latencies_node() {
    const int nstars = 100;    // maximum number of stars to distribute
    const int nintervals = 10; // number of intervals

    int p[nintervals] = {};
    double max_latency = *(std::max_element(pckt_latencies_node.begin(), pckt_latencies_node.end()));
    double min_latency = *(std::min_element(pckt_latencies_node.begin(), pckt_latencies_node.end()));
    double interval_size = (max_latency - min_latency) / (double) nintervals;

    for (const double &element: pckt_latencies_node) {
        if (element >= min_latency && element < min_latency + interval_size) ++p[0];
        else if (element >= min_latency + interval_size && element < min_latency + 2 * interval_size) ++p[1];
        else if (element >= min_latency + 2 * interval_size && element < min_latency + 3 * interval_size) ++p[2];
        else if (element >= min_latency + 3 * interval_size && element < min_latency + 4 * interval_size) ++p[3];
        else if (element >= min_latency + 4 * interval_size && element < min_latency + 5 * interval_size) ++p[4];
        else if (element >= min_latency + 5 * interval_size && element < min_latency + 6 * interval_size) ++p[5];
        else if (element >= min_latency + 6 * interval_size && element < min_latency + 7 * interval_size) ++p[6];
        else if (element >= min_latency + 7 * interval_size && element < min_latency + 8 * interval_size) ++p[7];
        else if (element >= min_latency + 8 * interval_size && element < min_latency + 9 * interval_size) ++p[8];
        else if (element >= min_latency + 9 * interval_size && element <= max_latency) ++p[9];
    }

    for (int i = 0; i < nintervals; ++i) {
        std::cout << min_latency + i * interval_size << "-" << min_latency + (i + 1) * interval_size << ": ";
        std::cout << std::string(p[i] * nstars / pckt_latencies_node.size(), '*') << std::endl;
    }
}
