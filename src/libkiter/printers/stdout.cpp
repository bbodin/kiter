/*
 * stdout.cpp
 *
 *  Created on: 16 avr. 2013
 *      Author: toky
 */

#include <models/Dataflow.h>
#include <printers/stdout.h>
#include <commons/commons.h>
#include <printers/SDF3Wrapper.h>

std::string add_block ( std::string name , TIME_UNIT start, TIME_UNIT duration, double node_height,  double current_task_y_pos, double line_width =  0.1 , bool fill = false) {
		std::ostringstream returnStream;
		returnStream<< "\"" << name << 0  << "\""
				<< " [label=\"\", width=\"" << duration / 72 << "\", penwidth=\"" << line_width << "\"";
		if (fill) returnStream <<  ", style=\"filled\" , fillcolor=blue" ;
		returnStream <<  ", height=\"" << node_height << "\", fixedsize=true, pos=\"" << start + (duration/2) << ","<< current_task_y_pos << "\",shape=box]" << std::endl;
		return returnStream.str();
}

std::string printers::PeriodicScheduling2DOT    (models::Dataflow* const  dataflow,  models::Scheduling& sched ,   TIME_UNIT last_execution_end_at, bool ,  double xscale , double yscale ) {

  std::ostringstream returnStream;
  scheduling_t scheduling = sched.getTaskSchedule();
  static double label_node_width  =   0.5;
  static double label_node_height = yscale *  0.10;

  static double node_height = yscale * 0.05;
  static double pixel_start =  (-(72/2) - 1);
  static double line_width =  0.1;
  static double fontsize = 4;

  returnStream << "// Auto-generate by Kiter" << std::endl;
  returnStream << "//   use this dot file with -Knop only!\n" << std::endl;
  returnStream << "//   node_height = " << node_height << " \n" << std::endl;
  returnStream << "//   fontsize = " << fontsize << " \n" << std::endl;
  returnStream << "//   fontsize = " << fontsize << " \n" << std::endl;
  returnStream << "digraph {\n" << std::endl;
  returnStream << "graph [sep = \"0\", nodesep = \"0\"];" << std::endl;


  if (last_execution_end_at == 0)
  {ForEachVertex(dataflow,t) {
      auto tid = dataflow->getVertexId(t); 
      //VERBOSE_ASSERT(dataflow->getPhasesQuantity(t) == 1, "Support only SDF");
  	  auto Ni = dataflow->getNi(t);

  	  auto period = scheduling[tid].periodic_starts.first;
  	  auto duration = dataflow->getVertexTotalDuration(t);
  	  auto starts = scheduling[tid].periodic_starts.second;

        for (EXEC_COUNT iter = 0 ; iter < (Ni/(EXEC_COUNT)starts.size()) ; iter++) {
      	  for (auto  start : starts) {
      		last_execution_end_at = std::max(last_execution_end_at, duration + (start + period * iter));
      	  }
        }

      }}

  double idx = 0;
  {ForEachVertex(dataflow,t) {
      auto tid = dataflow->getVertexId(t); 
      //VERBOSE_ASSERT(dataflow->getPhasesQuantity(t) == 1, "Support only SDF");

	  auto period = scheduling[tid].periodic_starts.first;
	  auto starts = scheduling[tid].periodic_starts.second;

	  double current_task_y_pos = yscale * -(fontsize + 1) * idx ;

      returnStream << "\""<< dataflow->getVertexName(t) << "\"" << " [label=\"" <<  dataflow->getVertexName(t) << "\\r\",fontsize=\"" << fontsize << "\", margin=\"0,0\", width=\"" << label_node_width << "\", height=\"" << label_node_height << "\", fixedsize=true, pos=\"" << pixel_start << ","<< current_task_y_pos << "\",shape=plaintext]" << std::endl;

      EXEC_COUNT execution_index = 0;

      bool fill_init  = true;
      bool fill_first = true;
      bool fill_per   = false;

      for (EXEC_COUNT init_phase = 0 ; init_phase < dataflow->getInitPhasesQuantity(t) ; init_phase++) {
    	  TIME_UNIT start = starts[init_phase];
		  auto duration = dataflow->getVertexInitDuration(t,init_phase+1);
		  returnStream << add_block (
				  dataflow->getVertexName(t) + std::to_string(execution_index++),
				  xscale * (start),
				  xscale * (duration),  node_height,   current_task_y_pos,  line_width , fill_init ) ;
      }


      EXEC_COUNT first_start_index = dataflow->getInitPhasesQuantity(t);

      for (EXEC_COUNT iter = 0 ;  ((starts[first_start_index] + period * iter)) < last_execution_end_at ; iter++) {
    	  for (EXEC_COUNT sidx = first_start_index; sidx < (EXEC_COUNT) starts.size() ; sidx++) {
    		  auto current_phase = 1 +  sidx % dataflow->getPhasesQuantity(t);
    		  auto duration = dataflow->getVertexDuration(t,current_phase);

    		  bool fill = iter == 0 ? fill_first : fill_per;
        	  TIME_UNIT start = starts[sidx];
    		  returnStream << add_block ( dataflow->getVertexName(t) + std::to_string(execution_index++),
    		     				  xscale * (start + period * iter),
    		 					  xscale * (duration),  node_height,   current_task_y_pos,  line_width , fill ) ;
    	  }
      }

      idx+=1;

    }}

  returnStream << std::endl;

  returnStream <<  "}" << std::endl;

  return returnStream.str();

}
void  printers::printGraphAsKiterScript (models::Dataflow* const  dataflow, parameters_list_t params) {

	  bool verbose = params.count("verbose");

	  std::cout << generate_kiter(dataflow, verbose);
}

