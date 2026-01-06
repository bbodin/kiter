/*
 * EventGraph.h
 *
 *  Created on: 17 avr. 2012
 *      Author: toky
 */


#ifndef EVENTGRAPH_H_
#define EVENTGRAPH_H_
#define BOOST_BIND_GLOBAL_PLACEHOLDERS // Required for boost graph to avoid warnings.
#include <iostream>
#include <cassert>
#include <ctime>
#include <boost/functional/hash.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/random.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include <boost/version.hpp>
#include <stdexcept>

#include "howard_cycle_ratio.hpp"
//#include <boost/graph/howard_cycle_ratio.hpp>

#include <queue>
#include <commons/commons.h>
#include <commons/verbose.h>

#define TXT_NEW_EDGE_ERROR "NEW_EDGE_ERROR"

#define VERBOSE_EVENTGRAPH_DEBUG(msg) {VERBOSE_CUSTOM_DEBUG("EventGraph",msg);}
/**
 * This struct is the holy graal ...
 * ... otherwise boost use an epsilon of 0.005.
 */
struct TIME_UNIT_LIMITS {
    typedef TIME_UNIT value_type;

    static TIME_UNIT infinity()
    { return std::numeric_limits<TIME_UNIT>::infinity(); }

    static TIME_UNIT epsilon()
    { return  std::numeric_limits<TIME_UNIT>::epsilon()  ; }


};


namespace models {


struct SchedulingEvent {
private :
    ARRAY_INDEX _t;
    PHASE_INDEX _p;
    EXEC_COUNT  _n;
public :
    ARRAY_INDEX inline  getTaskId() const {return this->_t;};
    PHASE_INDEX inline  getTaskPhase() const {return this->_p;};
    EXEC_COUNT  inline  getTaskOc() const {return this->_n;};
    SchedulingEvent(ARRAY_INDEX t,PHASE_INDEX p,EXEC_COUNT n) : _t(t) , _p(p) , _n(n) {}
    SchedulingEvent(ARRAY_INDEX t,EXEC_COUNT n) : _t(t) , _p(1) , _n(n) {}
    SchedulingEvent(ARRAY_INDEX t) : _t(t) , _p(1) , _n(1) {}
    std::string toString() const {return commons::toString<ARRAY_INDEX>(_t) + ',' + commons::toString<PHASE_INDEX>(_p) + ',' + commons::toString<EXEC_COUNT>(_n);}
    friend  bool operator==(const SchedulingEvent& lh, const SchedulingEvent& rh) ;
    friend  bool operator!=(const SchedulingEvent& lh, const SchedulingEvent& rh) ;
    friend  bool operator<(const SchedulingEvent& lh, const SchedulingEvent& rh) ;
};
inline bool operator==(const SchedulingEvent& lh, const SchedulingEvent& rh) {return (lh._t == rh._t) &&  (lh._p == rh._p)&&  (lh._n == rh._n);}
inline bool operator< (const SchedulingEvent& lh, const SchedulingEvent& rh) {
    if (lh._t != rh._t)
        return (lh._t < rh._t);
    if ((lh._p != rh._p))
        return (lh._p < rh._p);
    return (lh._n < rh._n);
}
inline bool operator!=(const SchedulingEvent& lh, const SchedulingEvent& rh) {return (lh._t != rh._t) ||  (lh._p != rh._p) ||  (lh._n != rh._n);}


struct SchedulingEventConstraint {
    SchedulingEvent _s;
    SchedulingEvent _t;
  TIME_UNIT       _w; // Weight is TIME_UNIT and must keep it like that !
    TIME_UNIT       _d;
    ARRAY_INDEX     _id;

