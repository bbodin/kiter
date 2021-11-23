/*
 * TDMAScheduler.cpp
 *
 *  Created on: 15 Aug 2021
 *      Author: toky
 */

#include <algorithms/scheduling/TDMAScheduler.h>
#include <queue>


	long findEventCycle(long curr_slot, long my_slot, long total_slot)
	{
		if (my_slot >= curr_slot)
			return my_slot - curr_slot;
		else
			return (total_slot - curr_slot) + my_slot;
	}

	long getDistance(long src, long des, long rows, long cols)
	{
		long s_r = src / rows, s_c = src % cols;
		long d_r = des / rows, d_c = des % cols;

		return abs(s_r-d_r) + abs(s_c-d_c) + 1;
	}


void algorithms::scheduling::TDMAScheduler::write_tdma_schedule (std::string slot_filename, std::string pkt_filename, std::string taskmap_filename) {
		std::vector<long> task_map; //task mapping
		long TOTAL_SLOTS = 0;

		//reading packet data from file
		//read packets from the file
		std::vector<packet_struct> packets;

		//start taskmap
		std::string l2;
		std::ifstream f2;
		f2.open(taskmap_filename);
		bool flag = false;
		long hyperperiod;
		while(f2 >> l2)
		{
			std::stringstream ss(l2);
			while( ss.good() )
			{
				std::string substr;
				std::getline( ss, substr, ',' );
				if(flag)
					task_map.push_back(std::atol(substr.c_str()));
				else
				{
					hyperperiod = std::atol(substr.c_str());
					flag = true;
				}
			}
		}
		f2.close();
		//taskmap filename end

		std::string l;
		std::ifstream f;
		f.open(pkt_filename);
		long rows = -1;
		long cols = -1;
		long bank = -1;
		long count = 0;

		while(f >> l)
		{
			if(l.find('#') !=std::string::npos)
				continue;

			std::vector<long> tl;
			std::stringstream ss(l);
			while( ss.good() )
			{
				std::string substr;
				std::getline( ss, substr, ',' );
				tl.push_back(std::atol(substr.c_str()));
			}
			//get system specs, i.e. rows, cols and number of banks
			if(count == 0)
			{
				rows = tl[0];
				cols = tl[1];
				bank = tl[2];
				//cout << rows << "," << cols << "," << bank << endl;

				OFFSET = rows*cols;
				count += 1;
				continue;
			}

			//SRC,DEST,DURATION,PACKET_ID,BANK_ID,DEP1,DEP2,...
			long src = long(tl[0]); //src
			long dest = long(tl[1]); //dest
			long duration = long(tl[2]);
			long p_id = long(tl[3]); //packed id
			long b_id = long(tl[4]); //bank id

			//cout << "ll:" << p_id << "," << src << "," << dest << "," << duration << "," << /*p_id << "," <<*/ b_id << endl;
			std::vector<long> deps;
			for (long tl_i = 5; tl_i < tl.size(); tl_i++)
			{
				long item = tl[tl_i];
				deps.push_back((long)item);
				//cout << "," << deps[deps.size()-1];
			}
			//cout << endl;
			//insert packet into the data structure
			//packets[p_id] = [src, dest, b_id, duration, deps];
			packet_struct elem(task_map[src], task_map[dest], b_id, duration, deps);
			if(p_id != packets.size())
				std::cout << "PACKET ID DOES NOT MATCH SEQUENTIALLY\n";
			packets.push_back(elem);

			//cout << "rr:" << packets.size()-1 << "," << packets[packets.size()-1].src << "," << packets[packets.size()-1].dest << endl;
		}
		f.close();
		//read packet end

		std::cout << "HYPER_PERIOD=" << hyperperiod << "\n";
		for(long i = 0; i < OFFSET*OFFSET; i++)
		{
			slot_table.push_back(-1);
			slotlock.push_back(-1);
		}
		f.open(slot_filename);
		flag = false;
		while(f >> l)
		{
			std::vector<std::string> tline;
			std::stringstream ss(l);
			while( ss.good() )
			{
				std::string substr;
				std::getline( ss, substr, ',' );
				tline.push_back( substr );
			}

			if(!flag)
			{
				TOTAL_SLOTS =  atol(tline[0].c_str());
				std::cout << "TOTAL_SLOTS=" << TOTAL_SLOTS << std::endl;
				flag = true;
				continue;
			}

			long src = atol(tline[1].c_str());
			long dest = atol(tline[3].c_str());
			long slot = atol(tline[5].c_str());
			slot_table[ get_slot_table_index(src,dest) ] = slot;
			//cout << "src," << src << ",dest," << dest << ",slot," << slot << endl;
		}
		f.close();
		//print slot_table
		//finished reading slot values

		//add cycle value and the packet into the heapqueue
		std::priority_queue<pq_struct> myheap;
		std::vector<std::vector<long>> packet_profile(packets.size(), std::vector<long>(4, -1));
		for (long pkt_id=0; pkt_id < packets.size(); pkt_id++)
		{
			pq_struct pq_elem(0, pkt_id, false);
			myheap.push(pq_elem);
			isPktDone.push_back(-1);
			//packet_profile[pkt_id] = [-1, -1, -1, -1]; //depsdone,computedone,slotwait,transferdone
		}
		//prlong myheap


		long SYSTEM_CYCLE = 0;
		while (myheap.size() != 0)
		{
			pq_struct pq_elem = myheap.top();
			myheap.pop();
			long cycle = pq_elem.cycle;
			long pkt_id = pq_elem.pkt_id;
			bool computeDone = pq_elem.computeDone;

			long advance_cycles = 1;
			long src = packets[pkt_id].src;
			long dest = packets[pkt_id].dest;
			long duration = packets[pkt_id].duration;
			std::vector<long> deps = packets[pkt_id].deps;
			long src_dest_key = get_slot_table_index(src, dest);

			std::string print_str ="pkt_id=" + std::to_string(pkt_id) + ",src=" + std::to_string(src) + ",dest=" + std::to_string(dest)  + ",duration=" + std::to_string(duration)  + ",cycle=" + std::to_string(cycle);
			if (isDepsDone(deps, cycle)) //if all dependencies are met
			{
				if (packet_profile[pkt_id][DEPS] == -1)
					packet_profile[pkt_id][DEPS] = cycle;

				if (computeDone) //if compute is done
				{
					if (packet_profile[pkt_id][COMP] == -1)
						packet_profile[pkt_id][COMP] = cycle;

					long curr_slot = cycle%TOTAL_SLOTS;
					//cout << "src=" << src << ",dest=" << dest << ",slot=" << slot_table[src_dest_key] << endl;

					if (src == dest)
					{
						isPktDone[pkt_id] = cycle + LOCAL_LAT;
						print_str += " done in " + std::to_string(cycle + LOCAL_LAT);
						packet_profile[pkt_id][DATA] = cycle + LOCAL_LAT;
						packet_profile[pkt_id][SLOT] = cycle;
						std::cout << print_str << std::endl;
						continue;
					}
					else if (curr_slot == slot_table[src_dest_key])
					{
						if (/*slotlock.find(src_dest_key) != slotlock.end() &&*/ cycle == slotlock[src_dest_key]) //if some packet is sent to particulat destination, check which cycle it was sen3
						{
							advance_cycles = TOTAL_SLOTS;
							packet_profile[pkt_id][SLOT] = cycle+advance_cycles;
						}
						else
						{
							slotlock[src_dest_key] = cycle; //prevent same source from sending multiple packets in the same cycle
							//change start srjkvr
							//int cycles_left = TOTAL_SLOTS - curr_slot;
							long cycles_left = getDistance(src, dest, rows, cols);
							//if(cycles_left > (TOTAL_SLOTS - curr_slot))
							//	cout << "error src=" << src << ",dest=" << dest << ",col=" << cols << ",row=" << rows << "\n";
							//change end srjkvr

							isPktDone[pkt_id] = cycle + cycles_left;

							if (packet_profile[pkt_id][SLOT] == -1)
							{
								packet_profile[pkt_id][SLOT] = cycle;
							}

							print_str += " done in " + std::to_string(cycle + cycles_left);
							packet_profile[pkt_id][DATA] = cycle + cycles_left;
							std::cout << print_str << std::endl;
							continue;
						}
					}
					else
					{
						if (packet_profile[pkt_id][SLOT] == -1)
						{
							advance_cycles = findEventCycle(curr_slot, slot_table[src_dest_key], TOTAL_SLOTS);
							packet_profile[pkt_id][SLOT] = cycle+advance_cycles;
						}
						else
						{
							printf ("error2\n");
						}
					}
				}
				//if slot not found, just ignore
				else //if compute done
				{
					advance_cycles = duration;
					computeDone = true;
					//print_str += " compute"
				}
			}
			pq_struct pq_elem2(cycle+advance_cycles, pkt_id, computeDone);
			myheap.push(pq_elem2);
		}

		long iter_count = 0;
		long prev_start = 0;
		long prev_end = 0;
		long start = 0;
		long end = 0;
		long exec_time = 0, throughput = 0;

		printf("depsdone,computedone,slotwait,transferdone,src,dest,compute,\n");
		for(long p_id = 0; p_id < packets.size(); ++p_id)
		{
			if(p_id%hyperperiod == 0)
			{
				iter_count += 1;
				prev_start = start;
				start = packet_profile[p_id][DATA];
			}
			if((p_id+1)%hyperperiod == 0)
			{
				prev_end = end;
				end = packet_profile[p_id][DATA];

				exec_time += (end - start);
				throughput += (end - prev_end);
				//cout << "exec_time=" << exec_time << endl;
				std::cout << "thro=" << throughput << "\n";
			}

			long src  = packets[p_id].src;
			long dest = packets[p_id].dest;
			long comp = packets[p_id].duration;
			std::cout << "pkt_id=" << p_id << "," << packet_profile[p_id][DEPS] << "," << packet_profile[p_id][COMP] << "," << packet_profile[p_id][SLOT] << "," << packet_profile[p_id][DATA] << "," << src << "," << dest << "," << comp << std::endl;
		}

		std::cout << "thro=" << (float)throughput/iter_count << ",exec_time=" << (float)exec_time/iter_count << std::endl;
		return;
	}

