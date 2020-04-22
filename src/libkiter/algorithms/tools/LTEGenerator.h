/*
 * LTEGenerator.h
 *
 *  Created on: Apr 21, 2020
 *      Author: toky
 */

#ifndef SRC_LIBKITER_ALGORITHMS_TOOLS_LTEGENERATOR_H_
#define SRC_LIBKITER_ALGORITHMS_TOOLS_LTEGENERATOR_H_

#include <commons/commons.h>


namespace models {
	class Dataflow;
}


struct phase_info {
	int thds;
	TIME_UNIT exec_time;
	int send_pkts;
	std::string name;
};


static inline const phase_info getPhaseStruct(int t, TIME_UNIT e, int p, const std::string n) {
	const phase_info ret{t, e, p, n};
	return ret;
}


namespace algorithms {
	models::Dataflow*  generate_lte_sdf(parameters_list_t   param_list);
}



#endif /* SRC_LIBKITER_ALGORITHMS_TOOLS_LTEGENERATOR_H_ */