std::string  printers::generate_kiter (const models::Dataflow* const  dataflow, bool verbose) {


	  std::ostringstream returnStream;


	  returnStream << "// Auto-generate by Kiter for Kiter" << std::endl;
	  returnStream << "// " << std::endl;

	  returnStream << "models::Dataflow* generateGraph () {"   << std::endl;



	  if (verbose) returnStream << "std::cout << \"generate a dataflow graph...\" << std::endl;"  << std::endl;
	  returnStream << "models::Dataflow* new_graph = new models::Dataflow();" << std::endl;
	  returnStream << "" << std::endl;

	  {ForEachVertex(dataflow,t) {
		  PHASE_INDEX phase_count = dataflow->getPhasesQuantity(t);
		  PHASE_INDEX init_phase_count = dataflow->getInitPhasesQuantity(t);
		  returnStream << "{" << std::endl;
		  if (verbose) returnStream << "std::cout << \"generate a task ...\" << std::endl;"  << std::endl;
		  returnStream << "auto new_vertex = new_graph->addVertex(" << dataflow->getVertexId(t) << ");" << std::endl;

		  returnStream << " new_graph->setVertexName(new_vertex,\"" << dataflow->getVertexName(t) << "\");" << std::endl;
		  returnStream << " new_graph->setInitPhasesQuantity(new_vertex," <<init_phase_count << ");" << std::endl;
		  returnStream << " new_graph->setPhasesQuantity(new_vertex," <<phase_count << ");" << std::endl;
		  returnStream << " new_graph->setReentrancyFactor(new_vertex," << dataflow->getReentrancyFactor(t)<< "); " << std::endl;
		  if (init_phase_count) {
		  std::string init_duration_string = "";
		  for (PHASE_INDEX phase = 1 ; phase <= init_phase_count ; phase++) {
			  if (phase > 1)  init_duration_string += ",";
			  init_duration_string += commons::toString<TIME_UNIT>(dataflow->getVertexInitDuration(t,phase));
		  }
		  init_duration_string = "{" + init_duration_string + "}";
		  returnStream << " new_graph->setVertexInitDuration(new_vertex," << init_duration_string << ");" << std::endl;
		  }
		  std::string duration_string = "";
		  for (PHASE_INDEX phase = 1 ; phase <= phase_count ; phase++) {
			  if (phase > 1)  duration_string += ",";
			  duration_string += commons::toString<TIME_UNIT>(dataflow->getVertexDuration(t,phase));
		  }
		  duration_string = "{" + duration_string + "}";

		  returnStream << " new_graph->setVertexDuration(new_vertex," << duration_string << ");" << std::endl;

		  returnStream << "}" << std::endl;

	   }}



	  {ForEachChannel(dataflow,c){

	      Vertex in_vertex  = dataflow->getEdgeSource(c);
	      Vertex out_vertex = dataflow->getEdgeTarget(c);

	      ARRAY_INDEX in_vertex_id  = dataflow->getVertexId(in_vertex);
	      ARRAY_INDEX out_vertex_id = dataflow->getVertexId(out_vertex);



		  EXEC_COUNT in_phase_count = dataflow->getPhasesQuantity(in_vertex);
		  EXEC_COUNT out_phase_count = dataflow->getPhasesQuantity(out_vertex);



		  returnStream << "{" << std::endl;
		  if (verbose) returnStream << "std::cout << \"generate a buffer ...\" << std::endl;"  << std::endl;

	      returnStream << "auto new_edge = new_graph->addEdge(new_graph->getVertexById(" << in_vertex_id << "), new_graph->getVertexById(" << out_vertex_id << "));" << std::endl;


	      std::string in_rates = "";
	      for (PHASE_INDEX phase = 1 ; phase <= in_phase_count ; phase++) {
	      			  if (phase > 1)  in_rates += ",";
	      			in_rates += commons::toString<TOKEN_UNIT>(dataflow->getEdgeInPhase(c,phase));
	      }
	      in_rates = "{" + in_rates + "}";



	      std::string out_rates = "";
	      for (PHASE_INDEX phase = 1 ; phase <= out_phase_count ; phase++) {
	      			  if (phase > 1)  out_rates += ",";
	      			out_rates += commons::toString<TOKEN_UNIT>(dataflow->getEdgeOutPhase(c,phase));
	      }
	      out_rates = "{" + out_rates + "}";


		  EXEC_COUNT init_in_phase_count = dataflow->getInitPhasesQuantity(in_vertex);
		  EXEC_COUNT init_out_phase_count = dataflow->getInitPhasesQuantity(out_vertex);

		  if (init_in_phase_count) {
	      std::string init_in_rates = "";
	      for (PHASE_INDEX init_phase = 1 ; init_phase <= init_in_phase_count ; init_phase++) {
	      			  if (init_phase > 1)  init_in_rates += ",";
	      			init_in_rates += commons::toString<TOKEN_UNIT>(dataflow->getEdgeInInitPhase(c,init_phase));
	      }
	      init_in_rates = "{" + init_in_rates + "}";
		  returnStream << " new_graph->setEdgeInInitPhases(new_edge," << init_in_rates << ");" << std::endl;
		  }


		  if (init_out_phase_count) {
	      std::string init_out_rates = "";
	      for (PHASE_INDEX init_phase = 1 ; init_phase <= init_out_phase_count ; init_phase++) {
	      			  if (init_phase > 1)  init_out_rates += ",";
	      			init_out_rates += commons::toString<TOKEN_UNIT>(dataflow->getEdgeOutInitPhase(c,init_phase));
	      }
	      init_out_rates = "{" + init_out_rates + "}";
		  returnStream << " new_graph->setEdgeOutInitPhases(new_edge," << init_out_rates << ");" << std::endl;
		  }



		  returnStream << " new_graph->setEdgeInPhases(new_edge," << in_rates << ");" << std::endl;
		  returnStream << " new_graph->setEdgeOutPhases(new_edge," << out_rates << ");" << std::endl;
		  returnStream << " new_graph->setPreload(new_edge," << dataflow->getPreload(c) << ");" << std::endl;
		  returnStream << " new_graph->setEdgeName(new_edge,\"" << dataflow->getEdgeName(c) << "\");" << std::endl;

		  returnStream << "}" << std::endl;

	    }}
	  returnStream << std::endl;
	  returnStream << "}"   << std::endl;
	  return returnStream.str();

}

