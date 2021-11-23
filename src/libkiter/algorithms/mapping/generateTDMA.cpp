/*
 * generateTDMA.cpp
 *
 *  Created on: Jul 2021
 *      Author: katwinkl3
 */

#include <models/Dataflow.h>
#include <algorithms/mappings.h>
#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

void algorithms::mapping::generateTDMA (models::Dataflow* const  dataflow, parameters_list_t params) {

    xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	xmlNodePtr root = xmlNewNode(NULL,BAD_CAST"tdma");
    xmlNewProp(root,BAD_CAST"slots",BAD_CAST"1");
    xmlDocSetRootElement(doc,root);

    {ForEachEdge(dataflow,e){
        long* r;
        (*r) = dataflow->getRoute(e)[0];
        const unsigned char* rid = (unsigned char*)r;
        xmlNodePtr rule = xmlNewNode(NULL,BAD_CAST"rule");
        xmlNewProp(rule,BAD_CAST"id",rid);
        xmlNewProp(rule,BAD_CAST"slot",BAD_CAST"0");
    }}

    xmlSaveFile("../../../../benchmarks/tdmaeg.xml",doc);
	xmlFreeDoc(doc);

}

