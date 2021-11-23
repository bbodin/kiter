/*
 * dataflow.h
 *
 *  Created on: 16 avr. 2013
 *      Author: toky
 */

#ifndef DATAFLOW_H_
#define DATAFLOW_H_

#include <vector>
#include <numeric>
#include <iostream>
#include <sstream>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include <boost/version.hpp>
#include <stdexcept>
#include <string>
#include <commons/commons.h>
#include <commons/verbose.h>
#include <models/NoC.h>
#include <models/repetition_vector.h>

#define TXT_NEW_EDGE_ERROR "NEW_EDGE_ERROR"
#define TXT_TASK_NOT_FOUND "TASK_NOT_FOUND"
#define TXT_CHANNEL_NOT_FOUND "CHANNEL_NOT_FOUND"


#define VERBOSE_DATAFLOW_DEBUG(msg) {VERBOSE_CUSTOM_DEBUG("Dataflow",msg);}

/* Dataflow defintion */

enum EDGE_TYPE   {NORMAL_EDGE, BUFFERLESS_EDGE, VIRTUAL_EDGE};
//enum VERTEX_TYPE {NORMAL_VERTEX, PERIODIC_VERTEX};

typedef  std::string VERTEX_TYPE;

namespace boost
{

	enum vertex_phasecount_t { vertex_phasecount      };
	enum vertex_initphasecount_t { vertex_initphasecount      };
	enum vertex_phaseduration_t { vertex_phaseduration      };
	enum vertex_initphaseduration_t { vertex_initphaseduration      };
	enum vertex_Zi_t { vertex_Zi      };
	enum vertex_Ni_t { vertex_Ni      };
	enum vertex_mapping_t { vertex_mapping      };
	enum vertex_reentrancy_t { vertex_reentrancy      };
	enum vertex_type_t       { vertex_type      };
	enum edge_input_port_name_t        { edge_input_port_name             };
	enum edge_output_port_name_t       { edge_output_port_name            };
	enum edge_inputs_t        { edge_inputs             };
	enum edge_outputs_t       { edge_outputs            };
	enum edge_init_inputs_t        { edge_init_inputs             };
	enum edge_init_outputs_t       { edge_init_outputs            };
	enum edge_alpha_t       { edge_alpha            };
	enum edge_total_input_t        { edge_total_input             };
	enum edge_total_output_t       { edge_total_output            };
	enum edge_preload_t       { edge_preload          };
	enum edge_tokensize_t       { edge_tokensize      };
	enum edge_type_t       { edge_type      };
	enum edge_route_t       { edge_route      };
	enum graph_filename_t    { graph_filename         };

	BOOST_INSTALL_PROPERTY(vertex,   phasecount  );
	BOOST_INSTALL_PROPERTY(vertex,   initphasecount  );
	BOOST_INSTALL_PROPERTY(vertex,   phaseduration  );
	BOOST_INSTALL_PROPERTY(vertex,   initphaseduration  );
	BOOST_INSTALL_PROPERTY(vertex,   mapping  );
	BOOST_INSTALL_PROPERTY(vertex,   Zi  );
	BOOST_INSTALL_PROPERTY(vertex,   Ni  );
	BOOST_INSTALL_PROPERTY(vertex,   reentrancy  );
	BOOST_INSTALL_PROPERTY(vertex,   type  );
	BOOST_INSTALL_PROPERTY(edge,   inputs  );
	BOOST_INSTALL_PROPERTY(edge,   outputs );
	BOOST_INSTALL_PROPERTY(edge,   init_inputs  );
	BOOST_INSTALL_PROPERTY(edge,   init_outputs );
	BOOST_INSTALL_PROPERTY(edge,   alpha );
	BOOST_INSTALL_PROPERTY(edge,   total_input  );
	BOOST_INSTALL_PROPERTY(edge,   total_output );
	BOOST_INSTALL_PROPERTY(edge,   input_port_name  );
	BOOST_INSTALL_PROPERTY(edge,   output_port_name );
	BOOST_INSTALL_PROPERTY(edge,   preload );
	BOOST_INSTALL_PROPERTY(edge,   tokensize );
	BOOST_INSTALL_PROPERTY(edge,   route );
	BOOST_INSTALL_PROPERTY(edge,   type );
	BOOST_INSTALL_PROPERTY(graph,filename );
}



typedef boost::property < boost::vertex_name_t,       std::string ,                              // Task name
		boost::property < boost::vertex_index2_t,     ARRAY_INDEX   ,                            // Task id
		boost::property < boost::vertex_Zi_t,     TOKEN_UNIT   ,                            // NORMALIZATION : ZI
		boost::property < boost::vertex_mapping_t,     node_id_t   ,                            // mapping task to core id
		boost::property < boost::vertex_Ni_t,     EXEC_COUNT   ,                            // Repetition Vector : Ni
		boost::property < boost::vertex_phaseduration_t, std::vector<TIME_UNIT>   ,             // Task phase durations
		boost::property < boost::vertex_initphaseduration_t, std::vector<TIME_UNIT>   ,             // Task phase durations
		boost::property < boost::vertex_reentrancy_t,     EXEC_COUNT   ,                         // maximum reentacy (0 : never)
		boost::property < boost::vertex_type_t,     VERTEX_TYPE   ,                         // vertex type
		boost::property < boost::vertex_initphasecount_t, EXEC_COUNT    ,                         // task Init phase count
		boost::property < boost::vertex_phasecount_t, EXEC_COUNT    > > > > > > > > > > > vertexProperties;  // Task phase count

typedef boost::property < boost::edge_name_t, std::string,                                    // buffer name
		boost::property < boost::edge_input_port_name_t, std::string,                         // input port name
		boost::property < boost::edge_output_port_name_t, std::string,                        // output port name
		boost::property < boost::edge_inputs_t, std::vector<TOKEN_UNIT>   ,                    // production phases
		boost::property < boost::edge_outputs_t,std::vector<TOKEN_UNIT>   ,                    // consumption phases
		boost::property < boost::edge_init_inputs_t, std::vector<TOKEN_UNIT>   ,                    // production phases
		boost::property < boost::edge_init_outputs_t,std::vector<TOKEN_UNIT>   ,                    // consumption phases
		boost::property < boost::edge_total_input_t,TOKEN_UNIT   ,                                // total production
		boost::property < boost::edge_total_output_t,TOKEN_UNIT   ,                                // total consumption
		boost::property < boost::edge_preload_t,TOKEN_UNIT   ,                                // initial marking
		boost::property < boost::edge_alpha_t,TOKEN_FRACT   ,                                // NORMALIZATION : ALPHA
		boost::property < boost::edge_tokensize_t,TOKEN_UNIT   ,                                // token size
		boost::property < boost::edge_type_t,EDGE_TYPE   ,                                // edge type
		boost::property < boost::edge_route_t,std::vector<edge_id_t>   ,                                // edge route
		boost::property < boost::edge_index_t, ARRAY_INDEX > > > > > > > > > > > > > > >  edgeProperties;           // buffer id


