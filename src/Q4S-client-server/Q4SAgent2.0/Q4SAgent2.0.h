#ifndef _Q4SAGENT2.0_H_
#define _Q4SAGENT2.0_H_
class Measure {
public:
	std::string  Jitter;
	int Latency;
	int PacketLoss;
};

extern Measure measure;

#endif  // _Q4SAGENT2.0_H_