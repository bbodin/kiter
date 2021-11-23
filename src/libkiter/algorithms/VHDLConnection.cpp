/*
 * VHDLComponent.cpp
 *
 *  Created on: 15 June 2021
 *      Author: jkmingwen
 */

#include <models/Dataflow.h>
#include "VHDLConnection.h"

VHDLConnection::VHDLConnection(models::Dataflow* const dataflow, Edge e) {
  edge = e;
  connectionName = dataflow->getEdgeName(e);
  id = dataflow->getEdgeId(e);
  dataType = "float"; // assuming only data type is float for now
  bufferSize = dataflow->getPreload(e) + 10; // TODO replace with buffer size from XML
  initialTokenCount = dataflow->getPreload(e);
  dataTypeWidth = 34; // assume data type is float (using FloPoCo float representation)
  srcPort = dataflow->getEdgeInputPortName(e);
  dstPort = dataflow->getEdgeOutputPortName(e);
}

Edge VHDLConnection::getEdge() {
  return this->edge;
}

std::string VHDLConnection::getName() {
  return this->connectionName;
}

ARRAY_INDEX VHDLConnection::getId() {
  return this->id;
}

std::string VHDLConnection::getSrcPort() {
  return this->srcPort;
}

std::string VHDLConnection::getDstPort() {
  return this->dstPort;
}

std::string VHDLConnection::getType() {
  return this->dataType;
}

TOKEN_UNIT VHDLConnection::getBufferSize() {
  return this->bufferSize;
}

TOKEN_UNIT VHDLConnection::getInitialTokenCount() {
  return this->initialTokenCount;
}

int VHDLConnection::getTypeWidth() {
  return this->dataTypeWidth;
}

void VHDLConnection::setName(std::string newName) {
  this->connectionName = newName;
}

void VHDLConnection::setId(ARRAY_INDEX newId) {
  this->id = newId;
}

void VHDLConnection::setBufferSize(TOKEN_UNIT size) {
  this->bufferSize = size;
}

void VHDLConnection::setInitialTokenCount(TOKEN_UNIT count) {
  assert(count <= this->bufferSize);
  this->initialTokenCount = count;
}

std::string VHDLConnection::printStatus() {
  std::stringstream outputStream;

  outputStream << "\nEdge " << this->getName() << " (ID: " << this->getId()
               << ")" << std::endl;
  outputStream << "\tData type, width: " << this->getType()
               << ", " << this->getTypeWidth() << std::endl;
  outputStream << "\tBuffer size: " << this->getBufferSize() << std::endl;
  outputStream << "\tSource port, Destination port: "
               << this->getSrcPort() << ", " << this->getDstPort() << std::endl;

  return outputStream.str();
}