    SchedulingEventConstraint(SchedulingEvent s,SchedulingEvent t, TIME_UNIT w, TIME_UNIT d , ARRAY_INDEX id ) : _s(s) , _t(t), _w(w) , _d(d) , _id(id) {}
    std::string toString() const  {return "(" +_s.toString() + ") -> (" + _t.toString() + ") with w=" +  commons::toString<TIME_UNIT>(_w) + " and d=" + commons::toString<TIME_UNIT>(_d);}
};



/* BoostGraph defintion */

typedef boost::property < boost::vertex_index1_t,      unsigned int,   /* vertex_index1_t(task_id)        */
        boost::property < boost::vertex_potential_t,   unsigned int ,  /* vertex_potential_t(SCC INDEX)   */
        boost::property < boost::vertex_predecessor_t, unsigned int ,  /* EventGraphVertex                */
        boost::property < boost::vertex_color_t,       PHASE_INDEX,     /*  vertex_color_t(task's phase)                */
        boost::property < boost::vertex_index2_t,      EXEC_COUNT,    /* vertex_index2_t(occurence d'execution) */
        boost::property < boost::vertex_distance_t,    TIME_UNIT ,  /* delay */
        boost::property < boost::vertex_discover_time_t, TIME_UNIT    /* vertex_discover_time_t(execution time) */
        > > > > > > > EventGraphVertexProperties;

typedef boost::property < boost::edge_weight_t,TIME_UNIT, /* edge_weight_t(w)        */ // Weight is TIME_UNIT and must keep it like that !
        boost::property < boost::edge_index_t,ARRAY_INDEX,  /* edge_index_t(channel_id)        */
        boost::property < boost::edge_color_t,bool,  /* edge_color_t(strictness)        */
        boost::property < boost::edge_weight2_t, TIME_UNIT ,  /* edge_weight2_t (d) */
        boost::property < boost::edge_flow_t, TIME_UNIT > > > > > EventGraphEdgeProperties;  /* edge_flow_t (used to compute starting time) */


typedef boost::property < boost::graph_name_t, std::string  > EventGraphProperties;

typedef boost::adjacency_list<
        boost::vecS, boost::vecS,
        boost::bidirectionalS,
        EventGraphVertexProperties,
        EventGraphEdgeProperties,
        EventGraphProperties > BoostEventGraph;

typedef  boost::graph_traits<BoostEventGraph>::vertex_descriptor EventGraphVertex;
typedef  boost::graph_traits<BoostEventGraph>::edge_descriptor   EventGraphEdge;
typedef  boost::graph_traits<BoostEventGraph>::vertex_iterator   EventGraphVertexIterator;
typedef  boost::graph_traits<BoostEventGraph>::edge_iterator     EventGraphEdgeIterator;
typedef boost::graph_traits<BoostEventGraph>::in_edge_iterator   EventGraphInEdgeIterator;
typedef boost::graph_traits<BoostEventGraph>::out_edge_iterator  EventGraphOutEdgeIterator;

#define ForEachConstraint(g,p)                                                                      \
        models::EventGraphEdge p;                                                                    \
        models::EventGraphEdgeIterator p##count,p##_end;                                             \
        std::pair<models::EventGraphEdgeIterator,models::EventGraphEdgeIterator> p##res = g->getConstraints();  \
        p##count = p##res.first;                                                               \
        p##_end = p##res.second;                                                               \
        if (p##count != p##_end)  p = *(p##count);                                             \
        for (;p##count != p##_end; p = *(++p##count))


#define ForEachEvent(g,p)                                                                      \
        models::EventGraphVertex p;                                                                    \
        models::EventGraphVertexIterator p##count,p##_end;                                             \
        std::pair<models::EventGraphVertexIterator,models::EventGraphVertexIterator> p##res = g->getEvents();  \
        p##count = p##res.first;                                                               \
        p##_end = p##res.second;                                                               \
        if (p##count != p##_end)  p = *(p##count);                                             \
        for (;p##count != p##_end; p = *(++p##count))

#define ForEachInputs(g,parent,p)                                                                        \
        models::EventGraphEdge p;                                                                                \
        models::EventGraphInEdgeIterator p##count,p##_end;                                                       \
        std::pair<models::EventGraphInEdgeIterator,models::EventGraphInEdgeIterator> p##res =  g->getInputs(parent);     \
        p##count = p##res.first;                                                                         \
        p##_end = p##res.second;                                                                         \
        if (p##count != p##_end)  p = *(p##count);                                                       \
        for (;p##count != p##_end; p = *(++p##count))


#define ForEachOutputs(g,parent,p)                                                                        \
        models::EventGraphEdge p;                                                                                \
        models::EventGraphOutEdgeIterator p##count,p##_end;                                                       \
        std::pair<models::EventGraphOutEdgeIterator,models::EventGraphOutEdgeIterator> p##res =  g->getOutputs(parent);     \
        p##count = p##res.first;                                                                         \
        p##_end = p##res.second;                                                                         \
        if (p##count != p##_end)  p = *(p##count);                                                       \
        for (;p##count != p##_end; p = *(++p##count))

#define NULL_EventGraphVertex    ((models::EventGraphVertex)boost::graph_traits<models::BoostEventGraph>::null_vertex())
#define NULL_EventGraphEdge     ((models::EventGraphEdge)boost::detail::edge_desc_impl<boost::bidirectional_tag,models::EventGraphVertex>())

class EventGraph {
private :
    BoostEventGraph              g;
    EventGraphVertex             _root;
private :
    std::map< SchedulingEvent , EventGraphVertex > schedulingEvent2Vertex;
public :
    inline EventGraphVertex getEventGraphVertex(ARRAY_INDEX taskId) const {
    	return getEventGraphVertex(taskId, 1, 1) ;
    }
    inline EventGraphVertex getEventGraphVertex(ARRAY_INDEX taskId, EXEC_COUNT execution) const {
    	return getEventGraphVertex(taskId, 1, execution) ;
    }
    inline EventGraphVertex getEventGraphVertex(ARRAY_INDEX taskId, PHASE_INDEX phase, EXEC_COUNT execution) const {

    	//VERBOSE_ASSERT(schedulingEvent2Vertex.size() > taskId, "Task id " << taskId << " is not within the EventGraph");
    	//VERBOSE_ASSERT(schedulingEvent2Vertex[taskId].size() > phase, "Task phase " << phase << " is not within the EventGraph for task " << taskId);
    	//VERBOSE_ASSERT(schedulingEvent2Vertex[taskId][(unsigned int) phase].size() > execution, "Task execution " << execution << " is not within the EventGraph for task " << taskId << " with phase " << phase);

    	EventGraphVertex res= schedulingEvent2Vertex.at(SchedulingEvent(taskId, phase , execution ));

        VERBOSE_DEBUG_ASSERT(getTaskId(res),   taskId);
        // VERBOSE_DEBUG_ASSERT(getPhase(res),phase); // This can be negative or null now
        VERBOSE_DEBUG_ASSERT(getExecution(res),execution);
        return res;
    }
protected:
    inline  const BoostEventGraph&             getG                ()          const            {return this->g;}
    inline        BoostEventGraph&             getG                ()                           {return this->g;}

public :
    EventGraph       (unsigned int nVertex = 0)       : g(nVertex) , _root(NULL_EventGraphVertex) {VERBOSE_DEBUG_ASSERT(nVertex == 0,TXT_NO_IMPLEMENTATION);}
    void                    FullConnectionned                     () {
        int deleted = 1;
        while (deleted) {

            deleted = 0;

            {ForEachEvent(this,e) {
                unsigned long int  sumout = this->getOutDegree(e);
                unsigned long int  sumin  = this->getInDegree(e);
                if ((sumout == 0) && (sumin != 0)) { // pas de sortie
                    this->removeConnectedEdges(e);
                    //this->removeVertex(e);
                    deleted++;
                    //break;
                }
                if ((sumin == 0) && (sumout != 0)) { // pas de sortie
                    this->removeConnectedEdges(e);
                    //this->removeVertex(e);
                    deleted++;
                    //break;
                }
            }}

        } // end of while

        //{ForEachEvent(this,e) {
        //	unsigned long int  sum = this->getOutDegree(e) + this->getInDegree(e);
        //	if (sum == 0) {
        //		this->removeVertex(e);
        //	}
        //}}

    }
    void removeConnectedEdges(EventGraphVertex v) {
        boost::clear_vertex(v,this->getG());
    }

    void removeVertex(EventGraphVertex v) {
        boost::remove_vertex(v,this->getG());
    }

    long unsigned int  getInDegree(EventGraphVertex v) {
        return boost::in_degree(v,this->getG()) ;
    }
    long unsigned int  getOutDegree(EventGraphVertex v) {
        return boost::out_degree(v,this->getG()) ;
    }
public :
    inline void setStrictness(EventGraphEdge e, bool s)        { boost::put(boost::edge_color, this->getG(), e, s);}
    inline void setFlow(EventGraphEdge e, TIME_UNIT s)        { boost::put(boost::edge_flow, this->getG(), e, s);}
private :
    inline void setWeight(EventGraphEdge e, TIME_UNIT w)       { boost::put(boost::edge_weight, this->getG(), e, w);} // Weight is TIME_UNIT and must keep it like that !
    inline void setDuration(EventGraphEdge e, TIME_UNIT d)     { boost::put(boost::edge_weight2, this->getG(), e, d);}




public :
    inline  void                              setRoot         (const EventGraphVertex r)        {_root = r;}
    inline  EventGraphVertex                  getRoot         ()                                {return _root;}
    inline  EventGraphVertex                  getSource       (const EventGraphEdge c)          {return (boost::source(c, this->getG()));}
    inline  EventGraphVertex                  getTarget       (const EventGraphEdge c)          {return (boost::target(c, this->getG()));}

    inline  std::pair<EventGraphVertexIterator,EventGraphVertexIterator>      getEvents()    const        { return boost::vertices(this->getG());}
    inline  std::pair<EventGraphEdgeIterator,EventGraphEdgeIterator>      getConstraints()    const        { return boost::edges(this->getG());}
    inline  std::pair<EventGraphInEdgeIterator,EventGraphInEdgeIterator>    getInputs(EventGraphVertex t)const  { return boost::in_edges (t,this->getG());}
    inline  std::pair<EventGraphOutEdgeIterator,EventGraphOutEdgeIterator>    getOutputs(EventGraphVertex t) const { return boost::out_edges (t,this->getG());}
    unsigned int  getTaskId(EventGraphVertex v)              const          { return boost::get(boost::get(boost::vertex_index1, this->getG()), v);}
    PHASE_INDEX  getPhase(EventGraphVertex v)         const          { return boost::get(boost::get(boost::vertex_color, this->getG()), v);}
    EXEC_COUNT  getExecution(EventGraphVertex v)       const            { return boost::get(boost::get(boost::vertex_index2, this->getG()), v);}

    void  setStartingTime(EventGraphVertex v, TIME_UNIT s)                  { return boost::put(boost::get(boost::vertex_discover_time, this->getG()), v,s );}
    TIME_UNIT  getStartingTime(EventGraphVertex v)      const             { return boost::get(boost::get(boost::vertex_discover_time, this->getG()), v);}
    EventGraphVertex  getPrevious(EventGraphVertex v)   const                {
        VERBOSE_DEBUG_ASSERT(boost::get(boost::get(boost::vertex_predecessor, this->getG()), v) != NULL_EventGraphVertex  ,"please fix me");
        return (EventGraphVertex) boost::get(boost::get(boost::vertex_predecessor, this->getG()), v);
    }
    void  setPrevious(EventGraphVertex v,EventGraphVertex p)                  { boost::put(boost::vertex_predecessor, this->getG(), v,p);}
    void setSCC(EventGraphVertex v, unsigned int d)       { boost::put(boost::vertex_potential, this->getG(), v, d);}
    unsigned int  getSCC(EventGraphVertex v)               const          { return boost::get(boost::get(boost::vertex_potential, this->getG()), v);}
    SchedulingEvent  getSchedulingEvent(EventGraphVertex v) const { return SchedulingEvent(getTaskId(v),getPhase(v),getExecution(v));}
    ARRAY_INDEX getChannelId(EventGraphEdge e)     const                   { return boost::get(boost::get(boost::edge_index, this->getG()), e);}
    inline void setChannelId(EventGraphEdge e,ARRAY_INDEX id )                       {  boost::put(boost::edge_index, this->getG(), e, id);}
    inline TIME_UNIT getWeight(EventGraphEdge e)    const                   { return boost::get(boost::get(boost::edge_weight, this->getG()), e);} // Weight is TIME_UNIT and must keep it like that !
    inline bool getStrictness(EventGraphEdge e)   const                    { return boost::get(boost::get(boost::edge_color, this->getG()), e);}
    inline TIME_UNIT getFlow(EventGraphEdge e)  const      { return boost::get(boost::edge_flow, this->getG(), e);}

    inline TIME_UNIT  getDuration(EventGraphEdge e)   const                  { return boost::get(boost::get(boost::edge_weight2, this->getG()), e);}
    unsigned int getEventCount() const {return (unsigned  int) boost::num_vertices(this->getG());}
    unsigned int getConstraintsCount() const {return (unsigned  int) boost::num_edges(this->getG());}

    EventGraphVertex getVertexByEvent(SchedulingEvent se)      const   {
        EventGraphVertex res = getEventGraphVertex(se.getTaskId(),se.getTaskPhase(),se.getTaskOc());
        return res;
    }

    EventGraphVertex addEvent(SchedulingEvent se) {
        EventGraphVertex v = boost::add_vertex(this->getG());
        boost::put(boost::vertex_index1, this->getG(), v, se.getTaskId());
        boost::put(boost::vertex_color , this->getG(), v, se.getTaskPhase());
        boost::put(boost::vertex_index2, this->getG(), v, se.getTaskOc());
        schedulingEvent2Vertex[se] = v;
        return v;
    }

    void addEventGroup(ARRAY_INDEX taskid, PHASE_INDEX phase_count, EXEC_COUNT start_ki, EXEC_COUNT end_ki) {

        for (PHASE_INDEX p = 1 ; p <= phase_count ; p++ ) {
             for (EXEC_COUNT k = start_ki ; k <= end_ki ; k++ ) {

                EventGraphVertex v = boost::add_vertex(this->getG());
                boost::put(boost::vertex_index1, this->getG(), v, taskid);
                boost::put(boost::vertex_color, this->getG(), v, p);
                boost::put(boost::vertex_index2, this->getG(), v, k);

                schedulingEvent2Vertex[SchedulingEvent(taskid,p,k)] = v;
            }
        }
    }

    inline void addEventGroup(ARRAY_INDEX taskid, EXEC_COUNT task_phase,  EXEC_COUNT task_ki) { // FIXME : carefull with previous version ...
        addEventGroup(taskid,task_phase,1,task_ki);
    }
    SchedulingEvent getEvent(EventGraphVertex v) {
        return SchedulingEvent(getTaskId(v),getPhase(v),getExecution(v));
    }
    EventGraphEdge addEventConstraint(SchedulingEventConstraint sec) {
        EventGraphVertex source = getVertexByEvent(sec._s);
        EventGraphVertex target = getVertexByEvent(sec._t);
        return addEventConstraint(source,target,sec._w,sec._d, sec._id);

    }
    inline EventGraphEdge addEventConstraint(EventGraphVertex source, EventGraphVertex target,  TIME_UNIT w , TIME_UNIT d, ARRAY_INDEX id ) {
        std::pair<EventGraphEdge,bool> res = boost::add_edge(source, target,this->getG());
        VERBOSE_DEBUG_ASSERT(res.second,TXT_NEW_EDGE_ERROR);
        setWeight(res.first,w);
        setDuration(res.first,d);
        setChannelId(res.first,id);
        return res.first;
    }
    SchedulingEventConstraint getConstraint(EventGraphEdge c) {
        EventGraphVertex source = getSource(c);
        EventGraphVertex target = getTarget(c);
        SchedulingEventConstraint constraint = SchedulingEventConstraint(getEvent(source),
                getEvent(target) ,
                getWeight(c),
                getDuration(c),
                getChannelId(c));

        return constraint;
    }

    std::string printDOT() ;

    ~EventGraph() {};

    TOKEN_UNIT getWeight(SchedulingEvent se1 ,SchedulingEvent se2 )  ;
    struct EdgeProperties {
        int weight;
    };
    bool  computeStartingTimeWithOmega (TIME_UNIT omega ) {

    	if (omega == std::numeric_limits<TIME_UNIT>::infinity()) {
            {ForEachEvent(this,e)   {
                this->setStartingTime(e , 0);
            }}
            return false;
    	}

    	VERBOSE_ASSERT (omega < std::numeric_limits<TIME_UNIT>::infinity(), "Cannot compute starting time with infinite period.");

        VERBOSE_EVENTGRAPH_DEBUG("computeStartingTime omega = " << omega);

        VERBOSE_EVENTGRAPH_DEBUG("computeStartingTime setFlow");
        {ForEachConstraint(this,e)   {
            auto flow = ( this->getDuration(e)  - omega * this->getWeight(e));
            VERBOSE_EVENTGRAPH_DEBUG(" Edge " << e << ":" << this->getConstraint(e).toString() << " Flow = " << flow);
            this->setFlow(e, flow );
        }}
        // gets the weight property
        //boost::property_map<BoostEventGraph, boost::edge_flow_t>::type weight_pmap = get(boost::edge_flow_t(), g);
        //boost::property_map<BoostEventGraph, boost::vertex_discover_time_t>::type distance_pmap = get(boost::vertex_discover_time_t(), g);

        VERBOSE_EVENTGRAPH_DEBUG("computeStartingTime initStartingTime");
        bool first = true;
        //TIME_UNIT defaultmin = - std::numeric_limits<TIME_UNIT>::infinity();
        TIME_UNIT defaultmin = 0;

        std::queue<models::EventGraphVertex> worklist;
        std::vector<bool> in_queue(this->getEventCount(), false);
        std::vector<size_t> visit_count(this->getEventCount(), 0);

        {ForEachEvent(this,e)   {
        	auto startingtime = defaultmin;
            if (first) {
                first = false;
                startingtime = 0;
            }
            this->setStartingTime(e,startingtime);
            worklist.push(e);
            in_queue[e] = true;
            visit_count[e] = 1;
            VERBOSE_EVENTGRAPH_DEBUG(" Event " << e << ":" << this->getEvent(e).toString() << " Start = " << startingtime);
        }}

        VERBOSE_EVENTGRAPH_DEBUG("computeStartingTime getStartingTime with worklist-based approach");
        while (!worklist.empty()) {
            models::EventGraphVertex event1 = worklist.front();
            worklist.pop();
            in_queue[event1] = false;

            {ForEachOutputs(this,event1,constraint){
                models::EventGraphVertex event2 = this->getTarget(constraint);
                auto previous_start1 = this->getStartingTime(event1);
                auto previous_start2 = this->getStartingTime(event2);

                if (previous_start2 - previous_start1 < this->getFlow(constraint)) {
                    this->setStartingTime(event2, this->getFlow(constraint) + previous_start1);
                    if (!in_queue[event2]) {
                        worklist.push(event2);
                        in_queue[event2] = true;
                        visit_count[event2]++;
                        if (visit_count[event2] > this->getEventCount()) {
                            VERBOSE_ERROR("Positive cycle detected in computeStartingTimeWithOmega with omega = " << omega);
                            return false;
                        }
                    }
                }
            }}
        }


        VERBOSE_EVENTGRAPH_DEBUG("computeStartingTime min_time ");

        TIME_UNIT min_time = (std::numeric_limits<TIME_UNIT>::max)();


        {ForEachEvent(this,e)   {
            min_time = std::min (min_time , this->getStartingTime(e));
        }}

        VERBOSE_EVENTGRAPH_DEBUG(" min_time = " << min_time);

        {ForEachEvent(this,e)   {
        	auto old_start = this->getStartingTime(e);
        	auto new_start = old_start  - min_time ;
            VERBOSE_EVENTGRAPH_DEBUG("  Event " << e << ":"<< this->getEvent(e).toString() << " Adjusted Start from " << old_start << " to " << new_start );
            this->setStartingTime(e , new_start);
        }}

        VERBOSE_EVENTGRAPH_DEBUG("End of compute starts.");
        return true;

    }

    std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> >  MaxCycleRatio () {

        std::vector<models::EventGraphEdge>   cc;

        boost::property_map<BoostEventGraph, boost::vertex_index_t>::type vim = boost::get(boost::vertex_index, this->getG());
        boost::property_map<BoostEventGraph, boost::edge_weight_t>::type  ew1 = boost::get(boost::edge_weight,  this->getG());
        boost::property_map<BoostEventGraph, boost::edge_weight2_t>::type ew2 = boost::get(boost::edge_weight2, this->getG());

        //int i = 0;
        //boost::graph_traits<BoostEventGraph>::vertex_iterator vi, vi_end;
        // for (boost::tie(vi, vi_end) = vertices(this->getG()); vi != vi_end; vi++) {
        //        vim[*vi] = i++; ///Initialize vertex index property
        //    }

        cc.clear();
        TIME_UNIT max_cr = boost::maximum_cycle_ratio(this->getG(), vim, ew1, ew2, &cc,TIME_UNIT_LIMITS());
        VERBOSE_INFO("maximum cycle ratio is " << max_cr);
        std::pair<TIME_UNIT, TIME_UNIT> cr(.0,.0);
        for (std::vector<models::EventGraphEdge>::iterator itr = cc.begin(); itr != cc.end(); ++itr)
        {
            cr.first  += ew1[*itr];
            cr.second += ew2[*itr];
            VERBOSE_EVENTGRAPH_DEBUG( "   (" << vim[boost::source(*itr, this->getG())] << "," <<
                    vim[boost::target(*itr, this->getG())] << ") ");
        }
        if (std::abs(cr.first / cr.second - max_cr) > TIME_UNIT_LIMITS::epsilon()) {VERBOSE_ERROR("Huge bug in MaxCRP solving function.");}
        return  std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> >(max_cr,cc);
    }


    std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> >  MinCycleRatio (TIME_UNIT bound) {

        std::vector<models::EventGraphEdge>   cc;
        VERBOSE_INFO("minimum cycle ratio bound was  " << bound);

        boost::property_map<BoostEventGraph, boost::vertex_index_t>::type vim = boost::get(boost::vertex_index, this->getG());
        boost::property_map<BoostEventGraph, boost::edge_weight_t>::type ew1  = boost::get(boost::edge_weight,  this->getG());
        boost::property_map<BoostEventGraph, boost::edge_weight2_t>::type ew2 = boost::get(boost::edge_weight2, this->getG());

        cc.clear();
        TIME_UNIT max_cr = boost::minimum_cycle_ratio(this->getG(), vim, ew1, ew2, bound, &cc,TIME_UNIT_LIMITS());
        VERBOSE_INFO("minimum cycle ratio is " << max_cr);
        std::pair<TIME_UNIT, TIME_UNIT> cr(.0,.0);
        for (std::vector<models::EventGraphEdge>::iterator itr = cc.begin(); itr != cc.end(); ++itr)
        {
            cr.first  += ew1[*itr];
            cr.second += ew2[*itr];
            VERBOSE_EVENTGRAPH_DEBUG( "   (" << vim[boost::source(*itr, this->getG())] << "," <<
                    vim[boost::target(*itr, this->getG())] << ") ");
        }
        if (std::abs(cr.first / cr.second - max_cr) > TIME_UNIT_LIMITS::epsilon()) {VERBOSE_ERROR("Huge bug in MinCRP solving function.");}
        return  std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> >(max_cr,cc);
    }


    std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> >  MinCycleRatio () {

        std::vector<models::EventGraphEdge>   cc;


        boost::property_map<BoostEventGraph, boost::vertex_index_t>::type vim = boost::get(boost::vertex_index, this->getG());
        boost::property_map<BoostEventGraph, boost::edge_weight_t>::type ew1  = boost::get(boost::edge_weight,  this->getG());
        boost::property_map<BoostEventGraph, boost::edge_weight2_t>::type ew2 = boost::get(boost::edge_weight2, this->getG());

        cc.clear();
        TIME_UNIT max_cr = boost::minimum_cycle_ratio(this->getG(), vim, ew1, ew2, &cc,TIME_UNIT_LIMITS());
        VERBOSE_EVENTGRAPH_DEBUG("minimum cycle ratio is " << max_cr);
        std::pair<TIME_UNIT, TIME_UNIT> cr(.0,.0);
        for (std::vector<models::EventGraphEdge>::iterator itr = cc.begin(); itr != cc.end(); ++itr)
        {
            cr.first  += ew1[*itr];
            cr.second += ew2[*itr];
            VERBOSE_EVENTGRAPH_DEBUG( "   " << vim[boost::source(*itr, this->getG())] << " -> " <<
                    vim[boost::target(*itr, this->getG())] << " : (" << ew1[*itr] << "," << ew2[*itr] << ")");
        }
        if (std::abs(cr.first / cr.second - max_cr) > TIME_UNIT_LIMITS::epsilon()) {VERBOSE_ERROR("Huge bug in MinCRP solving function.");}
        return  std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> >(max_cr,cc);
    }




    std::string printXML    ();
    std::string printTikz    () ;

    EventGraphVertex getFirstEvent() { return 0;};
};
void freeStrongConnection ( std::map<unsigned int,EventGraph*>*  ) ;
std::map<unsigned int,EventGraph*>* StrongConnection(models::EventGraph * g)  ;




}

#endif /* EVENTGRAPH_H_ */