struct TaskSprite {
	  ARRAY_INDEX id = 0;
	  double x = 0,y = 0;
	  bool small  = false;
	  TaskSprite(ARRAY_INDEX id, double x, double y, bool small) : id (id) ,  x (x) ,  y (y) ,  small (small) {}
};

std::string printers::GenerateNoCDOT    (models::Dataflow* const  dataflow , bool connect_tasks, bool color_routes) {

  VERBOSE_DEBUG("Start NoC DOT generation");
  const NoC & noc = dataflow->getNoC();


  std::ostringstream returnStream;


  // ******* HEADER

  returnStream << "// Auto-generate by Kiter" << std::endl;
  returnStream << "//   use this dot file with neato for an optimal visu\n" << std::endl;
  returnStream << "digraph G {\n" << std::endl;

  //returnStream <<  "  graph [label=\"" << "Auto-generate by the Kiter"<< "\",overlap=scale,splines=true]\n";
  //returnStream << "   edge [labelangle=15,labeldistance=1,len=1.5,fontsize=8,labelsize=4,color=grey]" << std::endl;

  returnStream << std::endl;


  ARRAY_INDEX routeur_count = noc.getNodes().size();

  // ******* NoC Devices

  for (const NetworkNode node : noc.getNodes()) {
	  double x = noc.getXSize() - node.x;
	  double y = noc.getYSize() - node.y;
	  if (node.type == NetworkNodeType::Core) {
		  std::string label = commons::toString(node.id - (routeur_count/2));
		  returnStream << node.id << "[label=\"" << label << "\\l\", labelloc=\"t\", margin=0.03, fontsize=\"10\", shape=\"box\",  width=0.5, height=0.5, pos=\"" << x << "," << y << "!\", pin=TRUE, notranslate=TRUE];" << std::endl;
	  } else if (node.type == NetworkNodeType::Router) {
		  std::string label = commons::toString(node.id);
		  returnStream << node.id << "[label=\"" << label << "\", margin=0.03, fontsize=\"5\", shape=\"box\",  width=0.2, height=0.2, pos=\"" << x << "," << y << "!\", pin=TRUE, notranslate=TRUE];" << std::endl;
	  }
  }



  std::map<edge_id_t,std::vector<Edge>> link_usage;



  // ******* Build link_usage


  for (const NetworkEdge e : noc.getEdges()) {
	  link_usage[e.id];
      //returnStream << e.src << "->" << e.dst << std::endl;
  }



  {ForEachChannel(dataflow,c){
	  Vertex source_vtx   = dataflow->getEdgeSource(c);
	  Vertex target_vtx   = dataflow->getEdgeTarget(c);
	  const std::vector<edge_id_t>& route = dataflow->getRoute(c);
	  VERBOSE_DEBUG("edge_id=" << dataflow->getEdgeId(c) << " from tasks " <<  dataflow->getVertexId(source_vtx) << " and " <<   dataflow->getVertexId(target_vtx)  << " route = " <<  commons::toString(route));

	  for (edge_id_t e : route) {
		  const NetworkEdge& nedge = dataflow->getNoC().getEdge(e);
		  VERBOSE_DEBUG("  - edge_id_t=" << e << " connects " <<  nedge.src << "->" << nedge.dst);
	      //returnStream << nedge.src << "->" << nedge.dst<< "[style = \"bold\", color=\"red\", splines=\"false\" ];" << std::endl;
		  link_usage[nedge.id].push_back(c);
	  }
  }}


  std::vector<Edge>::size_type max_cnt = 0;
  for (const NetworkEdge e : noc.getEdges()) {
	  max_cnt = std::max(max_cnt , link_usage[e.id].size());
  }


  // ******* Print link_usage
  std::map<Edge,std::string> edge_color;
  std::vector<std::string> available_colors = {"#ACFF54","#41E869","#53FFEE","#4191E8","#4191E8","#634BFA",
		                                       "#DB42FF","#E83154","#FF7843","#E8A131","#FADC39", "#8736FF"
};
  std::string task_color = "#ff8065";
  std::string small_task_color = "#e2ffaf";

  for (const NetworkEdge e : noc.getEdges()) {
	  auto cnt = link_usage[e.id].size();
	  if (cnt == 0 or !color_routes) {
		  returnStream << e.src << "->" << e.dst << "[arrowsize=0.5]"<< std::endl;
	  } else {
		  std::string color = "";
		  for (Edge c : link_usage[e.id]) {
			  if (color != "") {color += "::";}

			  if (edge_color.find(c) == edge_color.end()) {
				  edge_color[c] = available_colors[edge_color.size() % available_colors.size()];
			  }
			  color += edge_color[c];
		  }
	      returnStream << e.src << " -> " << e.dst<< "[style = \"bold\", color=\"" << color << "\", splines=\"false\"  , penwidth=2 , arrowsize=0.5 ];" << std::endl;
	  }
  }




  // ******* Generate taskSprites



  std::vector<TaskSprite> taskSprites;

  for (const Vertex v : dataflow->vertices()) {
	  ARRAY_INDEX task_id = dataflow->getVertexId(v);
	  node_id_t node_id = dataflow->getMapping(v);
	  VERBOSE_DEBUG("task_id=" << task_id << " core_id = " <<  node_id);
	  if (node_id >= 0 and dataflow->getNoC().hasNode(node_id) ) {
		  const NetworkNode& core = dataflow->getNoC().getNode(node_id);
		  //double x = core.x;
		  //double y = core.y;

		  double x = noc.getXSize() - core.x;
		  double y = noc.getYSize() - core.y;

		  taskSprites.push_back(TaskSprite(task_id,x,y,(core.type != NetworkNodeType::Core)));
	  } else {
		  double x = 0;
		  double y = 0;
		  {ForInputEdges(dataflow,v,inc) {

			  Vertex vtx   = dataflow->getEdgeSource(inc);
			  node_id_t in_core_id = dataflow->getMapping(vtx);
			  if (in_core_id >= 0) {
				  const NetworkNode& core = dataflow->getNoC().getNode(in_core_id);
				  x +=  noc.getXSize() -core.x;
				  y +=  noc.getYSize() -core.y;
			  }

		  }}

		  {ForOutputEdges(dataflow,v,outc) {

			  Vertex outvtx   = dataflow->getEdgeTarget(outc);
			  node_id_t out_core_id = dataflow->getMapping(outvtx);
			  if (out_core_id >= 0) {
				  const NetworkNode& outcore = dataflow->getNoC().getNode(out_core_id);
				  x +=  noc.getXSize() -outcore.x;
				  y +=  noc.getYSize() -outcore.y;
			  }

		  }}

		  x /= dataflow->getVertexDegree(v);
		  y /= dataflow->getVertexDegree(v);
		  taskSprites.push_back(TaskSprite(task_id,x,y,true));

		  //returnStream <<  "task_" << task_id << "[label=\"Task\\n" << task_id << "\", style=\"filled\", color=\"red\", fontsize=\"2\", shape=\"circle\", pos=\"" << x << "," << y << "!\", fixedsize=\"shape\", width=0.1, height=0.1];" << std::endl;

		 // returnStream <<  "task_" << task_id << "[label=\"Task\\n" << task_id << "\", style=\"filled\", color=\"red\", fontsize=\"2\", shape=\"circle\", fixedsize=\"shape\", width=0.1, height=0.1];" << std::endl;

	  }
  }

  // ******* spread overlapping sprites
  // ** Naive algorithm, we don't need a jumbo cargo to carry two cheeses.

  std::map<std::pair<int,int>, std::vector<TaskSprite> > closed_sprites;

  for (const TaskSprite ts : taskSprites) {

	  long x = (long) std::round(ts.x * 100);
	  long y = (long) std::round(ts.y * 100);

	  closed_sprites[{x,y}].push_back(ts);

  }

  std::vector<TaskSprite> updatedSprites;

  for (auto item : closed_sprites) {
	  if (item.second.size() > 1) {
		  VERBOSE_DEBUG("We need to spread " << item.second.size() << " tasks");
	  }

	  if (item.second.size() == 1) {
		  VERBOSE_DEBUG("I edit this one to zero");
		  updatedSprites.push_back(item.second[0]);
	  } else if (item.second.size() == 2) {
		  item.second[0].x += 0.05;
		  item.second[0].y -= 0.05;
		  item.second[1].x -= 0.05;
		  item.second[1].y += 0.05;
		  updatedSprites.push_back(item.second[0]);
		  updatedSprites.push_back(item.second[1]);
	  } else if (item.second.size() == 3) {
		  item.second[0].x += 0.00;
		  item.second[0].y -= 0.08;
		  item.second[1].x -= 0.08;
		  item.second[1].y += 0.02;
		  item.second[2].x += 0.045;
		  item.second[2].y += 0.045;
		  updatedSprites.push_back(item.second[0]);
		  updatedSprites.push_back(item.second[1]);
		  updatedSprites.push_back(item.second[2]);
	  } else if (item.second.size() == 4) {
		  item.second[0].x += 0.06;
		  item.second[0].y -= 0.06;
		  item.second[1].x -= 0.06;
		  item.second[1].y += 0.06;
		  item.second[2].x += 0.06;
		  item.second[2].y += 0.06;
		  item.second[3].x -= 0.06;
		  item.second[3].y -= 0.06;
		  updatedSprites.push_back(item.second[0]);
		  updatedSprites.push_back(item.second[1]);
		  updatedSprites.push_back(item.second[2]);
		  updatedSprites.push_back(item.second[3]);
	  } else if (item.second.size() > 4) {
		  for (size_t idx = 0 ; idx < item.second.size(); idx++) {
			  auto s = item.second[idx];

			  s.x -= 0.06;
			  s.y -= 0.06;

			  s.x += 0.12 * ((double)idx / (double)item.second.size());
			  s.y += 0.12 * ((double)idx / (double)item.second.size());

			  updatedSprites.push_back(s);
		  }
	  }
  }




  // ******* output taskSprites
  {
	  for (auto ts : updatedSprites) {

	   if (ts.small) {
		   returnStream <<  "task_" << ts.id << "[penwidth=0.2,color=black,label=\"\", style=\"filled\", fillcolor=\"" << small_task_color << "\", fontsize=\"5\", shape=\"circle\", pos=\"" << ts.x << "," << ts.y << "!\", fixedsize=\"shape\", width=0.1, height=0.1];" << std::endl;
	   } else {
		   Vertex v = dataflow->getVertexById(ts.id);
		   std::string label = dataflow->getVertexName(v) ;
		   returnStream <<  "task_" << ts.id << "[penwidth=0.2,color=black,label=\"" << label << "\", style=\"filled\", fillcolor=\"" << task_color << "\", fontsize=\"9\", shape=\"circle\", pos=\"" << ts.x << "," << ts.y << "!\", fixedsize=\"shape\", width=0.4, height=0.4];" << std::endl;
	   }
	 }
   }


  // ******* connect tasks

  {ForEachChannel(dataflow,c){

	  if (connect_tasks) {

		  Vertex source_vtx   = dataflow->getEdgeSource(c);
		  Vertex target_vtx   = dataflow->getEdgeTarget(c);

		  ARRAY_INDEX source_task_id = dataflow->getVertexId(source_vtx);
		  ARRAY_INDEX target_task_id = dataflow->getVertexId(target_vtx);

		  returnStream <<   "task_" << source_task_id << " -> "  <<   "task_" << target_task_id << "[ color=\"red\", splines=\"false\"  , penwidth=0.8 , arrowsize=0.3 ];" << std::endl;
	  }
  }}


  returnStream <<  "}" << std::endl;

  returnStream << std::endl;

  return returnStream.str();

}




