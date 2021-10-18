# RSim
Replica Simulator. Network simulator for the Replica project.

## Update: September 2021

General tips for any new user to RSim:

1. Documentation in the `README.md` is outdated, since it was modified 5 years ago for the last time, even though we modified the code 2 years ago
2. Comments in the code (files in the `rsim/src/` folder should have comments, and those should be up to date)
3. There are no python scripts to plot results, but the python files we had (such as `rsim/parse_results.py`) are used to convert the output files in your chosen results folder (in our case it was `rsim/results/`) into `.csv` files. Then I would use those csv files to plot the results manually with Excel
4. The python script `rsim/parse_results.py` takes the results folder path as an argument, meaning you need to run `python rsim/parse_results.py -r <results_path>`. I think the script needs to run with Python2 instead of Python3. But I'm not 100% sure on that. The script tries to convert results for throughput, latency, and energy. Each of these results types should be in a subfolder inside your <results_path>, with names `<results_path>/data_throughput`, `<results_path>/data_latencies`, and `<results_path>/data_energy`, respectively.


---------------------------------------------------------------------------------------------

## Update: January 2017

(this document is intended to gather all thoughts and comments I get during the implementation of the simulator, and is not yet recommended for public audience. Also, it has not been revised in a while, and the content might not reflect the improvement on more recent versions of the simulator)

----------------------------------------------------------------------------------------------

Features and glossary of RSim:
- it detects when buffers are full, so that packets are lost
- when a new packet is injected it means that it's inserted into the buffer of the corresponding node. The interval between injections is calculated according the injection distribution.
- it calculates the average packet latency per node as a geometric mean. And also the avg packet latency per chip as a geometric mean of all avg_latency_node
- it plots a distribution of the packet latencies per node, by calling plot_pckt_latencies_node
- take a look at the manual of booksim and you'll see that i'm simulating exactly same stuff, like injection rate, type of injection, parameters of injection... So this is good, because it means i'm doing the same than the well-known simulators.
- input parameters can be either obtained from shell or from parameters.ini, if we don't detect input from shell we get it from ini
- parameter debugging=true sets debugging mode ON, where user can see step by step what the simulation does


----------------------------------------------------------------------------------------------

Limitations of RSim:
-

----------------------------------------------------------------------------------------------

Observations of RSim:
- For all fixed input parameters, only varying ncores, for CSMA/CA non-persistant, the number of packets that reach the max backoff size are more, because more nodes are trying to contend at the same time
- by using the plot_pckt_latencies_node we observe some eventual latencies of up to thousands of cycles. We should decide what do we do when packets take so long to transmit. And what mac protocol avoids this long-latency packets. So when we compare different mac protocols, we not only have to take into consideration the average latency, but also the mean, standard deviation, max and min of the latencies.
- contrarily to what intuitively one could think, weighted tdma incurs into worse average packet delay, since it spends most of its time solving the hotspot, but it leaves the non-hotspot nodes starving, which notoriously increases their latency

----------------------------------------------------------------------------------------------

For the paper/report:

Do as in the paper of Abadal, or synthethic traffic, explain each concept of the model, like the injection distribution model, the injection rate, the hurst exponent, the hotspotness...

Maybe put a plot of how the fuzzy size of the fuzzy token oscillates in time, to see if there is some kind of trend

explain that when designing fuzzy token, the two ideas were that the node with the token would either set its backoff to zero, or to make everybody wait for a cycle and the one with the token transmit immediately. The first option didn't give good results, whereas the second did, so we chose the second. Actually, there was another option that we didn't consider, which was to give equal transmission probability to all the nodes in the fuzzy area, including the one with the token, but obviously this didn't work either, because it was just giving slightly better results than brs en el primer, tots els nodes que estaben dins a l'area del fuzzy token tenien la mateixa probabilitat de transmetre, incluit el que te el token                        
 aixo ha donat mals resultats, ja que gairebe obtenia resultats similars a brs                        
una mica millor, pero no massa                        
en el segon disseny he considerat que el que te el token, per donar-li preferencia, si te paquets per transmetre, posa el backoff a zero, aixi intentara transmetre inmediatament                        
aixo tampoc ha funcionat en el cas de 1000 nodes, perque el fuzzy tenia una area d'uns 400 nodes i aixo feia que la probabilitat de colisio amb altres nodes que just havien arrivat a backoff=0 era alta                        
la tercera opcio ha sigut que tots els nodes escolten durant el cicle inmediatament posterior a haver passat el token (tots ho saben que s'ha passat el token perque l'enviem per wireless), i durant aquest cicle, el que te el token aprofita per transmetre directament (en cas que tingui paquets), i si no te cap paquet, senzillament deixa perdre un cicle                        
si passat aquest cicle els nodes no han escoltat res, tots fan el mecanisme de backoff habitual                        
 i passats tx_time+1 cicles, el token es passa                        
