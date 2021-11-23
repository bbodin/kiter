/*
 *
 *      Author: toky
 */

#define BOOST_TEST_MODULE StrictlyPeriodicTest
#include "helpers/test_classes.h"


#include <algorithms/throughput/csdf_strictly_periodic.h>
#include <algorithms/throughput/kperiodic.h>
#include <algorithms/schedulings.h>
#include <models/EventGraph.h>



BOOST_FIXTURE_TEST_SUITE( event_graph_test_suite , WITH_SAMPLE)

BOOST_AUTO_TEST_CASE( test_pipeline_generation )
{

	computeRepetitionVector(pipeline_sample);

	models::EventGraph* eg = algorithms::generate_csdf_strictly_periodic_event_graph(pipeline_sample);
	std::cout << eg->printXML() << std::endl;
	delete eg;

}

BOOST_AUTO_TEST_CASE( test_cycle_generation )
{

	computeRepetitionVector(cycle_sample);

	models::EventGraph* eg = algorithms::generate_csdf_strictly_periodic_event_graph(cycle_sample);
	std::cout << eg->printXML() << std::endl;
	delete eg;

}

BOOST_AUTO_TEST_CASE( test_MinCycleRatio )
{

	computeRepetitionVector(cycle_sample);

	models::EventGraph* eg = algorithms::generate_csdf_strictly_periodic_event_graph(cycle_sample);
	std::cout << eg->printXML() << std::endl;
	std::cout << "Looking for (Period:24) or (Thr:0.0416667)" << std::endl;
	BOOST_CHECK_CLOSE ( 1 / eg->MinCycleRatio().first , 24, 0.0001 );
	delete eg;

}

BOOST_AUTO_TEST_CASE( test_scheduling_simple_buffer_dataflow )
{
    models::Dataflow* g = new models::Dataflow(0);

    auto v1 = g->addVertex("one");
    g->setPhasesQuantity(v1,1);
    g->setVertexDuration(v1,{1});

    auto v2 = g->addVertex("two");
    g->setPhasesQuantity(v2,1);
    g->setVertexDuration(v2,{1});

    auto e1 = g->addEdge(v1,v2,"a");
    g->setEdgeInPhases(e1,{1});
    g->setEdgeOutPhases(e1,{1});

    g->setReentrancyFactor(v1,1);

	VERBOSE_ASSERT(computeRepetitionVector(g),"Cannot generate repetition vector.");
	models::Scheduling res2 = algorithms::scheduling::CSDF_SPeriodicScheduling (g);
    BOOST_REQUIRE_EQUAL( res2.getGraphPeriod(), 1 );

    delete g;

}

BOOST_AUTO_TEST_CASE( test_scheduling_varied_duration_time_buffer_dataflow )
{
    models::Dataflow* g = new models::Dataflow(0);

    auto v1 = g->addVertex("one");
    g->setPhasesQuantity(v1,2);
    g->setVertexDuration(v1,{1,2});

    auto v2 = g->addVertex("two");
    g->setPhasesQuantity(v2,1);
    g->setVertexDuration(v2,{1});

    auto e1 = g->addEdge(v1,v2,"a");
    g->setEdgeInPhases(e1,{1,1});
    g->setEdgeOutPhases(e1,{1});

    g->setReentrancyFactor(v1,1);

	VERBOSE_ASSERT(computeRepetitionVector(g),"Cannot generate repetition vector.");
	models::Scheduling res = algorithms::scheduling::CSDF_SPeriodicScheduling (g);

    BOOST_REQUIRE_EQUAL( res.getGraphPeriod(), 4);

	g->reset_computation();
    g->setReentrancyFactor(v1,1);

	VERBOSE_ASSERT(computeRepetitionVector(g),"Cannot generate repetition vector.");
	models::Scheduling res2 = algorithms::scheduling::CSDF_KPeriodicScheduling_LP (g, algorithms::scheduling::generate1PeriodicVector(g));

    BOOST_REQUIRE_NE( res.getGraphPeriod(), res2.getGraphPeriod());
	

    delete g;

}



BOOST_AUTO_TEST_CASE( test_SPeriodicTask )
{

	algorithms::scheduling::SPeriodicScheduling (cycle_sample, {}) ;

}


BOOST_AUTO_TEST_SUITE_END()