std::string printers::GenerateGraphDOT    (models::Dataflow* const  dataflow , bool simple) {

  bool consistent = dataflow->is_consistent();

  std::ostringstream returnStream;
  
  VERBOSE_DEBUG("Start Graph DOT generation");

  
  returnStream << "// Auto-generate by Kiter" << std::endl;
  returnStream << "//   use this dot file with circo for an optimal visu\n" << std::endl;
  returnStream << "digraph G {\n" << std::endl;
  returnStream <<  "    rankdir=\"LR\";" << std::endl;
  returnStream <<  "  graph [label=\"" << ""
	       << "\",splines=false]\n";
  returnStream << "  edge [len=1.5,fontsize=8,labelsize=4,color=grey]" << std::endl;

  returnStream << std::endl;


  {ForEachVertex(dataflow,t) {

      ARRAY_INDEX tid =  dataflow->getVertexId(t);

      returnStream << "  t_" << tid << " ["  << "shape=circle,  fixedsize=\"shape\", fontsize=\"7\"," << "label = \"" << dataflow->getVertexName(t);
      if (!simple) {
    	  returnStream	  << "\nid:" << tid
    		  << "\nPhases:" << commons::toString(dataflow->getPhasesQuantity(t))
    		  << "\nNi:" << (consistent ? commons::toString(dataflow->getNi(t)) : "N/A")
    		  << "\nduration:" << commons::toString(dataflow->getVertexPhaseDuration(t))
              << "\nreentrancy:" << commons::toString(dataflow->getReentrancyFactor(t))  ;
      }
      returnStream  << "\" ];" << std::endl;
    }}


  
  {ForEachChannel(dataflow,c){
      Vertex edgeIn  = (dataflow->getEdgeSource(c));
      Vertex edgeOut = (dataflow->getEdgeTarget(c));
      ARRAY_INDEX edgeInId  = dataflow->getVertexId(dataflow->getEdgeSource(c));
      ARRAY_INDEX edgeOutId = dataflow->getVertexId(dataflow->getEdgeTarget(c));
      returnStream << "  t_" << edgeInId << " -> t_" << edgeOutId << " [";
      std::string bl = dataflow->getEdgeTypeStr(c) ;
      ARRAY_INDEX eid = dataflow->getEdgeId(c) ;
      returnStream << " label=\"" ;
      if (!simple) {
    	  returnStream	  << bl
    		  	  << "\nid:" << eid
				  << "\npreload:"  << commons::toString(dataflow->getPreload(c)) ;
      } else {

    	  returnStream	  << commons::toString(dataflow->getPreload(c)) ;
      }
      returnStream << "\"," ;
      returnStream << " headlabel=\"" ;
      if (dataflow->getInitPhasesQuantity(edgeOut) > 0) {returnStream << "(" << commons::toString(dataflow->getEdgeInitOutVector(c)) << ")"  << ";" ;}
      returnStream <<  commons::toString(dataflow->getEdgeOutVector(c)) << "\"," ;
      returnStream << " taillabel=\"" ;
      if (dataflow->getInitPhasesQuantity(edgeIn) > 0) {returnStream << "("  <<  commons::toString(dataflow->getEdgeInitInVector(c))  << ")" << ";" ;}
      returnStream <<  commons::toString(dataflow->getEdgeInVector(c)) << "\"," ;
      returnStream << " ] ;" << std::endl;
    }}
  returnStream << std::endl;
  
  returnStream <<  "}" << std::endl;
  
  return returnStream.str();

}