es a dir, el token sempre te una duracio de tx_time+2 cicles                        
tx_time perque es lo minim per a que algu transmeti                        
+1 perque el brs te un cicle intermig per asseguar que no hi hagi colisions                        
i +1 per el cicle aquest que tots esperen al principi del token slot

explain that the results are obtained by doing the geometric mean of several executions, and that the average packet latency is measured as the 60% of the central data (20% on each side for warm-up and cool-down) and geometric mean of that per node, and geometric mean of that for all nodes, in order to get the value for the chip

simulate with more packets to see if the simulations that you first got are correct, because for 1000 nodes, the amount of packets per node is lower, so maybe results are not as reliable.

simulate to see what incremenet and decrement size for the fuzzy size gives the fastest adaptation for fuzzy token. So make a graph that compares different values of this increment-decrement (1by1, 2by2,...) for a given network configuration, to see what value is best. Jo ho faria més gran +X I més petit /2 Addirive increase i multiplicative decrease Com congestion a TCP (https://en.wikipedia.org/wiki/Additive_increase/multiplicative_decrease)

In related work explain other mac protocols, like the one i have on bookmarks, which does the same as my option 2 (set backoff to 0 to give preference to the node with the token, and the rest of the group use csma or cdma), but they get worse results, i know because i checked it myself by simulating their system (HAITHAM MIGHT ASK SIMULATION RESULTS FOR THIS STATEMENT, SO MAYBE YOU WILL HAVE TO SIMULATE TO PROOF THIS), also explain other hybrid designs and explain why fuzzy is better.

Explaining some concepts:
- the injection rate can be seen as the number of packets that a thread running on a node injects per cycle, on average. If we assume that the application is split evenly among cores, the injection rate of each node should be similar to each other, however this might not always be true, since the programmer might split unevenly the code among nodes, in order to exploit cache locality. For this reason, rsim also models the injection traffic according to a hotspotness parameter sigma (standard deviation of the normal distribution). Higher values of sigma represent higher concentrations of traffic, whereas lower values represent a more uniform one. This can be translated as, the higher the value of sigma, the higher will be the injection rate of fewer nodes compared to the others, and the lower the value of sigma, the more similar will be the injection rate of more nodes compared to the others. These two opposite situations can be observed in Figure X. On average, however, notice that regardless of the value of sigma, the average injection rate among all nodes remains constant and equal to the value stated by the user in the configuration file of RSim. This is because for higher concentrations of traffic, the hotspot nodes inject the equivalent of what other nodes should be transmitting in a uniform scenario, whereas the non-hotspot nodes are now more empty. However, this distribution of traffic must always sum up to the total number of packets to be injected, and therefore the average to be maintained. Note: put equations that prove that if we get a normal distribution with mean=1 and any standard deviation, if we get the range from 0 to 2, and we discretize the curve by dividing that range into N-1 parts (N points), and the value of each point is then multiplied by the desired average injection rate, and multiplied by the number of nodes, and divided by the sum of the discretized values of the gaussian function, the sum of all these injection rates divided by the number of nodes is equal to the desired average injection rate.

The plots should compare different values of hotspotness (sigma), burstiness (hurst), number of cores and type of mac protocol. The comparing value should be the average packet latency and maybe also the total execution time (for a fixed number of total injected packets)

Look at the notes written for the tdma weighted protocol, at the bottom of this page

Have a section where you explain how do you check that everything works. For example explain that you have a debugging mode where you can see what the mac protocol does cycle by cycle, or that you get a log with any possible error during execution, or that you can plot the packet_latencies vector for each node, to check its distribution. Or also that you can check that hotspotness works by seeing the injected packet number in the regular output. Or that you check that the theoretical and real injection rates match, and its averages as well.

Read the observation of rsim section above

Explain why the regular wired network doesn't scale. And why do we need an hybrid, like in AIA. Use plots similar to AIA, to explain the cache cocherence prtocol and a draw of the architecture.

Also, in 60 GHz, if we have 30 nodes per side, 900 in total, and we cluster in groups of 25 nodes, so that we have 6*6=36 clusters, the problem of near field disappears, because then the minimum distance between clusters is 20/6=3.33mm, which is more than 2*lambda=2.92mm, which is far field.

In our case it's very bad to have near field because every antenna will have different near field effects depending on its location in the chip, and the amount of surrounding antennas in the near field area, which is all the nodes within the 2.92mm radius from the anntena.

Near field effect basically makes the impedance that the antenna sees smaller, so we have to increase the impedance of the antenna to compensate. Also, the power decreases faster in the near field area, so how to we transmit enough as to compensate for the losses in the near field, and to arrive to the far field nodes?

Use plot similar to figure 1 in Networking Challenges and ... by Abadal et al.

Get plot of latency vs throughput
On the plot of latency, cut the plot at the lambda such that it saturates

The node that has the token is the one that listens for collisions, and sends the nack in case of collision, do that everybody listens. So we have to ensure that all can listen to the node with the token and that the one with the token can listen to the collisions.

Make a squared plot showing the area of the chip, and divided into squared clusters. Each cluster is painted lighter or darker according to the percentage of total collisions and nacks that can detect

Consider the fact that dipole antennas have a zero on the radiation diagram pointing some neighbour nodes, so instead you should try to find some 60 GHz omnidirectional antenna, like a loop, or something.

Say that we use surface wave instead of space wave, as the paper "Propagation mechanisms of radio waves over intra-chip..." say, so when we model the channel, we only assume one path between tx and rx

In token and fuzzy token, how do you pass the token? In the trailer of the last message of the packet? What if nobody transmitted a packet? Then you have to send it explicitly, right?

--------------------------------
About near field in electromagnetically short antennas:

(antennas shorter than half of the wavelength of the radiation they emit)
For antennas shorter than half of the wavelength of the radiation they emit (i.e., "electromagnetically short" antennas), the far and near regional boundaries are measured in terms of a simple ratio of the distance r from the radiating source to the wavelength λ of the radiation. For such an antenna, the near field is the region within a radius (r ≪ λ), while the far-field is the region for which r ≫ 2λ. The transition zone is the region between r = λ and r = 2λ

Note that D, the length of the antenna is not important, and the approximation is the same for all shorter antennas (sometimes ideally called "point antennas"). In all such antennas, the short length means that charges and currents in each sub-section of the antenna are the same at any given time, since the antenna is too short for the RF transmitter voltage to reverse before its effects on charges and currents are felt over the entire antenna length.

In contrast to the far field, the diffraction pattern in the near field typically differs significantly from that observed at infinity and varies with distance from the source. In the near field, the relationship between E and H becomes very complex. Also, unlike the far field where electromagnetic waves are usually characterized by a single polarization type (horizontal, vertical, circular, or elliptical), all four polarization types can be present in the near field.[4]

The "near field" is a region in which there are strong inductive and capacitive effects from the currents and charges in the antenna that cause electromagnetic components that do not behave like far-field radiation. These effects decrease in power far more quickly with distance than do the far-field radiation effects. Non-propagating (or evanescent) fields extinguish very rapidly with distance, which makes their effects almost exclusively felt in the near-field region.

Also, in the part of the near field closest to the antenna (called the "reactive near field", see below), absorption of electromagnetic power in the region by a second device has effects that feed back to the transmitter, increasing the load on the transmitter that feeds the antenna by decreasing the antenna impedance that the transmitter "sees". Thus, the transmitter can sense when power is being absorbed in the closest near-field zone (by a second antenna or some other object) and is forced to supply extra power to its antenna, and to draw extra power from its own power supply, whereas if no power is being absorbed there, the transmitter does not have to supply extra power.

If we could use 300 GHz, we would solve this problem, because lambda in silicon is 0.29mmm and fraunhoffer distance is 2*lambda=0.58mm, which is already smaller than the minimum distance between cores in a 1024 multicore chip with no clustering (minimum distance is 0.63mm with no clustering and 1.25mm with clustering of 4 cores per cluter)
----------------------------------------------------------------------------------------------

To-do and considerations:
- Am I implementing tdma weighted properly? i'm multiplying the hotspotness weight by the tdma slot size, but instead of doing the ceiling of this product, i'm doing the ceiling of just the weight.
- Read parameters only from standard input, so that you avoid having to repeat parameters every time you modify or add something
- when the input parameter of inj_rate is 1 or 0.8 or bigger than 0.1, I get error: ERROR DURING INJECTION: no condition was satisfied. See why this happens and fix it.
- we still have to check what happens in case of brs-mac p-persistent and in weighted tdma
- deal with the case in which we inject packet but buffer is full
- should we reserve a number of elements equal to buffer_size for the buffer of each node? Or it's simply better to let the class resize dynamically?
- see what variables can be statically allocated for all objects, like the poisson distribution or the random generator, so that we don't ceate it for every object
- see how to make everything separated into fuctions, and make every function only make one thing at a time
- instead of checking the size of each node's buffer every time, which consumes more, maybe we could just have a boolean flag that would indicate when each node has a packet ready to tx. This way is faster to check.
- try to see if you can merge loops over all nodes, so that you don't have to go over all the nodes for every step of the analysis, creating new iterators, etc.
- how do we deal with fairness, so that the nodes that come first in the loop don't have more opportunities to tx than the lasts? This should be taken care in the MAC protocol to see how do we deal with packets that have been wanting to tx for longer times
- put const and try to use & instead of * every time you can
- how do we model the burstiness? So, we have to keep the average rate of the Poisson process but depending of the burstiness we have to tx closer at some points, and then more separate in others.
- we have to create some kind of file called utilities, which has functions like one that can be called to debug the code, that tells the total number of packets that each core has injected and at what average rate each core injected them (total number of packets / total number of cycles)
- be able to transmit different sizes of packets. Be careful because now the buffer shouldn't count number of packets, it should count the total size available. This change will also have implications when you print the size of the buffer, you try to see how many packets you have in the buffer, etc.
- make the simulator to be multithreaded, so that for each cycle, every core gets a different range of nodes to calculate
- now the transmission time is only 1 cycle. Modify this so that user can adjust how many cycles it takes to tx a packet (this will depend of the size of the packet and the bitrate, which can also be specified as an input parameter)
- specify bitrate
- instead of including the headers everywhere, see if you can use forward declaration instead, so that you make the compiler more efficient and at the same time get rid of including redundancies
- in class Node, the in_buffer is public for simplicity. Make it private and prepare the appropriate public functions to operate with it.
- right now we are considering that whenever the exponent of the backoff reaches 10, it simply restarts to 1 again and outputs a warning message. But in reality we should drop the packet so that upper layers would take care of it.
- we have to deal with the BER, here we assume there's never errors during transmission, only for collisions, so we have to assume that some checksums may fail, and we have to send nacks to ask for resending (in case that the packet was important, otherwise we just drop it. This can be known because in the preamble can be specified what type of variable was it. And it's very unlikely that they are confused because we can use a sequencue of 1111100000 for important, and 000000111111 for non important, so that even if a bit is flipped the probability of error is minimal. And even if we get it wrong the only thing it will happen is that an important variable gets lost, which means the processor will ask it through the wired network I DON'T UNDERSTAND HOW THIS WILL WORK, or in the other scenario a neglectable data will be ask to be resend again, which will actually improve the accuracy even when it was not needed)

----------------------------------------------------------------------------------------------

Important considerations when choosing parameters:

Several studies have revealed that on-chip communication in general, and multicast in particular, is potentially
bursty and may be concentrated around a reduced number of cores [3, 40]. Burstiness can be modeled through the
Hurst exponent H, which takes values between 0.5 (exponential) and 1 (extremely bursty). We generate bursty traffic
by alternating ON/OFF periods, the length of which follows Pareto distributions defined by H [26]. Spatial concen-
tration is modeled through a gaussian parameter σ, which
takes values between 0 (concentrated) and ∞ (spread out)
and describes the percentage of the load to be randomly assigned to each node. Previous results assumed H = 0.5 and
σ/N = 100.
Due to the random nature of bursts, we performed 15 runs
for each {H, σ, λ} tuple, where λ is the aggregated load, and
calculated the geometric mean. Figure 6 the results of the
evaluation in a 256-core system for different levels of burstiness and concentration. On the one hand, all networks see
their performance significantly reduced as the burstiness of
traffic increases: W-CSMA and E-MESH show particularly dramatic growths in terms of latency, whereas in all
cases the throughput is approximately cut to half. On the
other hand, the impact of the traffic concentration on performance is generally minor with notable exceptions in
W-MTKN and E-MESH, where the throughput is slightly
higher if the load is spread out.

The goal is to model bursty time sequences, like disk and web traffic, where the traditional Poisson assumption does not hold.

----------------------------------------------------------------------------------------------

Problems, dilemmas, and decisions that I faced during the design of rsim:

- What kind of averaging do we use for calculating the average latency per packet per node? And per chip? Do we use geometric or arithmetic mean or harmonic mean? If we use the geometric mean, how do we create a geometric_mean function that doesn't overflow or lose precision?

- Do we have ACKs? Do we have NACKs? What happens when a node receives a message with a bad checksum? What happens when there are collisions? Even if we deal with collisions as in BRS-MAC, how do we deal with erroneous packets? By sending a NACK at the end? Here's my solution for BRS-MAC: we send preamble, and if any node doesn't receive it correctly it sends nack, so that origin assumes collision and backs off, if no nack is received it means no collision happened and all the other nodes won't try to transmit until tx is done.

- I first used a poisson process for modeling the time interval between the injection of packets in each node (using the average injection rate as a mean of the exponential distribution). But then i had to incorporate the burstiness parameter, and i realized i had to use another model, since the poisson model didn't hold anymore.
- I saw a paper called "Data Mining Meets Performance Evaluation: Fast Algorithms for Modeling Bursty Traffic" which need to model burstiness for other stuff, and they modeled it by using . Others have also modeled burstiness by alternating ON/OFF periods, the length of which follows Pareto distributions defined by H (the Hurst parameter).

Mathematically speaking, traffic burstiness is related to short-terms autocorrelations between the interarrival times. However, there is not a single widely accepted notion of burstiness [frost94traffic]; instead, several different measures are used, some of which ignore the effect of second order properties of the traffic.

(ARE THESE OTHER BURSTINESS MODELS? I SAW IT ON THE PAPER OF DATA MINING) Previous models (like fractional Brownian motion, FARIMA etc) tried to capture the ‘burstiness’. However, the proposed models either require too many parameters to fit and/or require prohibitively large (quadratic) time to generate large traces.

Empirical studies of measured traffic traces have led to the wide recognition of self-similarity in network traffic. Self-similar Ethernet traffic exhibits dependencies over a long range of time scales. This is to be contrasted with telephone traffic which is Poisson in its arrival and departure process. In traditional Poisson traffic, the short-term fluctuations would average out, and a graph covering a large amount of time would approach a constant value.

One way to represent burstiness through a Poisson model is to use the so called compound Poisson process, where packet arrivals happen in bursts (or batches), the interbatch times are independent and exponentially distributed (that is, they represent a Poisson process), and the batch sizes are random. This scenario can be modeled using two processes {An} and {Bn}, the first one representing the batch interarrival times and the second the batch sizes. However, as will be detailed in the following sections, different traffic models are preferable to compound Poisson traffic models in that they tackle the problem of burstiness in a more radical way. In particular, multi-scale burstiness is captured by self-similar traffic models. But, before considering self-similarity, let us introduce a simpler traffic model which overcomes other limitations of Poisson.

To define the burstiness of each node, the Hurst
exponent H s for source node s, is adopted. As a component
of the characteristics of self-similarity, H s determines the
temporal burstiness of traffic at each node, that is, the peak
size of injection packets and their injection patterns of arrival
time. To define one of spatial properties in NoC traffic traces,
the distribution of average injection rate at every node, denoted
by λ s is captured. Finally δ (s,d) representing the distribution
of traffic ratio from s node to d node in the given injection
rate λ s is extracted.

A number of models have been proposed for the task of modelling long-tail traffic. These include the following:
Fractional ARIMA
Fractional Brownian motion
Iterated Chaotic Maps
Infinite Markov Modulated Processes
Poisson Pareto Burst Processes (PPBP)
Markov Modulated Poisson Processes (MMPP) [16]
Multi-fractal models[3]
Matrix models[4]
Wavelet Modelling
Tweedie distributions
No unanimity exists about which of the competing models is appropriate,[4] but the Poisson Pareto Burst Process (PPBP), which is an M/G/ {\displaystyle {\mathcal {1}}} {\mathcal  {1}} process, is perhaps the most successful model to date. It is demonstrated to satisfy the basic requirements of a simple, but accurate, model of long-tail traffic.

For heavy-tailed traffic, extremely large bursts occur more frequently than with light-tailed traffic.[18] Additionally, aggregating streams of long-tail traffic typically intensifies the self-similarity ("burstiness") rather than smoothing it, compounding the problem.

Is the traffic that we have in Replica, self-similar? We should know that, so that we would model it with one burstiness model or another. Yes it is, as it's mentioned in the Soteriou et al. paper --> As many
communication traffics are proven to be statistically self-
similar, some researchers already showed that the traffic in
NoC also has a self-similar characteristic [15], [16]. Thus,
we parameterize such a degree of burstiness or LRD using H.
Furthermore, in order to be accurate, this parameter indicating
the burstiness is analyzed on every injection node.

A common measure of self-similarity in the literature is the Hurst exponent H [2]. A value of H between 1/2 and 1 indicates the degree of self-similarity. It is also used as a global1 index for burstiness [9]. There are several
exploratory analytic tools to estimate H, such as R/S plots, variance plots, autocorrelation functions, and periodograms [2].

How does a spatial distribution model take into consideration that there might be 2 hotspots instead of just 1? And that one of them is bigger than the other/s. A gaussian model only applies if we have one hotspot, right? --> No, a gaussian model indicates how many nodes want to transmit a lot, not how they are distributed. If sigma is smaller (narrow pdf) it means we have a lot of nodes wanting to transmit a lot, and if sigma is bigger (flat pdf) it means all want to transmit in a similar rate.

How do we combine the burstiness with the hotspotness? We could assign weights to the lambda of each core, in a way such that in average (l1+l2+...+ln)/n=avg_lambda and the weights could be assigned following a gaussian distribution, so that for smaller sigmas we would have a few nodes with large lambdas and a lot with low lambdas, and for big lambdas we would have a lot of nodes with approximately the same lambda

Do as in noxim simulator, that the user can choose among different injection distributions, to choose poisson, burst, hotspotness, and to adjust the parameter of each distribution.

----------------------------------------------------------------------------------------------

CSMA/CA non-persistent:
- At every cycle, every station with non-empty buffers check if channel is idle
- When a packet is put into the buffer it already has chosen the initial backoff guess between 1 and 2^1, so for every node if the medium is idle and its backoff counter is zero it transmits the first cycle of the packet (when a node starts transmitting it sets medium_busy, so be careful because in the loop that ckecks all nodes ready to tx we have to check if the medium was idle before starting the foreach loop otherwise we will be making the medium busy before than what it would actually be in reality), so we could have a vector being the packets being transmitted in the medium, and if this was more than 1 would mean collision. So we check if the medium was idle before starting the foreach loop, this way we ensure fairness among all nodes. And at the end of the foreach loop we see which ones had the backoff to zero and therefore transmitted. And if it was more than 1 then we make as if there was a nack in the next cycle, and we put both in backoff. Be careful because in the meantime we have to ensure that those packets are not checked again in the foreach loop.
- If a node has a non-empty buffer but the medium is busy, we do nothing. So we only decrease the counters when the medium is idle.

TDMA fixed:
- similar to tdma weighted, but the tdma_current_node_slot_size is always fixed, and gets the value from the ini file. We have a slot size, and we assign a slot to each node, so that they all have same number of slots. They can only transmit (in case they have some data to tx) when the current cycle matches with the time slot that they were assigned

TDMA weighted:
- in mac_protocols, the function protocol_tdma_weighted, which is called at every cycle, we have a global variable which determines the number of cycles that the current node can transmit, this number is determined when we pass the token to the next node, and we assign the value depending of the weight of that node, which will be the same percentage that is given by the hotspot parameter. So, at every cycle we check the global variable tdma_current_node, which is a counter that goes in the range [0,ncores] and resets when it gets to ncores. So every time that the time for the current node is done we increase this counter. So, this counter things are done first, and then we check if there's any node in nodes_ready that matches the tdma_current_node, and if so, it transmit one cycle and decreases cycles_left for that packet, and if cycles_left is zero we pop that packet out of the buffer.
- SHOW EQUATIONS FROM DRAFT NUMBER 2. Show how to get the timeslot size assigned to each node, based on the weights calculated from the hotspotness. The process if very similar to that used to assign the new lambdas, but now instead of new lambdas we have new slot sizes, and before the average was desired_inj_rate, now the average is desired_slot_size, such that desired_slot_size*ncores=total_tdma_frame

Token:
if we have something to transmit, we transmit the whole packet and then pass the token, otherwise we just pass the token.

Fuzzy token:
the token keeps passing in the same way than in token-ring, but now also the guys around the node with the token can also transmit. How many? This size is determined by what all the nodes can listen, if there's a brs-collision
