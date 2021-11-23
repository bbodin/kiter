/*
 * Scheduling.h
 *
 *  Created on: 25 June 2021
 *      Author: Katherine
 */

#ifndef SRC_LIBKITER_ALGORITHMS_SCHEDULING_SCHEDULING_H_
#define SRC_LIBKITER_ALGORITHMS_SCHEDULING_SCHEDULING_H_

#include <models/Dataflow.h>
#include <set>
#include <map>
#include <vector>

typedef std::vector<TIME_UNIT> static_task_schedule_t;
typedef std::pair<TIME_UNIT, static_task_schedule_t> periodic_task_schedule_t;

struct task_schedule_t {
	static_task_schedule_t initial_starts;
	periodic_task_schedule_t periodic_starts;
	task_schedule_t ()  :  initial_starts(), periodic_starts() {}
	task_schedule_t (static_task_schedule_t is, periodic_task_schedule_t ps) : initial_starts(is), periodic_starts(ps) {}
};

class scheduling_t {
private :
	std::map<ARRAY_INDEX, task_schedule_t> _data;
public:
	void set (ARRAY_INDEX i, task_schedule_t ts) {
		_data[i] = ts;
	}

	ARRAY_INDEX size () {
		return _data.size();
	}
	//task_schedule_t operator [](ARRAY_INDEX i) const {return _data[i];}
	task_schedule_t& operator [](ARRAY_INDEX i)  {return _data[i];}

	std::map<ARRAY_INDEX, task_schedule_t>::const_iterator  begin() const { return _data.begin();}
	std::map<ARRAY_INDEX, task_schedule_t>::const_iterator  end()  const { return _data.end(); }

};
//typedef std::map<ARRAY_INDEX, task_schedule_t> scheduling_t;
typedef std::set<Edge>  critical_circuit_t;

namespace models {

class Scheduling {
private:
	TIME_UNIT              _period ;
	const models::Dataflow*_dataflow;
	scheduling_t           _tasks_schedule;
    critical_circuit_t     _critical_edges;
public :
	Scheduling () : _period (0), _dataflow(nullptr)  {}
private :
	Scheduling (TIME_UNIT period, const models::Dataflow* dataflow) : _period (period), _dataflow(dataflow) {

	}

public :
	Scheduling (const models::Dataflow* dataflow, TIME_UNIT omega, scheduling_t tasks_schedule
	 ,critical_circuit_t critical_edges
	) : _period(omega), _dataflow(dataflow), _tasks_schedule(tasks_schedule)
	 , _critical_edges (critical_edges)
	{

	}
	Scheduling (const models::Dataflow* dataflow, TIME_UNIT omega, scheduling_t tasks_schedule
			) : _period(omega), _dataflow(dataflow), _tasks_schedule(tasks_schedule), _critical_edges ()
	{

	}

	const models::Dataflow* getDataflow () const {
			return _dataflow;
	}

	TIME_UNIT getGraphPeriod () const {
			return this->_period;
	}

	TIME_UNIT getGraphThroughput () const {
			return 1.0 / this->_period;
	}
	const scheduling_t& getTaskSchedule () const{
		return this->_tasks_schedule;
	}

	 const critical_circuit_t& getCriticalEdges() const {
	 	return this->_critical_edges;
	 }

	void verbose_print () const;
	std::string asText () const;
	std::string asASCII (int line_size) const;
	std::string asASCIINew (int line_size) const;
	std::string asASCIINewNew (int line_size) const;
	bool is_valid_schedule () const;

};
}

#endif /* SRC_LIBKITER_ALGORITHMS_SCHEDULING_SCHEDULING_H_ */