void printers::printMapping    (models::Dataflow* const  dataflow, parameters_list_t params ) {

	bool color_routes  = false;
	bool connect_tasks = false;
	if (params.find("color_routes") != params.end() ) {
		color_routes = (params["color_routes"] != "0");
	}
	if (params.find("connect_tasks") != params.end() ) {
		connect_tasks = (params["connect_tasks"] != "0");
	}

	std::string data = printers::GenerateNoCDOT    ( dataflow , connect_tasks, color_routes) ;


	static const std::string filename_argument = "filename";
	if (params.find(filename_argument) != params.end() ) {
		std::string filename = params[filename_argument];
		std::ofstream mfile;
		mfile.open (filename);
		mfile << data << std::endl;
		mfile.close();
	} else {
		 std::cout << data << std::endl;
	}


}

void printers::printGraph    (models::Dataflow* const  dataflow, parameters_list_t params ) {

	static const std::string filename_argument = "filename";
	std::string data = printers::GenerateGraphDOT    ( dataflow ) ;

	if (params.find(filename_argument) != params.end() ) {
		std::string filename = params[filename_argument];
		std::ofstream mfile;
		mfile.open (filename);
		mfile << data << std::endl;
		mfile.close();
	} else {
		 std::cout << data << std::endl;
	}


}
void printers::printXML    (models::Dataflow* const  dataflow, parameters_list_t params ) {

	static const std::string filename_argument = "filename";

	if (params.find(filename_argument) != params.end() ) {
		std::string filename = params[filename_argument];
		printers::writeSDF3File(filename, dataflow );
	} else {
		 std::cout << printers::generateSDF3XML(dataflow) << std::endl;
	}

}