typedef boost::property < boost::graph_name_t, std::string,                                   // Graph name
		boost::property < boost::graph_filename_t, std::string> > graphProperties;            // Graph source file name

typedef boost::adjacency_list<
		boost::vecS, boost::vecS,
		boost::bidirectionalS,
		vertexProperties,
		edgeProperties,
		graphProperties > BoostDataflow;

typedef  boost::graph_traits<BoostDataflow>::vertex_descriptor VertexD;
typedef  boost::graph_traits<BoostDataflow>::edge_descriptor   EdgeD;

typedef boost::graph_traits<BoostDataflow>::edge_iterator   	edge_iterator;
typedef boost::graph_traits<BoostDataflow>::vertex_iterator 	vertex_iterator;

typedef boost::graph_traits<BoostDataflow>::in_edge_iterator  	in_edge_iterator;
typedef boost::graph_traits<BoostDataflow>::out_edge_iterator 	out_edge_iterator;


#define NULL_VERTEXD	(boost::graph_traits<BoostDataflow>::null_vertex())
#define NULL_EDGED	 	((EdgeD)boost::detail::edge_desc_impl<boost::bidirectional_tag,VertexD>())


struct Vertex {
	typedef vertex_iterator iterator ;
	VertexD v;
	Vertex(VertexD in = NULL_VERTEXD) : v(in) {}

};
struct Edge	  {
	typedef edge_iterator   iterator ;
	EdgeD   e;
	Edge  (EdgeD   in = NULL_EDGED)   : e(in)  {}
};


/* TODO MEMORY : INVALID READ SIZE HERE */

inline 	Vertex 	it2Vertex(vertex_iterator   in) 		{return Vertex(*in);}
inline 	Edge 	it2Edge  (edge_iterator     in) 	 	{return Edge(*in);}
inline 	Edge 	it2Edge  (in_edge_iterator  in) 	  	{return Edge(*in);}
inline 	Edge 	it2Edge  (out_edge_iterator in) 	  	{return Edge(*in);}




inline bool operator<(const Vertex& lh, const Vertex& rh) {return ((int) lh.v < (int) rh.v);}
inline bool operator<(const Edge& lh, const Edge& rh) {return (lh.e.get_property() < rh.e.get_property());}

inline bool operator==(const Vertex& lh, const Vertex& rh) {return (lh.v == rh.v);}
inline bool operator==(const Edge& lh, const Edge& rh) {return (lh.e == rh.e);}

inline bool operator!=(const Vertex& lh, const Vertex& rh) {return (lh.v != rh.v);}
inline bool operator!=(const Edge& lh, const Edge& rh) {return (lh.e != rh.e);}

inline std::ostream& operator<< (std::ostream &out,const Edge& e)		{out << e.e; return out;}
inline std::ostream& operator<< (std::ostream &out,const Vertex& v)		{out << v.v; return out;}

template <class T,class T_IT>
struct tie {
	T& v;
	T_IT& l,r;

	tie(T&v,T_IT& l, T_IT& r) : v(v), l(l), r(r) {}


	tie& operator=(const std::pair<T_IT, T_IT> k) {
	    this->l   =  k.first;
	    this->r   =  k.second;
	    this->v   = T(*(k.first));
	    return *this;
	}

};


#define ForEachTask(g,p) ForEachVertex(g,p)
#define ForEachChannel(g,p) ForEachEdge(g,p)

