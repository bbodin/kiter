/*
 * throughput.h
 *
 *  Created on: 16 avr. 2013
 *      Author: toky
 */

#ifndef PERIODIC_FIXED_H_
#define PERIODIC_FIXED_H_

#include <commons/KiterRegistry.h>
#include <models/Dataflow.h>
#include <algorithms/buffersizing.h>
#include <vector>

namespace models {
	class EventGraph;
        class Dataflow;
}

namespace algorithms {

    bool ctnewMinMax(models::Dataflow * dataflow,  TIME_UNIT OMEGA, Vertex t, std::vector<TIME_UNIT>& res) ;
    void checkOffsets (models::Dataflow * const dataflow, TIME_UNIT OMEGA, std::map<Vertex,std::vector<TIME_UNIT> > & offsets) ;

    bool generateBurstOffsets(models::Dataflow * const dataflow,std::map<Vertex,std::vector<TIME_UNIT> > & res);
    bool generateStrictlyPeriodicOffsets(models::Dataflow * dataflow, TIME_UNIT OMEGA, std::map<Vertex,std::vector<TIME_UNIT> > & res) ;
    bool generateAverageOffsets(models::Dataflow * const dataflow,TIME_UNIT OMEGA, std::map<Vertex,std::vector<TIME_UNIT> > & res);
    bool generateMinMaxOffsets(models::Dataflow * const dataflow,TIME_UNIT OMEGA, std::map<Vertex,std::vector<TIME_UNIT> > & res);
    bool generateWiggersOffsets(models::Dataflow * const dataflow,TIME_UNIT OMEGA, std::map<Vertex,std::vector<TIME_UNIT> > & res);

    BufferSizingResult speriodic_memory_sizing_csdf(models::Dataflow* const  dataflow, TIME_UNIT PERIOD, bool INTEGERSOLVING, bool ilp_solving) ;
    BufferSizingResult compute_periodic_fixed_memory(models::Dataflow* const  dataflow, std::map<Vertex,std::vector<TIME_UNIT> > & res,  TIME_UNIT PERIOD , bool ilp_solving , bool gen_only);

    void compute_strictly_periodic_memory(models::Dataflow* const  dataflow, parameters_list_t params);
    void compute_fixed_offset_buffer_sizing (models::Dataflow* const dataflow, parameters_list_t params);

    void add_vbuffers (models::Dataflow* const  dataflow, parameters_list_t params);

} // end of namespace algorithms

//Buffer sizing techniques
ADD_TRANSFORMATION(SPeriodicSizing,
transformation_t({ "SPeriodicSizing" , "Minimal Buffer size estimation by periodic scheduling with StrictlyPeriodic policy.", algorithms::compute_strictly_periodic_memory}));
ADD_TRANSFORMATION(FixedOffsetBufferSizing,
transformation_t({ "FixedOffsetBufferSizing" , "Minimal Buffer size estimation by periodic scheduling with fixed offset buffer sizing techniques (BURST, AVERAGE, MINMAX, WIGGERS). Define chosen sizing technique as parameter.", algorithms::compute_fixed_offset_buffer_sizing}));
ADD_TRANSFORMATION(AddVBuffers,
transformation_t({ "AddVBuffers" , "Add virtual buffers for each edge in dataflow graph", algorithms::add_vbuffers}));

#endif /* PERIODIC_FIXED_H_ */