static EXEC_COUNT sum_nt(models::Dataflow* const  dataflow) {
	bool consistent = dataflow->is_consistent();
	if (!consistent) { return 0; }

	EXEC_COUNT total = 0;
	{ForEachTask(dataflow,t) {
		total += dataflow->getNi(t) ;
	}}

	return total;
}
static EXEC_COUNT sum_phit(models::Dataflow* const  dataflow) {
	bool consistent = dataflow->is_consistent();
	if (!consistent) { return 0; }

	EXEC_COUNT total = 0;
	{ForEachTask(dataflow,t) {
		total += dataflow->getPhasesQuantity(t) ;
	}}

	return total;
}
static EXEC_COUNT sum_qt(models::Dataflow* const  dataflow) {
	bool consistent = dataflow->is_consistent();
	if (!consistent) { return 0; }

	EXEC_COUNT total = 0;
	{ForEachTask(dataflow,t) {
		total += dataflow->getNi(t) / dataflow->getPhasesQuantity(t);
	}}

	return total;
}

 EXEC_COUNT computeNCombinations(models::Dataflow* const  dataflow) {
	bool consistent = dataflow->is_consistent();
	if (!consistent) { return 0; }

	EXEC_COUNT total = 1;
	{ForEachTask(dataflow,t) {
		total *= dataflow->getNi(t) ;
	}}

	return total;
}


 EXEC_COUNT divisorCount(EXEC_COUNT n) {
	EXEC_COUNT total = 0;

	for (EXEC_COUNT i = 1 ; i <= n ; i++) {
		if (n % i == 0) {total ++; };
	}

	return total;
}

 EXEC_COUNT computeKCombinations(models::Dataflow* const  dataflow) {
	bool consistent = dataflow->is_consistent();
	if (!consistent) { return 0; }

	EXEC_COUNT total = 1;
	{ForEachTask(dataflow,t) {
		total *= divisorCount ( dataflow->getNi(t) ) ;
	}}

	return total;
}