#define ForEachVertex(g,p)															       \
		Vertex p;	   																       \
		std::pair<Vertex::iterator,Vertex::iterator> p##res = g->getVertices();		       \
		Vertex::iterator  p##count = p##res.first;													       \
		Vertex::iterator  p##_end = p##res.second;													       \
		if (p##count != p##_end)  p = it2Vertex(p##count);				     			   \
		for (;p##count != p##_end; p = it2Vertex(++p##count))

#define ForEachEdge(g,p)															       \
		Edge p;	   																	       \
		std::pair<Edge::iterator,Edge::iterator> p##res = g->getEdges();			       \
		Edge::iterator p##count = p##res.first;													       \
		Edge::iterator p##_end = p##res.second;													       \
		if (p##count != p##_end)  p = it2Edge(p##count);				     			   \
		for (;p##count != p##_end; p = it2Edge(++p##count))

#define ForInputEdges(g,parent,p)													       \
		Edge p;	   																	       \
		std::pair<in_edge_iterator,in_edge_iterator> p##res =  g->getInputEdges(parent);   \
		in_edge_iterator p##count = p##res.first;													       \
		in_edge_iterator p##_end = p##res.second;													       \
		if (p##count != p##_end)  p = it2Edge(p##count);				     			   \
	    for (;p##count++ != p##_end; p =  (p##count != p##_end)?it2Edge(p##count):Edge())

#define ForOutputEdges(g,parent,p)													       \
		Edge p;	   																	       \
		std::pair<out_edge_iterator,out_edge_iterator> p##res =  g->getOutputEdges(parent);\
		out_edge_iterator p##count = p##res.first;													       \
		out_edge_iterator p##_end = p##res.second;													       \
		if (p##count != p##_end)  p = it2Edge(p##count);				     			   \
		for (;p##count++ != p##_end; p =  (p##count != p##_end)?it2Edge(p##count):Edge())

// TODO gerer le cas du noeud sans arc
#define ForConnectedEdges(g,parent,p)                                                      \
        Edge p;                                                                            \
        std::pair<Edge::iterator,Edge::iterator> p##res = g->getEdges();                   \
        Edge::iterator p##count = p##res.first;                                                           \
        Edge::iterator p##_end = p##res.second;                                                           \
        if (p##count != p##_end)  p = it2Edge(p##count);                                   \
        for (;p##count != p##_end; p = it2Edge(++p##count))                                \
        if ((g->getEdgeSource(p) == parent) || (g->getEdgeTarget(p) == parent))

#define ForEachPhase(g,v,p)\
        EXEC_COUNT __max##p = g->getPhasesQuantity(v);\
        for(EXEC_COUNT p = 1 ; p <= __max##p;p++)

#define NULL_VERTEX	 Vertex()
#define NULL_EDGE	 Edge  ()

#define ASSERT_WRITABLE() if (readonly) FAILED("ASSERT_WRITABLE FAILED");
#define ASSERT_NOT_NORMALIZED() if (normalizationisdone) FAILED("ASSERT_NOT_NORMALIZED FAILED");
#define ASSERT_NOT_REPETITION_VECTOR() if (repetitionvectorisdone) FAILED("ASSERT_NOT_REPETITION_VECTOR FAILED");

#define ASSERT_NOT_WRITABLE() if (!readonly) FAILED("ASSERT_NOT_WRITABLE FAILED");
#define ASSERT_NORMALIZED() if (!normalizationisdone) FAILED("ASSERT_NORMALIZED FAILED");
#define ASSERT_REPETITION_VECTOR() if (!repetitionvectorisdone) FAILED("ASSERT_REPETITION_VECTOR FAILED");

namespace models {

class Dataflow {

private :
	bool          readonly;
	bool          normalizationisdone;
	bool          repetitionvectorisdone;
	BoostDataflow g;
    std::string   graph_name;
    std::string   graph_type;
    std::string   app_name;
    ARRAY_INDEX   graph_id;
    std::string   filename;
	ARRAY_INDEX   auto_vertex_num;
	ARRAY_INDEX   auto_edge_num;
	TIME_UNIT     normalized_period;
	NoC           noc;


	/* all about getters */
public:
	inline  Edge                    getFirstEdge        ()                  const   {return it2Edge(boost::edges(this->getG()).first);}
	inline  Vertex					getEdgeSource		(const Edge c)		const	{return Vertex(boost::source(c.e, this->getG()));}
	inline  Vertex					getEdgeTarget		(const Edge c)		const	{return Vertex(boost::target(c.e, this->getG()));}
	inline  ARRAY_INDEX			    getVerticesCount 	()					const	{return (unsigned int) boost::num_vertices(this->getG());}
	inline  ARRAY_INDEX  			getEdgesCount		()					const	{return (unsigned int) boost::num_edges(this->getG());}
	inline  unsigned int			getVertexDegree		(const Vertex t)	const	{return (unsigned int) boost::degree(t.v,this->getG());}
	inline  unsigned int			getVertexInDegree	(const Vertex t)	const	{return (unsigned int) boost::in_degree(t.v,this->getG());}
	inline  unsigned int			getVertexOutDegree	(const Vertex t)	const	{return (unsigned int) boost::out_degree(t.v,this->getG());}



protected:
	inline        BoostDataflow& 	getG				()						{return this->g;}
	inline  const BoostDataflow& 	getG				()			const		{return this->g;}


public :
	Dataflow		(unsigned int nVertex = 0)		: readonly(false), normalizationisdone(false), repetitionvectorisdone(false),
	g(nVertex), graph_name(""),graph_type(""),app_name(""), graph_id(0) ,  auto_vertex_num (1) , auto_edge_num (1) ,
	normalized_period(0), noc (0,0) {
		VERBOSE_ASSERT(nVertex == 0,TXT_NO_IMPLEMENTATION);
	}

	const NoC& getNoC() const { return this->noc;};
	void       setNoC(NoC& noc) { this->noc = noc; };

	void set_read_only() {readonly = true;}
	void set_normalize() {normalizationisdone = true;}
	void set_repetition_vector() {repetitionvectorisdone = true;}

	void reset_computation() {repetitionvectorisdone = false; normalizationisdone = false; readonly = false; }

	bool is_read_only() const {return readonly;}
	bool is_normalized() const {return normalizationisdone ;}
	bool has_repetition_vector() const {
		return repetitionvectorisdone ;
	}
	bool is_consistent()  {
		if (!repetitionvectorisdone) computeRepetitionVector(this);
		return repetitionvectorisdone ;
	}

public :

inline	 Edge addEdge(const Vertex from, const Vertex to) {
	// TODO: fixme too long
	ForEachEdge(this,e) 	{ if (this->getEdgeId(e) >= auto_edge_num) auto_edge_num = this->getEdgeId(e) + 1;};
	return addEdgeUnsafe(from , to , auto_edge_num);
}

inline	 Edge addEdge(const Vertex from, const Vertex to, const  ARRAY_INDEX id) {
	return addEdgeUnsafe(from , to , id);
}

inline	 Edge addEdge(const Vertex from, const Vertex to, const  ARRAY_INDEX id, const std::string name) {
	Edge ne = addEdge(from , to , id);
	this->setEdgeName(ne,name);
	return ne;
}

inline	 Edge addEdge(const Vertex from, const Vertex to, const std::string name) {
	Edge ne = addEdge(from , to);
	this->setEdgeName(ne,name);
	return ne;
}


inline	 Edge addEdgeUnsafe(const Vertex from, const Vertex to, const  ARRAY_INDEX id) {
	ASSERT_WRITABLE();
	reset_computation();
	std::pair<EdgeD,bool> res = boost::add_edge( from.v, to.v,this->getG());
	VERBOSE_ASSERT(res.second,TXT_NEW_EDGE_ERROR);
	Edge newChannel = Edge(res.first);
	this->setEdgeId(newChannel,id);
	this->setEdgeType(newChannel,EDGE_TYPE::NORMAL_EDGE);
	return newChannel;
}


inline 	Vertex 				addVertex			() 							{
	// TODO: fixme too long
	ForEachVertex(this,v) 	{ if (this->getVertexId(v) >= auto_vertex_num) auto_vertex_num = this->getVertexId(v) + 1;};
	return addUnsafeVertex(auto_vertex_num++);
}

inline  Vertex                addVertex         (const ARRAY_INDEX id)      {
	// TODO: fixme too long
	ForEachVertex(this,v) 	{ if (this->getVertexId(v) >= id) VERBOSE_FAILURE();};
	return addUnsafeVertex(id);

}

inline  Vertex                addVertex         (const ARRAY_INDEX id, const std::string name)      {
	Vertex nt = addVertex(id);
	this->setVertexName(nt,name);
	return nt;

}
inline  Vertex                addVertex         (const std::string name)      {
	Vertex nt = addVertex();
	this->setVertexName(nt,name);
	return nt;

}

inline  Vertex                addUnsafeVertex         (const ARRAY_INDEX id)      {

	ASSERT_WRITABLE();
	reset_computation();
	Vertex nt = Vertex(boost::add_vertex(this->getG()));
	this->setVertexId(nt,id);
	//this->setVertexName(nt,"Vertex" + std::to_string(this->getVertexId(nt)));
	this->setInitPhasesQuantity(nt,0);
	this->setMapping(nt,-1);
	return nt;
}


	inline 	void 					removeVertex		(const Vertex t) 		{
		ASSERT_WRITABLE();
		reset_computation();
		while (getVertexInDegree(t)) {
			 this->removeEdge(*this->getInputEdges(t).first);
		}

		while (getVertexOutDegree(t)) {
			 this->removeEdge(*this->getOutputEdges(t).first);
		}

		boost::remove_vertex(t.v,this->getG());
	}
	inline  void					removeEdge   		(const Edge c)			{
		ASSERT_WRITABLE();
		reset_computation();
		boost::remove_edge(c.e,this->getG());
	}

public :
	inline  const boost::integer_range<VertexD>             vertices()        const { return this->getG().vertex_set() ;} // This function is added because it can be used with foreach.
	inline  const boost::integer_range<Edge::iterator>      edges()           const {
		const std::pair<Edge::iterator,Edge::iterator> mypair = boost::edges(this->getG());
		return boost::integer_range<Edge::iterator>(mypair.first, mypair.second);
		//return this->getG().vertex_set() ;
	} // This function is added because it can be used with foreach.
	inline  std::pair<edge_iterator,edge_iterator>   		getEdges() 				const { return boost::edges(this->getG());}
	inline  std::pair<vertex_iterator,vertex_iterator>  	getVertices()    		const { return boost::vertices(this->getG());}
	inline  std::pair<in_edge_iterator,in_edge_iterator>  	getInputEdges(Vertex t) const { return boost::in_edges (t.v,this->getG());}
	inline  std::pair<out_edge_iterator,out_edge_iterator>	getOutputEdges(Vertex t) const { return boost::out_edges(t.v,this->getG());}
	inline  const boost::integer_range<in_edge_iterator>  	in_edges(Vertex t) const {
		const std::pair<in_edge_iterator,in_edge_iterator> mypair = boost::in_edges(t.v,this->getG());
		return boost::integer_range<in_edge_iterator>(mypair.first, mypair.second);
	}
	inline   const boost::integer_range<out_edge_iterator> 	out_edges(Vertex t)const {
		const std::pair<out_edge_iterator,out_edge_iterator> mypair = boost::out_edges(t.v,this->getG());
		return boost::integer_range<out_edge_iterator>(mypair.first, mypair.second);
	}



public :
    inline  Vertex                  getFirstVertex      ()                   const    {return *(this->getVertices().first);}
	inline 	ARRAY_INDEX 			getVertexId			(const Vertex t)	const	{return boost::get(get(boost::vertex_index2, this->getG()), t.v); }
	inline 	ARRAY_INDEX 			getEdgeId			(const Edge c)		const	{return boost::get(get(boost::edge_index, this->getG()), c.e);}


	inline	Vertex					getVertexById		(const ARRAY_INDEX id) const	{
		// TODO: fixme too long
		ForEachVertex(this,pVertex){ if (this->getVertexId(pVertex) == id) return pVertex;};
		throw std::out_of_range(TXT_TASK_NOT_FOUND);
	}

	inline	Edge					getEdgeById			(const ARRAY_INDEX id) const	{
		// TODO: fixme too long
		ForEachEdge(this,pEdge) 	{ if (this->getEdgeId(pEdge) == id) return pEdge;};throw std::out_of_range(TXT_CHANNEL_NOT_FOUND);}


	inline  void 				setVertexId		(const Vertex t,
													 const ARRAY_INDEX id) 				{
		ASSERT_WRITABLE();
		reset_computation();
		    try{
		        Vertex ot = getVertexById(id);
		    if (ot  != t)
		        FAILED("VERTEX ID ALREADY USED");
		    } catch(std::out_of_range& e ) {
		    	setVertexIdUnsafe(t,id);
		    }

	}
		inline  void 				setVertexIdUnsafe		(const Vertex t,
													 const ARRAY_INDEX id) 				{
			ASSERT_WRITABLE();
			reset_computation();

				boost::put(boost::vertex_index2, this->getG(), t.v, id);
		        auto_vertex_num = std::max(auto_vertex_num,id+1);

	}
		inline 	void 				setEdgeId	(const Edge c,
													 const ARRAY_INDEX id)				{
			ASSERT_WRITABLE();
			reset_computation();
			VERBOSE_ASSERT(id > 0,"null edge id is forbidded");
			try{
				Edge oc =getEdgeById(id);
				if (oc  != c) FAILED("EDGE ID ALREADY USED");
			} catch(std::out_of_range& e) {
				boost::put(boost::edge_index, this->getG(), c.e, id);
				auto_edge_num = std::max(auto_edge_num,id+1);
			}
		}



	inline bool                     edgeExist       (const Vertex from,
	                                                 const Vertex to)     const      {return boost::edge(from.v,to.v,this->getG()).second;}


public :
    void setFilename (std::string f) {filename = f;}
    std::string getFilename () const { return filename;}
public:
    inline  void                setGraphName    (const std::string name)    {					ASSERT_WRITABLE();
	reset_computation();this->graph_name = name;}
    inline  void                setAppName    (const std::string name)    {					ASSERT_WRITABLE();
	reset_computation();this->app_name = name;}
    inline  void                setGraphType    (const std::string name)    {					ASSERT_WRITABLE();
	reset_computation();this->graph_type = name;}
    inline  const std::string   getGraphName    ()                      const     {return this->graph_name;}
    inline  const std::string   getGraphType   ()                      const     {return this->graph_type;}
    inline  const std::string   getAppName    ()                      const     {return this->app_name;}
    inline  void                setId      (ARRAY_INDEX id)           {					ASSERT_WRITABLE();
	reset_computation();this->graph_id = id;}
    inline  ARRAY_INDEX         getId      ()             const             {return this->graph_id;}


public :
    inline  ARRAY_INDEX         getMaxEdgeId 	()      const     		{return auto_edge_num; }
    inline  ARRAY_INDEX         getMaxVertexId  ()      const           {return auto_vertex_num; }
    inline  void                setVertexName   (const Vertex t,
                                                 const std::string name){
    	ASSERT_WRITABLE();
    	reset_computation();
    	VERBOSE_ASSERT(name != "", "Empty name is not permit for vertex.");
    	try {
    		Vertex other = this->getVertexByName(name);
    		VERBOSE_ASSERT(other == t , "Cannot name two vertex the same: " << name);
    	} catch (std::out_of_range& e) {

    	}
    	boost::put(boost::vertex_name, this->getG(), t.v, name);
    }
    /*
     * TODO: Be careful tasks can be unamed from now.
     */
    inline  const std::string   getVertexName   (const Vertex t)      const {

    	std::string name = boost::get(get(boost::vertex_name, this->getG()), t.v);
    	//VERBOSE_ASSERT(name != "", "Empty name is not permit for vertex.");
    	return name;
    }


    inline  Vertex              getVertexByName (const std::string s) const {
    	// TODO: FIXME PLEASE!
    	ForEachVertex(this,pVertex) {if (this->getVertexName(pVertex) == s)return pVertex;};throw std::out_of_range(TXT_TASK_NOT_FOUND + s);
    }

    inline 	const std::string 	getEdgeName	    (const Edge c)		const	{
    		std::string s = boost::get(get(boost::edge_name, this->getG()), c.e);
        	//VERBOSE_ASSERT(s != "", "Empty name is not permit for edges.");
    		return s;
    }

    inline  Edge                getEdgeByName   (const std::string s) const {
    	// TODO: fixme too long
    	{ForEachEdge(this,pEdge)     {
    		if (this->getEdgeName(pEdge) == s) return pEdge;
    	}};
    	throw std::out_of_range(TXT_CHANNEL_NOT_FOUND  + s);
    }
    inline  void                setEdgeName     (const Edge c,
                                                 const std::string name)    {
		ASSERT_WRITABLE();
		reset_computation();
    	VERBOSE_ASSERT(name != "", "Empty name is not permit for edges.");

    	try {
    	    		Edge other = this->getEdgeByName(name);
    	    		VERBOSE_ASSERT(other == c , "Cannot name two edges the same: " << name);
    	} catch (std::out_of_range& e) {

    	}

    	boost::put(boost::edge_name, this->getG(), c.e, name);
    }





public :

    inline void                 setEdgeType (const Edge e,
                                           const EDGE_TYPE t)    {
		ASSERT_WRITABLE();
		reset_computation();
		boost::put(boost::edge_type, this->getG(), e.e, t);}
    inline EDGE_TYPE           getEdgeType (const Edge e )    const     {return boost::get(get(boost::edge_type, this->getG()), e.e);}
    inline std::string           getEdgeTypeStr (const Edge e )    const     {
    	EDGE_TYPE et = this->getEdgeType(e);
    	switch (et) {
    		case NORMAL_EDGE : return "NORMAL_EDGE";
    		case  BUFFERLESS_EDGE : return "BUFFERLESS_EDGE";
    		default : return "UNKNOWN";
    	}
    }


    inline void                 setVertexType (const Vertex v,
                                           const VERTEX_TYPE t)    {
		ASSERT_WRITABLE();
		reset_computation();
		boost::put(boost::vertex_type, this->getG(), v.v, t);
    }
    inline VERTEX_TYPE           getVertexType (const Vertex v )    const     {
    	return boost::get(get(boost::vertex_type, this->getG()), v.v);
    }


    inline void                 setRoute (const Edge c, const route_t & route)    {
   		ASSERT_WRITABLE();
   		reset_computation();
   		boost::put(boost::edge_route, this->getG(), c.e, route);
       }
    inline const route_t &           getRoute (const Edge c )   const      {return boost::get(get(boost::edge_route, this->getG()), c.e);}

    inline void                 setMapping (const Vertex t,
                                           const node_id_t core_id)    {
		ASSERT_WRITABLE();
		reset_computation();
		boost::put(boost::vertex_mapping, this->getG(), t.v, core_id);
    }
    inline node_id_t           getMapping (const Vertex t )   const      {return boost::get(get(boost::vertex_mapping, this->getG()), t.v);}


    inline void                 setZi (const Vertex t,
                                       const TOKEN_UNIT Zi)    {
                           			ASSERT_NOT_NORMALIZED(); boost::put(boost::vertex_Zi, this->getG(), t.v, Zi);}
  inline   TOKEN_UNIT          getZi (const Vertex t )    const     {ASSERT_NORMALIZED(); return (TOKEN_UNIT) boost::get(get(boost::vertex_Zi, this->getG()), t.v);}

    inline void                 setNi (const Vertex t,
                                       const EXEC_COUNT Ni)    {	ASSERT_NOT_REPETITION_VECTOR(); boost::put(boost::vertex_Ni, this->getG(), t.v, Ni);}
    inline EXEC_COUNT           getNi (const Vertex t )    const     {ASSERT_REPETITION_VECTOR(); return boost::get(get(boost::vertex_Ni, this->getG()), t.v);}


    inline void                 setPhasesQuantity (const Vertex t,
                                                   const EXEC_COUNT phi)    {		ASSERT_WRITABLE();
                                               	reset_computation();
                                               	boost::put(boost::vertex_phasecount, this->getG(), t.v, phi);}
    inline EXEC_COUNT           getPhasesQuantity (const Vertex t )   const      {return boost::get(get(boost::vertex_phasecount, this->getG()), t.v);}
    inline void                 setInitPhasesQuantity (const Vertex t,
                                                   const EXEC_COUNT phi)    {		ASSERT_WRITABLE();
                                               	reset_computation();
                                               	boost::put(boost::vertex_initphasecount, this->getG(), t.v, phi);}
    inline EXEC_COUNT           getInitPhasesQuantity (const Vertex t )   const      {return boost::get(get(boost::vertex_initphasecount, this->getG()), t.v);}
    inline void                 setReentrancyFactor (const Vertex t,
                                                   const EXEC_COUNT r)    {				ASSERT_WRITABLE();
                                       			reset_computation();boost::put(boost::vertex_reentrancy, this->getG(), t.v, r);}
    inline EXEC_COUNT           getReentrancyFactor (const Vertex t )    const     {return boost::get(get(boost::vertex_reentrancy, this->getG()), t.v);}


    inline  TOKEN_FRACT          getAlpha          (const Edge c)      const      {ASSERT_NORMALIZED(); return boost::get(get(boost::edge_alpha, this->getG()), c.e);}
    inline  void                setAlpha          (const Edge c,
                                                   const TOKEN_FRACT a)   {ASSERT_NOT_NORMALIZED(); boost::put(boost::edge_alpha, this->getG(), c.e, a);}
    inline  TOKEN_UNIT          getPreload        (const Edge c)        const    {return boost::get(get(boost::edge_preload, this->getG()), c.e);}
    inline  void                setPreload        (const Edge c,
                                                   const TOKEN_UNIT p)   {		ASSERT_WRITABLE();
                                                   boost::put(boost::edge_preload, this->getG(), c.e, p);}
    inline  DATA_UNIT           getTokenSize      (const Edge c)       const     {return boost::get(get(boost::edge_tokensize, this->getG()), c.e);}
    inline  void                setTokenSize      (const Edge c,
                                                   const DATA_UNIT ts)    {				ASSERT_WRITABLE();
                                       			reset_computation();boost::put(boost::edge_tokensize, this->getG(), c.e, ts);}
    inline  EXEC_COUNT          getEdgeOutPhasesCount   (const Edge c) const   {
    	EXEC_COUNT tmp =  boost::get(get(boost::edge_outputs, this->getG()), c.e).size();
    	VERBOSE_DEBUG_ASSERT(this->getPhasesQuantity(this->getEdgeTarget(c)) == tmp, "Edge (" << this->getEdgeName(c) << ") output spec (" << tmp << ") does not match task spec(" << this->getPhasesQuantity(this->getEdgeTarget(c)) << ")");
    	return tmp;
    }
    inline  EXEC_COUNT          getEdgeInPhasesCount   (const Edge c) const   {
    	EXEC_COUNT tmp =  boost::get(get(boost::edge_inputs, this->getG()), c.e).size();
    	VERBOSE_DEBUG_ASSERT(this->getPhasesQuantity(this->getEdgeSource(c)) == tmp, "Edge input spec  (" << tmp << ") does ot match task spec (" << this->getPhasesQuantity(this->getEdgeSource(c)) << ")");
    	return tmp;
    }

    inline  EXEC_COUNT          getEdgeOutInitPhasesCount   (const Edge c) const   {
    	EXEC_COUNT tmp = boost::get(get(boost::edge_init_outputs, this->getG()), c.e).size();
    	VERBOSE_DEBUG_ASSERT(this->getInitPhasesQuantity(this->getEdgeTarget(c)) == tmp, "Edge output init spec does ot match task init spec");
        return tmp;
    }
    inline  EXEC_COUNT          getEdgeInInitPhasesCount   (const Edge c) const   {
    	EXEC_COUNT tmp =  boost::get(get(boost::edge_init_inputs, this->getG()), c.e).size();
    	VERBOSE_DEBUG_ASSERT(this->getInitPhasesQuantity(this->getEdgeSource(c)) == tmp, "Edge input init spec (" << tmp << ") does ot match task init spec (" << this->getInitPhasesQuantity(this->getEdgeSource(c)) << ")");
        return tmp;
    }

    inline  TOKEN_UNIT          getEdgeOut    (const Edge c)  		       const   {return boost::get(get(boost::edge_total_output, this->getG()), c.e);}

    inline const std::vector<TOKEN_UNIT> &         getEdgeOutVector   (const Edge c) const   {
    	const std::vector<TOKEN_UNIT> &       tmp = boost::get(get(boost::edge_outputs, this->getG()), c.e);
    	VERBOSE_DEBUG_ASSERT(this->getPhasesQuantity(this->getEdgeTarget(c)) == (EXEC_COUNT) tmp.size(), "Edge output spec does ot match task spec");
    	return tmp;
    }

    inline const std::vector<TOKEN_UNIT> &          getEdgeInVector   (const Edge c)  const  {
    	const std::vector<TOKEN_UNIT> &       tmp = boost::get(get(boost::edge_inputs, this->getG()), c.e);
    	VERBOSE_DEBUG_ASSERT(this->getPhasesQuantity(this->getEdgeSource(c)) == (EXEC_COUNT) tmp.size(), "Edge input spec does ot match task spec");
    	return tmp;
    }


    inline const std::vector<TOKEN_UNIT> &         getEdgeInitOutVector   (const Edge c) const   {
    	const std::vector<TOKEN_UNIT> & tmp =  boost::get(get(boost::edge_init_outputs, this->getG()), c.e);
    	VERBOSE_DEBUG_ASSERT(this->getInitPhasesQuantity(this->getEdgeTarget(c)) == (EXEC_COUNT) tmp.size(), "Edge output init spec does ot match task init spec");
        return tmp;
    }

    inline const std::vector<TOKEN_UNIT> &          getEdgeInitInVector   (const Edge c)  const  {

    	const std::vector<TOKEN_UNIT> & tmp = boost::get(get(boost::edge_init_inputs, this->getG()), c.e);
    	VERBOSE_DEBUG_ASSERT(this->getInitPhasesQuantity(this->getEdgeSource(c)) == (EXEC_COUNT) tmp.size(), "Edge input init spec (" <<   tmp.size() << ") does not match task init spec (" << this->getInitPhasesQuantity(this->getEdgeSource(c)) << ")");
        return tmp;

    }



    inline  TOKEN_UNIT          getEdgeOutPhase   (const Edge c, PHASE_INDEX k)  const  {



    	if (k <= 0 ) {
  		  return getEdgeOutInitPhase(c, k + boost::get(get(boost::edge_init_outputs, this->getG()), c.e).size());
    	}

    	if ((PHASE_INDEX)  boost::get(get(boost::edge_outputs, this->getG()), c.e).size() < k) {
            VERBOSE_ERROR("k value ("<< k << ") is too high (output list is about "<< boost::get(get(boost::edge_outputs, this->getG()), c.e).size() <<" values ).");
            VERBOSE_FAILURE();
        }


        return boost::get(get(boost::edge_outputs, this->getG()), c.e).at(k-1);
    }

    inline  void                setEdgeOutPhases  (const Edge c,
                                                   std::vector<TOKEN_UNIT> l)    {
    	ASSERT_WRITABLE();
    	reset_computation();
        VERBOSE_DATAFLOW_DEBUG(" - Add outputs for " << this->getEdgeName(c) << " coded " << c << " = " << l.size() << " states = " <<  commons::join(l.begin(),l.end(),std::string(",")));
    	boost::put(boost::edge_outputs, this->getG(), c.e, l);
    	TOKEN_UNIT total =std::accumulate(l.begin(),l.end(),0);
    	boost::put(boost::edge_total_output, this->getG(), c.e, total);
    	EXEC_COUNT q = this->getPhasesQuantity(this->getEdgeTarget(c));
    	VERBOSE_DATAFLOW_DEBUG("   -   Edge from " << this->getVertexName(this->getEdgeSource(c)) << " to " << this->getVertexName(this->getEdgeTarget(c)));
    	VERBOSE_DATAFLOW_DEBUG("   - EdgeTarget is " << this->getVertexName(this->getEdgeTarget(c)) << " with " << q << " states");
    	if (q > 0) {VERBOSE_ASSERT(q == (EXEC_COUNT) l.size(),"Error, the number of phase for the target task of edge " << c << " is " << q <<", but the number of value given is " << l.size());}
    	this->setPhasesQuantity(this->getEdgeTarget(c),l.size());
    }

    inline  TOKEN_UNIT          getEdgeOutInitPhase   (const Edge c, PHASE_INDEX k)  const  {
        if ((PHASE_INDEX)  boost::get( get(boost::edge_init_outputs, this->getG()), c.e).size() < k) {
            VERBOSE_ERROR("k value ("<< k << ") is too high (init output list is about "<< boost::get(get(boost::edge_init_outputs, this->getG()), c.e).size() <<" values ).");
            VERBOSE_FAILURE();
        }
        return boost::get(get(boost::edge_init_outputs, this->getG()), c.e).at(k-1);
    }

    inline  void                setEdgeOutInitPhases  (const Edge c,
                                                   std::vector<TOKEN_UNIT> l)    {
    	ASSERT_WRITABLE();
    	reset_computation();
        VERBOSE_DATAFLOW_DEBUG(" - Add outputs for " << c << " = " << l.size() << " states = " <<  commons::join(l.begin(),l.end(),std::string(",")));
    	boost::put(boost::edge_init_outputs, this->getG(), c.e, l);
    	EXEC_COUNT q = this->getInitPhasesQuantity(this->getEdgeTarget(c));
    	VERBOSE_DATAFLOW_DEBUG(" -   Edge from " << this->getVertexName(this->getEdgeSource(c)) << " to " << this->getVertexName(this->getEdgeTarget(c)));
    	VERBOSE_DATAFLOW_DEBUG(" -   Edge Target is " << this->getVertexName(this->getEdgeTarget(c)) << " with " << q << " init states");
    	if (q > 0) {VERBOSE_ASSERT(q == (EXEC_COUNT) l.size(),"Error, the number of init phase for the target task of edge " << c << " is " << q <<", but the number of value given is " << l.size());}
    	this->setInitPhasesQuantity(this->getEdgeTarget(c),l.size());
    }


    inline  TOKEN_UNIT          getEdgeIn    (const Edge c)  		       const   {return boost::get(get(boost::edge_total_input, this->getG()), c.e);}
    inline  TOKEN_UNIT          getEdgeInPhase    (const Edge c, PHASE_INDEX k)  const  {


    	if (k <= 0 ) {
  		  return getEdgeInInitPhase(c, k + boost::get(get(boost::edge_init_inputs, this->getG()), c.e).size());
    	}
       	if ((PHASE_INDEX) boost::get(get(boost::edge_inputs, this->getG()), c.e).size() < k) {
        		VERBOSE_ERROR("k value ("<< k << ") is too high (input list is about "<< boost::get(get(boost::edge_inputs, this->getG()), c.e).size() <<" values ).");
        		VERBOSE_FAILURE();
        	}

    	return boost::get(get(boost::edge_inputs, this->getG()), c.e).at(k-1);
    }


       inline  void                setEdgeInPhases   (const Edge c,
                                                      std::vector<TOKEN_UNIT> l)    {
       	ASSERT_WRITABLE();
       	reset_computation();
           VERBOSE_DATAFLOW_DEBUG(" - Add inputs for " << c << " = " << l.size() << " states = " <<  commons::join(l.begin(),l.end(),std::string(",")));
       	boost::put(boost::edge_inputs, this->getG(), c.e, l);
       	TOKEN_UNIT total =std::accumulate(l.begin(),l.end(),0);
       	boost::put(boost::edge_total_input, this->getG(), c.e, total);
       	EXEC_COUNT q = this->getPhasesQuantity(this->getEdgeSource(c));
       	VERBOSE_DATAFLOW_DEBUG(" - EdgeSource is " << this->getVertexName(this->getEdgeSource(c)) << " with " << q << " states");
       	if (q > 0) {VERBOSE_ASSERT(q == (EXEC_COUNT) l.size(),"Error, the number of phase for the source task of edge " << c << " is " << q <<", but the number of value given is " << l.size());}

       	this->setPhasesQuantity(this->getEdgeSource(c),l.size());
       }

       inline  TOKEN_UNIT          getEdgeInInitPhase    (const Edge c, PHASE_INDEX k)  const  {
              if ((PHASE_INDEX) boost::get( get(boost::edge_init_inputs, this->getG()), c.e).size() < k) {
                  VERBOSE_ERROR("k value ("<< k << ") is too high (input list is about "<< boost::get(get(boost::edge_init_inputs, this->getG()), c.e).size() <<" values ).");
                  VERBOSE_FAILURE();
              }
              return boost::get(get(boost::edge_init_inputs, this->getG()), c.e).at(k-1);
          }
          inline  void                setEdgeInInitPhases   (const Edge c,
                                                         std::vector<TOKEN_UNIT> l)    {
          	ASSERT_WRITABLE();
          	reset_computation();
              VERBOSE_DATAFLOW_DEBUG(" - Add inputs for " << c << " = " << l.size() << " states = " <<  commons::join(l.begin(),l.end(),std::string(",")));
          	boost::put(boost::edge_init_inputs, this->getG(), c.e, l);
          	EXEC_COUNT q = this->getInitPhasesQuantity(this->getEdgeSource(c));
          	VERBOSE_DATAFLOW_DEBUG(" - EdgeSource is " << this->getVertexName(this->getEdgeSource(c)) << " with " << q << " init states");
          	if (q > 0) {VERBOSE_ASSERT(q == (EXEC_COUNT) l.size(),"Error, the number of init phase for the source task of edge " << c << " is " << q <<", but the number of value given is " << l.size());}

        	VERBOSE_DEBUG_ASSERT(
        			this->getInitPhasesQuantity(this->getEdgeSource(c))
					== (EXEC_COUNT) l.size(), "Edge input init spec (" <<   l.size() << ") does not match task init spec (" << this->getInitPhasesQuantity(this->getEdgeSource(c)) << ")");

          	this->setInitPhasesQuantity(this->getEdgeSource(c),l.size());
          }


    inline  const std::string   getEdgeInputPortName     (const Edge c)       const       {return boost::get(get(boost::edge_input_port_name, this->getG()), c.e);}
    inline  void                setEdgeInputPortName     (const Edge c,
                                                 const std::string name)    {					ASSERT_WRITABLE();
                                     			reset_computation();boost::put(boost::edge_input_port_name, this->getG(), c.e, name);}


    inline  const std::string   getEdgeOutputPortName     (const Edge c)       const       {return boost::get(get(boost::edge_output_port_name, this->getG()), c.e);}
    inline  void                setEdgeOutputPortName     (const Edge c,
                                                 const std::string name)    {				ASSERT_WRITABLE();
                                     			reset_computation();boost::put(boost::edge_output_port_name, this->getG(), c.e, name);}


    inline Edge               getInputEdgeByPortName         (const Vertex t,
                                                                    const std::string   name) const {
          {ForInputEdges(this,t,pChannel) {
              if (getEdgeOutputPortName(pChannel) == name) return pChannel;
          }}
          return NULL_EDGE; // not found
          }

      inline Edge               getOutputEdgeByPortName         (const Vertex t,
                                                                    const std::string   name)  const{
          {ForOutputEdges(this,t,pChannel) {
              if (getEdgeInputPortName(pChannel) == name) return pChannel;
          }}
          return NULL_EDGE; // not found
          }
      inline  const std::vector<TIME_UNIT>&          getVertexPhaseDuration    (const Vertex t)  const  {
    	  VERBOSE_ASSERT(boost::get(get(boost::vertex_phaseduration, this->getG()), t.v).size() > 0, "I take too much coffee, and this task " << getVertexName(t) <<  " - " << getVertexId(t) <<  " has no duration.");

    	  return boost::get(get(boost::vertex_phaseduration, this->getG()), t.v);
      }

      inline  const std::vector<TIME_UNIT>&          getVertexInitPhaseDuration    (const Vertex t)  const  {
       	  return boost::get(get(boost::vertex_initphaseduration, this->getG()), t.v);
         }


      inline  TIME_UNIT          getVertexTotalDuration    (const Vertex t)  const   {
    	  VERBOSE_ASSERT(boost::get(get(boost::vertex_phaseduration, this->getG()), t.v).size() > 0, "I take too much coffee, and this task " << getVertexName(t) <<  " - " << getVertexId(t) <<  " has no duration.");
    	  auto phasesDurations =  boost::get(get(boost::vertex_phaseduration, this->getG()), t.v);
    	  return std::accumulate(phasesDurations.begin(), phasesDurations.end(), 0);
      }


      inline  TIME_UNIT          getVertexDuration    (const Vertex t) const   {
    	  VERBOSE_ASSERT(boost::get(get(boost::vertex_phaseduration, this->getG()), t.v).size() > 0, "I take too much coffee, and this task " << getVertexName(t) <<  " - " << getVertexId(t) <<  " has no duration.");
    	  VERBOSE_ASSERT(boost::get(get(boost::vertex_phaseduration, this->getG()), t.v).size() == 1, "I take too much coffee, and this task " << getVertexName(t) <<  " - " << getVertexId(t) <<  " has more than one duration, you should use getVertexPhaseDuration.");

    	  return boost::get(get(boost::vertex_phaseduration, this->getG()), t.v).at(0);
      }

      inline  TIME_UNIT          getVertexDuration    (const Vertex t, EXEC_COUNT k)   const {
    	  const std::vector<TIME_UNIT>&  vec = boost::get(get(boost::vertex_phaseduration, this->getG()), t.v);
    	  if (k <= 0 ) {
    		  return getVertexInitDuration(t, k + getInitPhasesQuantity(t));
    	  }
      	return vec.at( (k-1) % vec.size() );
      }

      inline  TIME_UNIT          getVertexInitDuration    (const Vertex t, EXEC_COUNT k)   const {
      	return boost::get(get(boost::vertex_initphaseduration, this->getG()), t.v).at(k-1);
      }

    inline  void                setVertexDuration (const Vertex t,
                                                   std::vector<TIME_UNIT> l)    {
		ASSERT_WRITABLE();
		reset_computation();
		auto pq = getPhasesQuantity(t);
    	VERBOSE_ASSERT((EXEC_COUNT) l.size() == pq, "Task " << getVertexName(t) <<  "(" << getVertexId(t) <<  ") duration vector of size " << l.size() << " does not match task phase count (" << pq << ").")
    	boost::put(boost::vertex_phaseduration, this->getG(), t.v, l);

    }

    inline  void                setVertexInitDuration (const Vertex t,
                                                      std::vector<TIME_UNIT> l)    {
   		ASSERT_WRITABLE();
   		reset_computation();
   		auto ipq = getInitPhasesQuantity(t);
       	VERBOSE_ASSERT((EXEC_COUNT) l.size() == ipq, "Task " << getVertexName(t) <<  "(" << getVertexId(t) <<  ") duration vector of size " << l.size() << " does not match init task phase count (" << ipq << ").")
   		boost::put(boost::vertex_initphaseduration, this->getG(), t.v, l);

       }

	/**
	 * GCDA work
	 */
    std::map<ARRAY_INDEX, TOKEN_UNIT >              channelGCDA;          //!< dams precompute dam
    inline TOKEN_UNIT computeFineGCD(Edge c) const  {

    // compute Channel GCD (multiple commun à toutes les valeurs wa1 wa2 .. va1 va2...)
    TOKEN_UNIT channelGCD = getEdgeInPhase(c,1);


    for (PHASE_INDEX k = 1 ; k <= this->getEdgeInPhasesCount(c) ; k++) {
        channelGCD = std::gcd(channelGCD,getEdgeInPhase(c,k));
    }


    for (PHASE_INDEX k = 1 ; k <= this->getEdgeOutPhasesCount(c) ; k++) {
        channelGCD = std::gcd(channelGCD,getEdgeOutPhase(c,k));
    }
    return channelGCD;

    }
    inline   TOKEN_UNIT getFineGCD(Edge c) const {
    	 std::map<ARRAY_INDEX,TOKEN_UNIT >::const_iterator res = channelGCDA.find(this->getEdgeId(c));
    	 if (res != channelGCDA.end()) return res->second;
    	 return computeFineGCD(c);
    }

    inline   TOKEN_UNIT getFineGCD(Edge c)  {

    	//return computeFineGCD(c);
	       // search in cache
	       std::map<ARRAY_INDEX,TOKEN_UNIT >::iterator res = channelGCDA.find(this->getEdgeId(c));
	       if (res != channelGCDA.end()) return res->second;

	       //Compute GCDA
	       TOKEN_UNIT gcda = computeFineGCD(c);
	       // add in cache
	       channelGCDA.insert(std::pair<ARRAY_INDEX,TOKEN_UNIT> (this->getEdgeId(c), gcda));

	       return gcda;

	   }


/**
 * Normalized access
 */
  inline TOKEN_UNIT      normalizeValue (const Edge c, const TOKEN_UNIT old) const {
            //SPEED UP NORMALIZATION !
            TOKEN_FRACT alpha = getAlpha(c);
            TOKEN_FRACT result_i =  TOKEN_FRACT(old) * alpha;
            if (result_i.denominator() > 1) {
            	VERBOSE_ERROR("FAIL DENOMINATOR " << result_i << "=" << TOKEN_FRACT(old) << "*" << alpha);
            }
            VERBOSE_ASSERT_EQUALS(result_i.denominator(),1);
            return  result_i.numerator();
    }
  inline TOKEN_UNIT      unnormalizeValue (const Edge c, const TOKEN_UNIT v) const {

	  TOKEN_FRACT alpha = getAlpha(c);
	  TIME_UNIT result = commons::division(v * alpha.denominator(),alpha.numerator());
	  if (!commons::isInteger(result)) {
		  VERBOSE_FAILURE();
		  result = std::ceil(result); // TODO need to use gcda
	  }
	  VERBOSE_ASSERT(commons::isInteger(result),TXT_NEVER_HAPPEND);
	  return (TOKEN_UNIT) result;
    }

 inline TOKEN_UNIT      normalizeUnknowValue (const Edge c, const TOKEN_UNIT v)  {
        TOKEN_UNIT gcda = this->getFineGCD(c);
        TOKEN_UNIT old  = commons::floor(v,gcda);

        return normalizeValue(c,old);
    }
 inline TOKEN_UNIT		getNormMop  	(const Edge c)	 {
    TOKEN_UNIT mop= this->getPreload(c);
    return normalizeUnknowValue(c,mop);
}
  inline  TOKEN_UNIT     getNormIn       (const Edge c, PHASE_INDEX phase)  const {
        const TOKEN_UNIT old = this->getEdgeInPhase(c,phase);
        return normalizeValue(c,old);
    }
  inline  TOKEN_UNIT     getNormOut       (const Edge c, PHASE_INDEX phase)  const {
        const TOKEN_UNIT old = this->getEdgeOutPhase(c,phase);
        return normalizeValue(c,old);
    }
};

} // end of namespace models

namespace commons {
template<>
std::string toString<  std::map<Vertex,EXEC_COUNT>  >(const  std::map<Vertex,EXEC_COUNT> & v);
}

#endif /* DATAFLOW_H_ */
