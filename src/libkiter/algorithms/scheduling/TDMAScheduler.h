/*
 * TDMAScheduler.h
 *
 *  Created on: 15 Aug 2021
 *      Author: Bruno, based on original code from Vanchi
 *
 *  TODO: Temporary, need to be rewritten using internal Kiter resources.
 *
 *  Current wa yto run it:
 *  ./Release/bin/kiter -f ./benchmarks/ascenttestbench/one_modem.xml -pslotfile=./benchmarks/temporary/slot_4x4
 *                                                                    -ppktfile=./benchmarks/temporary/modem.txt
 *                                                                    -pmapfile=./benchmarks/temporary/modem_task
 *                                                                    -a TDMASchedule
 *
 * slot_4x4 is the s4noc NoC scheduling
 * modem_task contains "10000,5,6,9,10,11,7,3,1,12,8,4,0,2,15,14,13"
 * modem.txt can be generated with the SymbolicExecutionWP action in kiter.
 *
 *
 */

#ifndef SRC_LIBKITER_ALGORITHMS_SCHEDULING_TDMASCHEDULER_H_
#define SRC_LIBKITER_ALGORITHMS_SCHEDULING_TDMASCHEDULER_H_


#include <commons/commons.h>
#include <commons/KiterRegistry.h>

#include <vector>
#include <string>


namespace models {
	class Dataflow;
	class Scheduling;
}

namespace algorithms {
namespace scheduling {
//#define TOTAL_SLOTS 18
#define DEPS 0
#define COMP 1
#define SLOT 2
#define DATA 3
#define LOCAL_LAT 0

struct pq_struct {
	long cycle, pkt_id;
	bool computeDone;
	pq_struct(long c, long p, bool cd) : cycle(c), pkt_id(p), computeDone(cd) {}
};

bool operator<(const pq_struct& a, const pq_struct& b) {
	if (a.cycle == b.cycle)
		return a.pkt_id > b.pkt_id;
	return a.cycle > b.cycle;
}


struct packet_struct {
	long src, dest, b_id, duration;
	std::vector<long> deps;
	packet_struct(long s, long d, long b, long dur, std::vector<long> dep) : src(s), dest(d), b_id(b), duration(dur), deps(dep) {}
};





class TDMAScheduler {

	//typedef std::map<int, int> int_map;
	//int_map slot_table, /*isPktDone,*/ slotlock;

	long OFFSET;
	std::vector<long> isPktDone, slotlock, slot_table;

	long get_slot_table_index(long s, long d)
	{
		return s*OFFSET /*300*/ + d;
	}

	//checking if packet processing done
	bool isDepsDone(std::vector<long> deps, long cycles)
	{
		for(long i = 0; i < deps.size(); i++)
		{
			long d = deps[i];
			if(isPktDone[d] == -1)
			//if (isPktDone.find(d) == isPktDone.end()) //if an entry for compute is not present, then the packets has not been sent. Thus not ready.
				return false;
			else if (isPktDone[d] > cycles)
				return false;
		}
		return true;
	}

	/**
	 * Example of Usage:
	 * write_tdma_schedule ( "slot_4x4", "modem.txt", "modem_task" );
	 */
public:
	void write_tdma_schedule (std::string slot_filename, std::string pkt_filename, std::string taskmap_filename);


};


#define SLOTFILE_PARAMETER "slotfile"
#define PACKETFILE_PARAMETER "pktfile"
#define MAPFILE_PARAMETER "mapfile"
void TDMASchedule    (models::Dataflow* const , parameters_list_t  parameters  ) {
	TDMAScheduler sched;
	VERBOSE_ASSERT(parameters.count(SLOTFILE_PARAMETER), "Slot description file not found (i.e. slot_4x4), please use the parameter '" << SLOTFILE_PARAMETER << "'");
	VERBOSE_ASSERT(parameters.count(PACKETFILE_PARAMETER), "Packet description file not found (i.e. modem.txt), please use the parameter '" << PACKETFILE_PARAMETER << "'");
	VERBOSE_ASSERT(parameters.count(MAPFILE_PARAMETER), "Task mapping description file not found (i.e. modem_task), please use the parameter  '" << MAPFILE_PARAMETER << "'");
	std::string slot_filename = parameters[SLOTFILE_PARAMETER];
	std::string pkt_filename =parameters[PACKETFILE_PARAMETER];
	std::string taskmap_filename =parameters[MAPFILE_PARAMETER];
	sched.write_tdma_schedule(slot_filename, pkt_filename, taskmap_filename);
}
}
}


ADD_TRANSFORMATION(TDMASchedule,
		transformation_t({ "TDMASchedule" , "Execute the TDMA schedule for given list of packets", algorithms::scheduling::TDMASchedule}));

#endif /* SRC_LIBKITER_ALGORITHMS_SCHEDULING_TDMASCHEDULER_H_ */