void printers::printInfos    (models::Dataflow* const  dataflow, parameters_list_t ) {

	bool consistent = dataflow->is_consistent();


	std::cout << "App name       = " <<  dataflow->getAppName() << std::endl;
	std::cout << "Graph name     = " <<  dataflow->getGraphName() << std::endl;
	std::cout << "Graph type     = " <<  dataflow->getGraphType() << std::endl;
	std::cout << "Task count     = " <<  dataflow->getVerticesCount() << std::endl;
	std::cout << "Channel count = " <<  dataflow->getEdgesCount() << std::endl;

	if (consistent) {
		std::cout << "Sum(qt) = " <<  sum_qt(dataflow) << std::endl;
		std::cout << "Sum(phit) = " <<  sum_phit(dataflow) << std::endl;
		std::cout << "Sum(Nt=qt*phit) (complexity) = " <<  sum_nt(dataflow) << std::endl;
	} else {
		std::cout << "This dataflow graph is not consistent!" << std::endl;
	}


	std::cout << "                 " << std::endl;

	size_t max_str = 0 ;
	{ForEachTask(dataflow,t) {
		max_str = std::max(max_str , dataflow->getVertexName(t).length());
	}}

	{ForEachChannel(dataflow,c) {
		max_str = std::max(max_str , dataflow->getEdgeName(c).length());
	}}

	int name_len = (int) max_str + 4;

	if (dataflow->getVerticesCount()) {
		std::cout << "TASKS" << std::endl;
		std::cout << std::setw(name_len) << "VertexName"
				<< " ("
				<< std::setw(8) << "VertexId"
				<< ")"
				<< std::setw(9) << "InEdges"
				<< std::setw(9) << "OutEdges"
				<< std::setw(16) << "TotalDuration"
				<< std::setw(20)<< "RepetitionFactor"
				<< std::setw(16)<< "PhaseQuantity"
				<< std::setw(15)<< "Mapping"
				<< std::endl;

		{ForEachTask(dataflow,t) {
			std::cout << std::setw(name_len) << dataflow->getVertexName(t)
                      << " ("
                      << std::setw(8) << dataflow->getVertexId(t)
                      << ")"
					  << std::setw(9) << dataflow->getVertexInDegree(t)
					  << std::setw(9) << dataflow->getVertexOutDegree(t)
                      << std::setw(16) << dataflow->getVertexTotalDuration(t)
                      << std::setw(20)<< (consistent ? commons::toString(dataflow->getNi(t)) : "N/A")
                      << std::setw(16)<< dataflow->getPhasesQuantity(t)
                      << std::setw(15)<< dataflow->getMapping(t)
                      << std::endl;


		}}

	} else {
		std::cout << "NO TASKS" << std::endl;
	}

	std::cout << "                 " << std::endl;

	if (dataflow->getEdgesCount()) {
		std::cout << "CHANNELS" << std::endl;

		std::cout <<  std::setw(name_len) << "ChannelName"
				<< " ("
				<< std::setw(10) << "ChannelId"
				<< ")"
				<< std::setw(10) << "Source"
				<< std::setw(10) << "Target"
				<< std::setw(10) << "TotalIn"
				<< std::setw(10) << "TotalOut"
				<< std::setw(15) << "InitialMarking"
				<< std::setw(20) << "Route"
				<< std::endl;

		{ForEachChannel(dataflow,c) {
			std::cout << std::setw(name_len) << dataflow->getEdgeName(c)
                      << " ("
					  << std::setw(10) << dataflow->getEdgeId(c)
					  << ")"
					  << std::setw(10) << dataflow->getVertexId(dataflow->getEdgeSource(c))
					  << std::setw(10) <<dataflow->getVertexId(dataflow->getEdgeTarget(c))
					  << std::setw(10) << dataflow->getEdgeIn(c)
					  << std::setw(10) << dataflow->getEdgeOut(c)
					  << std::setw(15) << dataflow->getPreload(c)
					  << std::setw(20) << commons::toString(dataflow->getRoute(c))
					  << std::endl;


		}}
	} else {

		std::cout << "NO CHANNELS" << std::endl;
	}
	dataflow->reset_computation();
}

std::string printers::Scheduling2Tikz    (const models::Scheduling& scheduling) {


	  std::ostringstream returnStream;

	  auto task_count     = scheduling.getDataflow()->getVerticesCount();
	  auto scale          = 3.0;
	  auto execution_time = 30;
	  auto ticks          = 2.0;

	  std::vector<std::string> task_colors = {"red", "blue", "green"};

	  returnStream << "\\begin{scheduling}{" << task_count << "}{" << execution_time << "}{" << scale << "}{" << ticks << "}" << std::endl;


	  auto task_index = 0;
	  for (auto item : scheduling.getTaskSchedule()) {
		  task_index++;
		  std::string init_execution_format = "bottom color=" +  task_colors[(task_index - 1) % task_colors.size()] +  "!60,  top color= white";
		  std::string periodic_fexecution_format = "bottom color="+  task_colors[(task_index - 1) % task_colors.size()] +  "!40,  top color= white";
		  std::string periodic_sexecution_format = "bottom color="+  task_colors[(task_index - 1) % task_colors.size()] +  "!20,  top color= white";
		  auto taskId = item.first;
		  auto df = scheduling.getDataflow();
		  auto t = df->getVertexById(taskId);
		  std::string task_name = df->getVertexName(t);
		  auto phi = df->getPhasesQuantity(t);
		  returnStream << "% Task " << taskId << std::endl;

		  auto istarts = item.second.initial_starts;
		  auto pstarts = item.second.periodic_starts;

		  auto exec_index = 0;

		  for (auto cstart : istarts) {
			  exec_index++;
			  auto cphi = (exec_index - 1) % phi + 1;
			  auto cdur = df->getVertexDuration(t, cphi);
			  returnStream << "% Init Start " << cstart << std::endl;
			  returnStream << "\\addexecution[" << init_execution_format  << "]{" <<  task_index << "}{$" << task_name << "_{" << cphi << "}$}{" << cdur << "}{" << cstart << "}" << std::endl;

		  }
		  auto period = pstarts.first;
		  for (auto cstart : pstarts.second) {
			  exec_index++;
			  auto cphi = (exec_index - 1) % phi + 1;
			  auto cdur = df->getVertexDuration(t, cphi);
			  returnStream << "% Periodic Start " << cstart  << " with period " << period << std::endl;
			  returnStream << "\\addperiodictask[" << periodic_sexecution_format  << "]{" <<  task_index << "}{$" << task_name << "_{" << cphi << "}$}{" << cdur << "}{" << cstart << "}{" << period << "}{" << 0 << "}" << std::endl;
			  returnStream << "\\addexecution[" << periodic_fexecution_format  << "]{" <<  task_index << "}{$" << task_name << "_{" << cphi << "}$}{" << cdur << "}{" << cstart << "}" << std::endl;


		  }
	  }




	  returnStream << "\\end{scheduling}" << std::endl;
	  return returnStream.str();

}

