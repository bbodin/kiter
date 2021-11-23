/*
 * VHDLGenerator.h
 *
 *  Created on: 14 June 2021
 *      Author: jkmingwen
 */

#include <set>
#include <string>
#include <boost/filesystem.hpp>
#include "VHDLGenerator.h"
#include "VHDLComponent.h"
#include "VHDLConnection.h"
#include "VHDLCircuit.h"

// for signal name retrieval
#define VALID 0
#define READY 1
#define DATA 2

void algorithms::generateVHDL(models::Dataflow* const dataflow,
                              parameters_list_t param_list) {
  bool outputDirSpecified = false;
  bool bufferless = false;
  std::string dirName = "./" + dataflow->getGraphName() + "_vhdl_gen/"; // default output directory
  std::string componentDir = dirName + "/components/";
  std::string referenceDir = "./kiter/vhdl_generation/";
  std::ofstream outputFile;
  VHDLCircuit circuit;
  circuit.setName(dataflow->getGraphName());

  // check for specified directory
  if (param_list.find("OUTPUT_DIR") != param_list.end()) {
    outputDirSpecified = true;
    dirName = param_list["OUTPUT_DIR"] + "/";
    std::cout << "Generating VHDL code in " << dirName << std::endl;
    componentDir = dirName + "/components/";
  } else {
    std::cout << "Use '-p OUTPUT_DIR=dirName' to set output directory"
              << std::endl;
  }
  if (!boost::filesystem::is_directory(dirName)) {
    boost::filesystem::create_directory(dirName);
  }
  boost::filesystem::create_directory(componentDir);

  // check if FIFO buffers should be generated
  if (param_list.find("BUFFERLESS") != param_list.end()) {
    bufferless = true;
  } else {
    std::cout << "Use '-p BUFFERLESS=t' to generate VHDL without FIFO buffers"
              << std::endl;
  }

  // define location of VHDL generation reference files
  if (param_list.find("REFERENCE_DIR") != param_list.end()) {
    referenceDir = param_list["REFERENCE_DIR"] + "/";
    std::cout << "Looking in " << referenceDir
              << " for VHDL generation reference files" << std::endl;
  } else {
    std::cout << "Looking in " << referenceDir
              << " for VHDL generation reference files by default" << std::endl;
    std::cout << "\tUse -p REFERENCE_DIR=/path/to/reference_directory/ to set reference directory"
              << std::endl;
  }

  // populate circuit object with components and connections based on dataflow
  {ForEachVertex(dataflow, actor) {
      VHDLComponent newComp(dataflow, actor);
      circuit.addComponent(newComp);
    }}
  {ForEachEdge(dataflow, edge) {
      VHDLConnection newConn(dataflow, edge);
      circuit.addConnection(newConn);
    }}
  generateOperators(circuit, componentDir, referenceDir, bufferless);
  generateCircuit(circuit, dirName, bufferless);
  std::cout << circuit.printStatus() << std::endl;
  if (circuit.getMultiOutActors().size()) {
    std::cout << "Actors with abnormal number of outputs detected:\n"
              << "\t1. Edit and run generated bash script "
              << "(merge_outs_" << dataflow->getGraphName() << ".sh)\n"
              << "\t2. Re-run VHDL generation on the resulting SDF (" << dataflow->getGraphName()
              << "_merged.xml)" << std::endl;
    generateMergingScript(circuit.getMultiOutActors(), dataflow->getGraphName(),
                          dirName, referenceDir);
  }
  std::cout << "VHDL files generated in: " << dirName << std::endl;
  return;
}

void algorithms::generateOperators(VHDLCircuit &circuit, std::string compDir,
                                   std::string compRefDir, bool isBufferless) {
  std::map<std::string, int> operatorMap = circuit.getOperatorMap();
  // Generate VHDL files for individual components
  for (auto const &op : operatorMap) {
    std::cout << "Generate VHDL component file for " << op.first << std::endl;
    // track number of outputs for cases where operator can have different number of outputs
    std::map<int, int> outputCounts;
    outputCounts = circuit.getNumOutputs(op.first);
    if (op.first == "Proj") {
      generateSplitterOperators(compDir, compRefDir, outputCounts);
    } else if (op.first == "const_value") {
      generateConstOperator(compDir, compRefDir, outputCounts);
    } else if (op.first == "int2float" || op.first == "float2int") {
      generateConversionOperators(circuit.getFirstComponentByType(op.first),
                                  compDir, compRefDir);
    } else {
      generateOperator(circuit.getFirstComponentByType(op.first),
                       compDir, compRefDir);
    }

  }
  generateAXIInterfaceComponents(compDir, compRefDir, isBufferless);

}

void algorithms::generateConstOperator(std::string compDir,
                                       std::string referenceDir,
                                       std::map<int, int> outputCounts) {
  std::ofstream vhdlOutput;
  std::string operatorFileName = "const_value";
  std::string wordsToReplace[] = {"$N_OUTPUTS", "$PORT_LIST", "$PORT_CONNECTIONS"};
  // generate separate const_value components for given number of outputs required
  for (auto &i : outputCounts) {
    vhdlOutput.open(compDir + operatorFileName + "_" + std::to_string(i.first)
                    + ".vhd");
    std::stringstream portListStream;
    std::stringstream portConnStream;
    std::string portList;
    std::string portConn;
    std::string portListEnd = ";\n";
    // generate port names and behaviour
    for (int p = 0; p < i.first; p++) {
      if (p + 1 == i.first) {
        portListEnd = "";
      }
      portListStream << "out_ready_" << p << " : in std_logic;\n";
      portListStream << "out_valid_" << p << " : out std_logic;\n";
      portListStream << "out_data_" << p
                     << " : out std_logic_vector (33 downto 0) := (others => '0')"
                     << portListEnd << std::endl;
      portConnStream << "out_valid_" << p << " <= '1';\n";
      portConnStream << "out_data_" << p << " <= value;\n" << std::endl;
    }
    portList = portListStream.str();
    portConn = portConnStream.str();
    std::map<std::string, std::string> replacementWords = {{"$N_OUTPUTS", std::to_string(i.first)},
                                                           {"$PORT_LIST", portList},
                                                           {"$PORT_CONNECTIONS", portConn}};
    std::ifstream compReference(referenceDir + operatorFileName + "_n_outputs.vhd");
    std::string fileContent;
    if (compReference.is_open()) {
      while (std::getline(compReference, fileContent)) {
        for (const std::string &word : wordsToReplace) { // TODO account for multiple occurances in single line
          size_t pos = fileContent.find(word);
          if (pos != std::string::npos) {
            fileContent.replace(pos, word.length(),
                                replacementWords[word]);
          }
        }
        vhdlOutput << fileContent << std::endl;
      }
      compReference.close();
      vhdlOutput.close();
    } else {
      std::cout << "Reference file for " << operatorFileName
                << " does not exist/not found!" << std::endl; // TODO turn into assert
    }
  }
}

// Generate type conversion operators
void algorithms::generateConversionOperators(VHDLComponent comp, std::string compDir,
                                             std::string referenceDir) {
  std::ofstream vhdlOutput;
  std::string entityName = comp.getType();
  std::string axiInterfaceName = "axi_" + entityName;
  vhdlOutput.open(compDir + entityName + ".vhd");
  std::ifstream operatorRef(referenceDir + entityName + ".vhd");
  std::string fileContent;
  // generate conversion operator
  if (operatorRef.is_open()) {
    while (std::getline(operatorRef, fileContent)) {
      vhdlOutput << fileContent << std::endl;
    }
    operatorRef.close();
    vhdlOutput.close();
  } else {
    std::cout << "Reference file for " << comp.getType()
              << " does not exist/not found!" << std::endl; // TODO turn into assert
  }
  // generate AXI interface for conversion operator
  vhdlOutput.open(compDir + axiInterfaceName + ".vhd");
  std::ifstream interfaceRef(referenceDir + axiInterfaceName + ".vhd");
  if (interfaceRef.is_open()) {
    while (std::getline(interfaceRef, fileContent)) {
      vhdlOutput << fileContent << std::endl;
    }
    interfaceRef.close();
    vhdlOutput.close();
  } else {
    std::cout << "Reference file for " << comp.getType()
              << " does not exist/not found!" << std::endl; // TODO turn into assert
  }
}

// Generate AXI splitter components for the Proj components in the circuit (defined by number of outputs)
void algorithms::generateSplitterOperators(std::string compDir, std::string referenceDir,
                                           std::map<int, int> outputCounts) {
  std::ofstream vhdlOutput;
  std::string operatorFileName = "axi_splitter";
  for (auto &i : outputCounts) {
    std::string refFileName = operatorFileName + "_" + std::to_string(i.first) + ".vhd";
    std::ifstream compReference(referenceDir + refFileName);
    std::string fileContent;
    vhdlOutput.open(compDir + refFileName);
    if (compReference.is_open()) {
      while (std::getline(compReference, fileContent)) {
        vhdlOutput << fileContent << std::endl;
      }
      compReference.close();
      vhdlOutput.close();
    } else {
      std::cout << "Reference file for " << refFileName
                << " does not exist/not found!" << std::endl; // TODO turn into assert
    }
  }

}

// Copy FloPoCo operator from reference file to project
void algorithms::generateFPCOperator(VHDLComponent comp, std:: string compDir,
                                     std::string referenceDir) {
  std::ofstream vhdlOutput;
  std::string operatorRefDir = referenceDir + "/operators/";
  std::string operatorFileName = comp.getType() + "_flopoco";
  // workaround for diff hack
  if (comp.getType() == "fp_diff" || comp.getType() == "int_diff") { // TODO separate operator type from name with data type
    operatorFileName = comp.getDataType() + "_" + "add_flopoco"; // we use an add operator and negate the second argument
  }
  vhdlOutput.open(compDir + operatorFileName + ".vhd"); // instantiate VHDL file
  std::ifstream operatorRef(operatorRefDir + operatorFileName + ".vhdl");
  std::string fileContent;
  if (operatorRef.is_open()) {
    while (std::getline(operatorRef, fileContent)) {
      vhdlOutput << fileContent << std::endl;
    }
    operatorRef.close();
    vhdlOutput.close();
  } else {
    std::cout << "Reference file for " << comp.getType()
              << " does not exist/not found!" << std::endl; // TODO turn into assert
  }
}

// Generate AXI interface for each FloPoCo operator
void algorithms::generateOperator(VHDLComponent comp, std::string compDir,
                                  std::string referenceDir) {
  std::ofstream vhdlOutput;
  std::string entityName = comp.getType();
  std::string componentName = entityName + "_flopoco";
  std::string wordsToReplace[] = {"$ENTITY_NAME", "$FLOPOCO_OP_NAME",
                                  "$COMPONENT_NAME", "$OP_LIFESPAN",
                                  "$AXM_TYPE"};
  std::string opInputCount = std::to_string(comp.getInputPorts().size());

  if (comp.getType() != "INPUT" && comp.getType() != "OUTPUT" && !comp.isConst()) {
    // generate flopoco operators
    generateFPCOperator(comp, compDir, referenceDir); // generate FloPoCo operator
    vhdlOutput.open(compDir + entityName + ".vhd"); // instantiate VHDL file
    std::ifstream operatorRef(referenceDir + "flopoco_axi_interface" +
                              + "_" + opInputCount + ".vhd");
    std::string fileContent;
    std::string operatorName = comp.getFPCName();
    std::string axmType = "";
    if (comp.getType() == "fp_diff" || comp.getType() == "int_diff") {
      axmType = "_negate";
    } else if (comp.getInputPorts().size() == 1) {
      axmType = "_one";
    }
    std::map<std::string, std::string> replacementWords = {{"$ENTITY_NAME", entityName},
                                                           {"$FLOPOCO_OP_NAME", operatorName},
                                                           {"$COMPONENT_NAME", componentName},
                                                           {"$OP_LIFESPAN", std::to_string(comp.getLifespan())},
                                                           {"$AXM_TYPE", axmType}};
    if (operatorRef.is_open()) {
      while (std::getline(operatorRef, fileContent)) {
        for (const std::string &word : wordsToReplace) { // TODO account for multiple occurances in single line
          size_t pos = fileContent.find(word);
          if (pos != std::string::npos) {
            fileContent.replace(pos, word.length(),
                                replacementWords[word]);
          }
        }
        vhdlOutput << fileContent << std::endl;
      }
      operatorRef.close();
      vhdlOutput.close();
    } else {
      std::cout << "Reference file for flopoco_axi_interface does not exist/not found!"
                << std::endl;
    }
  }
}

void algorithms::generateCircuit(VHDLCircuit &circuit, std::string outputDir,
                                 bool isBufferless) {
  std::ofstream vhdlOutput;
  std::string graphName = circuit.getName() + "_circuit"; // TODO decide on naming convention
  int numInputPorts = circuit.getOperatorCount("INPUT");
  int numOutputPorts = circuit.getOperatorCount("OUTPUT");
  std::map<std::string, int> operatorMap = circuit.getOperatorMap();
  bool noOperators = operatorMap.size() == 2 && operatorMap.count("INPUT") && operatorMap.count("OUTPUT"); // edge case where Faust program has no operators (only inputs/outputs)

  vhdlOutput.open(outputDir + graphName + ".vhd"); // instantiate VHDL file
  // 1. Define libraries used
  vhdlOutput << "library ieee;\n"
             << "use ieee.std_logic_1164.all;\n"
             << "use ieee.numeric_std.all;\n" << std::endl;
  // 2. Port declarations
  vhdlOutput << "entity " << circuit.getName() << " is\n"
             << "generic (\n"
             << "    " << "ram_width : natural := 34;\n"
             << "    " << "ram_depth : natural := 2);\n" // buffer size
             << "port (\n"
             << "    " << "clk : in std_logic;\n"
             << "    " << "rst : in std_logic;\n"
             << std::endl;
  // Specify ready, valid, and data ports for each input port:
  for (auto i = 0; i < numInputPorts; i++) {
    // TODO define portName variable outside of loop to minimize unnecessary repetition
    //      (also, for loop should be outside if loop)
    std::string portName = "    " + circuit.getName() + "_in";
    vhdlOutput << portName + "_ready_" + std::to_string(i) + " : out std_logic;\n"
               << portName + "_valid_" + std::to_string(i) + " : in std_logic;\n"
               << portName + "_data_" + std::to_string(i) + " : in std_logic_vector("
               << "ram_width - 1 downto 0) := (others => '0');\n"
               << std::endl;
  }
  // Specify ready, valid, and data ports for each output port:
  for (auto i = 0; i < numOutputPorts; i++) {
    std::string portName = "    " + circuit.getName() + "_out";
    vhdlOutput << portName + "_ready_" + std::to_string(i) + " : in std_logic;\n"
               << portName + "_valid_" + std::to_string(i) + " : out std_logic;\n";
    if (i + 1 == numOutputPorts) {
      vhdlOutput << portName + "_data_" + std::to_string(i) + " : out std_logic_vector("
                 << "ram_width - 1 downto 0) := (others => '0')\n"
                 << std::endl; // last line of port declaration has no terminating semicolon
    } else {
      vhdlOutput << portName + "_data_" + std::to_string(i) + " : out std_logic_vector("
                 << "ram_width - 1 downto 0) := (others => '0');\n"
                 << std::endl;
    }
  }
  vhdlOutput << ");\nend " << circuit.getName() << ";\n" << std::endl;

  // 3. Specify architecture (behaviour) of operator type
  // component declaration
  vhdlOutput << "architecture behaviour of " << circuit.getName() << " is\n" << std::endl;
  if (!noOperators) { // no need to generate components other than buffers if there aren't any operators
    std::map<int, int> constOutputs;
    for (auto const &op : operatorMap) {
      if (op.first != "INPUT" && op.first != "OUTPUT") {
        std::map<int, int> outputCounts;
        outputCounts = circuit.getNumOutputs(op.first);
        if (op.first == "Proj") {
          vhdlOutput << generateSplitterComponents(outputCounts) << std::endl;
        } else if (op.first == "const_value" ||
                   circuit.getFirstComponentByType(op.first).isConst()) {
          constOutputs.insert(outputCounts.begin(), outputCounts.end()); // workaround for UI components
        } else {
          vhdlOutput << generateComponent(circuit.getFirstComponentByType(op.first))
                     << std::endl;
        }

      }
    }
    if (constOutputs.size()) {
      vhdlOutput << generateConstComponents(constOutputs) << std::endl;
    }
  }
  if (!isBufferless) {
    vhdlOutput << generateBufferComponent(circuit.getName()) << std::endl;
  }
  // Track top-level input and output signals
  std::vector<std::string> inSignalNames;
  std::vector<std::string> outSignalNames;
  if (isBufferless) { // no FIFO buffers between components
    for (auto &op : circuit.getComponentMap()) {
      if (op.second.getType() == "INPUT") {
        for (auto &i : op.second.getOutputEdges()) {
          inSignalNames.push_back(i);
        }
      } else if (op.second.getType() == "OUTPUT") {
        for (auto &i : op.second.getInputEdges()) {
          outSignalNames.push_back(i);
        }
      }
    }
  } else { // use port names to generate signals to/from FIFO buffers
    for (auto &op : circuit.getComponentMap()) {
      if (op.second.getType() == "INPUT") {
        for (auto &i : op.second.getOutputPorts()) {
          inSignalNames.push_back(i);
        }
      } else if (op.second.getType() == "OUTPUT") {
        for (auto &i : op.second.getInputPorts()) {
          outSignalNames.push_back(i);
        }
      }
    }
  }

  // Define and generate signal names
  int inCount = 0;
  int outCount = 0;
  bool isTopInput;
  bool isTopOutput;
  for (auto &connection : circuit.getConnectionMap()) {
    if (isBufferless) {
      isTopInput = std::find(inSignalNames.begin(), inSignalNames.end(),
                             connection.second.getName()) != inSignalNames.end();
      isTopOutput = std::find(outSignalNames.begin(), outSignalNames.end(),
                              connection.second.getName()) != outSignalNames.end();
    } else {
      isTopInput = std::find(inSignalNames.begin(), inSignalNames.end(),
                             connection.second.getSrcPort()) != inSignalNames.end();
      isTopOutput = std::find(outSignalNames.begin(), outSignalNames.end(),
                              connection.second.getDstPort()) != outSignalNames.end();
    }

    if (isTopInput) {
      std::vector<std::string> signalNames(3);
      signalNames[VALID] = circuit.getName() + "_in_valid_" + std::to_string(inCount);
      signalNames[READY] = circuit.getName() + "_in_ready_" + std::to_string(inCount);
      signalNames[DATA] = circuit.getName() + "_in_data_" + std::to_string(inCount);
      if (isBufferless && !noOperators) {
        circuit.addInputPort(connection.second.getName(), signalNames);
      } else if (isBufferless && noOperators) { // TODO can be reduced to fewer conditions
        circuit.addInputPort(connection.second.getSrcPort(), signalNames);
      } else {
        if (!noOperators) {
          circuit.addInputPort(connection.second.getDstPort(), signalNames);
        } else { // if there are only inputs/outputs, then the top-level input port is the output port of the INPUT operators
          circuit.addInputPort(connection.second.getSrcPort(), signalNames);
        }
      }
      inCount++;
    }
    if (isTopOutput) {
      std::vector<std::string> signalNames(3);
      signalNames[VALID] = circuit.getName() + "_out_valid_" + std::to_string(outCount);
      signalNames[READY] = circuit.getName() + "_out_ready_" + std::to_string(outCount);
      signalNames[DATA] = circuit.getName() + "_out_data_" + std::to_string(outCount);
      if (isBufferless && !noOperators) {
        circuit.addOutputPort(connection.second.getName(), signalNames);
      } else if (isBufferless && noOperators) { // TODO can be reduced to fewer conditions
        circuit.addOutputPort(connection.second.getDstPort(), signalNames);
      } else {
        if (!noOperators) {
          circuit.addOutputPort(connection.second.getSrcPort(), signalNames);
        } else { // if there are only inputs/outputs, then the top-level input port is the input port of the OUTPUT operators
          circuit.addOutputPort(connection.second.getDstPort(), signalNames);
        }
      }
      outCount++;
    }
  }

  std::vector<std::string> dataSignals;
  std::vector<std::string> validReadySignals;
  for (auto &connection : circuit.getConnectionMap()) {
    bool isNotTopInOut; // check if connection belongs to top level input/output
    if (isBufferless) {
      isNotTopInOut = !(circuit.getInputPorts().count(connection.second.getName()) ||
                        circuit.getOutputPorts().count(connection.second.getName()));
    } else {
      isNotTopInOut = !(circuit.getInputPorts().count(connection.second.getDstPort()) ||
                        circuit.getOutputPorts().count(connection.second.getSrcPort()));
    }
    // only generate signal names for non-input/output ports
    if (!noOperators) {
      if (isNotTopInOut) {
        if (isBufferless) {
          std::vector<std::string> signalNames(generateSendSigNames(connection.second.getName(), circuit));
          dataSignals.push_back(signalNames[DATA]);
          validReadySignals.push_back(signalNames[VALID]);
          validReadySignals.push_back(signalNames[READY]);
        } else {
          // separate send/receive signals required due to buffers between components
          std::vector<std::string> sendSignals(generateSendSigNames(connection.second.getSrcPort(), circuit));
          std::vector<std::string> receiveSignals(generateReceiveSigNames(connection.second.getDstPort(), circuit));
          dataSignals.push_back(sendSignals[DATA]);
          dataSignals.push_back(receiveSignals[DATA]);
          validReadySignals.push_back(sendSignals[VALID]);
          validReadySignals.push_back(receiveSignals[VALID]);
          validReadySignals.push_back(sendSignals[READY]);
          validReadySignals.push_back(receiveSignals[READY]);
        }
      }
    } else {
      if (isBufferless) {
        std::vector<std::string> signalNames(generateSendSigNames(connection.second.getName(), circuit));
        dataSignals.push_back(signalNames[DATA]);
        validReadySignals.push_back(signalNames[VALID]);
        validReadySignals.push_back(signalNames[READY]);
      } else {
        // separate send/receive signals required due to buffers between components
        std::vector<std::string> sendSignals(generateSendSigNames(connection.second.getSrcPort(), circuit));
        std::vector<std::string> receiveSignals(generateReceiveSigNames(connection.second.getDstPort(), circuit));
        dataSignals.push_back(sendSignals[DATA]);
        dataSignals.push_back(receiveSignals[DATA]);
        validReadySignals.push_back(sendSignals[VALID]);
        validReadySignals.push_back(receiveSignals[VALID]);
        validReadySignals.push_back(sendSignals[READY]);
        validReadySignals.push_back(receiveSignals[READY]);
      }
    }
  }
  // data signals
  if (!noOperators) { // no need to use internal signals if no operators
    vhdlOutput << "signal ";
    std::string delim = ",";
    int counter = 0;
    for (auto &signal : dataSignals) {
      counter++;
      if (counter == dataSignals.size()) {
        delim = "";
      }
      vhdlOutput << signal << delim << std::endl;
    }
    vhdlOutput << " : std_logic_vector(ram_width - 1 downto 0);\n"
               << std::endl;
    // valid and ready signals
    vhdlOutput << "signal ";
    delim = ",";
    counter = 0;
    for (auto &signal : validReadySignals) {
      counter++;
      if (counter == validReadySignals.size()) {
        delim = "";
      }
      vhdlOutput << signal << delim << std::endl;
    }
    vhdlOutput << " : std_logic;\n" << std::endl;
    vhdlOutput << generatePortMapping(circuit, isBufferless, noOperators) << std::endl;
    vhdlOutput << "end behaviour;" << std::endl;

    vhdlOutput.close();
  } else {
    vhdlOutput << generatePortMapping(circuit, isBufferless, noOperators) << std::endl;
    vhdlOutput << "end behaviour;" << std::endl;
  }
}

std::string algorithms::generateComponent(VHDLComponent comp) {
  std::stringstream outputStream;
  std::string componentName;
  if (comp.getType() == "const_value") {
    componentName = comp.getType();
  } else if (comp.getType() == "int2float" || comp.getType() == "float2int") {
    componentName = "axi_" + comp.getType();
  } else {
    componentName = comp.getType();
  }
  int numInputPorts;
  int numOutputPorts;
  // TODO assert that component is not an INPUT/OUTPUT
  numInputPorts = comp.getInputPorts().size();
  numOutputPorts = comp.getOutputPorts().size();
  // every component requires clock and reset ports
  outputStream << "component " << componentName << " is" << std::endl;
  if (comp.getType() == "const_value") {
    outputStream << "generic (\n"
                 << "    " << "value : std_logic_vector(ram_width - 1 downto 0)\n"
                 << ");" << std::endl;
  }
  outputStream << "port (\n"
               << "    " << "clk : in std_logic;\n"
               << "    " << "rst : in std_logic;\n"
               << std::endl;
  // Specify ready, valid, and data ports for each input port:
  for (auto i = 0; i < numInputPorts; i++) {
    std::string portName = "    op_in";
    if (comp.getType() == "const_value") {
      portName = "    in";
    }
    outputStream << portName + "_ready_" + std::to_string(i) + " : out std_logic;\n"
                 << portName + "_valid_" + std::to_string(i) + " : in std_logic;\n"
                 << portName + "_data_" + std::to_string(i) + " : in std_logic_vector("
                 << "ram_width - 1 downto 0);\n"
                 << std::endl;
  }

  // Specify ready, valid, and data ports for each output port:
  for (auto i = 0; i < numOutputPorts; i++) {
    std::string portName = "    op_out";
    if (comp.getType() == "const_value") {
      portName = "    out";
    }
    outputStream << portName + "_ready_" + std::to_string(i) + " : in std_logic;\n"
                 << portName + "_valid_" + std::to_string(i) + " : out std_logic;\n";
    if (i + 1 == numOutputPorts) {
      outputStream << portName + "_data_" + std::to_string(i) + " : out std_logic_vector("
                   << "ram_width - 1 downto 0)\n" << std::endl; // last line of port declaration has no terminating semicolon
    } else {
      outputStream << portName + "_data_" + std::to_string(i) + " : out std_logic_vector("
                   << "ram_width - 1 downto 0);\n" << std::endl;
    }
  }

  outputStream << "); end component;\n" << std::endl;
  return outputStream.str();
}

std::string algorithms::generateBufferComponent(std::string circuitName) {
  std::stringstream outputStream;

  // every component requires clock and reset ports
  outputStream << "component axi_fifo is\n"
               << "generic (\n"
               << "    " << "ram_width : natural;\n"
               << "    " << "ram_depth : natural;\n"
               << "    " << "ram_init : natural\n"
               << ");\n"
               << "port (\n"
               << "    " << "buffer_clk : in std_logic;\n"
               << "    " << "buffer_rst : in std_logic;\n"
               << std::endl;
  outputStream << "    " << "buffer_in_ready : out std_logic;\n"
               << "    " << "buffer_in_valid : in std_logic;\n"
               << "    " << "buffer_in_data : in std_logic_vector("
               << "ram_width - 1 downto 0);\n"
               << std::endl;
  outputStream << "    " << "buffer_out_ready : in std_logic;\n"
               << "    " << "buffer_out_valid : out std_logic;\n"
               << "    " << "buffer_out_data : out std_logic_vector("
               << "ram_width - 1 downto 0)\n"
               << std::endl;
  outputStream << "); end component;\n" << std::endl;
  return outputStream.str();
}

// generate one AXI splitter component for each corresponding Proj component (defined by number of outputs)
// TODO find a way to generalise splitter component and generation
std::string algorithms::generateConstComponents(std::map<int, int> outputCounts) {
  std::stringstream outputStream;
  std::string componentName;
  for (auto &i : outputCounts) {
    componentName = "const_value_" + std::to_string(i.first);
    int numOutputPorts = i.first;
    // every component requires clock and reset ports
    outputStream << "component " << componentName << " is\n"
                 << "generic (\n"
                 << "    " << "value : std_logic_vector(ram_width - 1 downto 0)\n"
                 << ");\n"
                 << "port (\n"
                 << "    " << "clk : in std_logic;\n"
                 << "    " << "rst : in std_logic;\n"
                 << std::endl;
    // Specify ready, valid, and data ports for each output port:
    for (auto i = 0; i < numOutputPorts; i++) {
      std::string portName = "    out";
      outputStream << portName + "_ready_" + std::to_string(i) + " : in std_logic;\n"
                   << portName + "_valid_" + std::to_string(i) + " : out std_logic;\n";
      if (i + 1 == numOutputPorts) {
        outputStream << portName + "_data_" + std::to_string(i) + " : out std_logic_vector("
                     << "ram_width - 1 downto 0)\n" << std::endl; // last line of port declaration has no terminating semicolon
      } else {
        outputStream << portName + "_data_" + std::to_string(i) + " : out std_logic_vector("
                     << "ram_width - 1 downto 0);\n" << std::endl;
      }
    }

    outputStream << "); end component;\n" << std::endl;
  }
  return outputStream.str();
}

// generate one AXI splitter component for each corresponding Proj component (defined by number of outputs)
// TODO find a way to generalise splitter component and generation
std::string algorithms::generateSplitterComponents(std::map<int, int> outputCounts) {
  std::stringstream outputStream;
  std::string componentName;
  for (auto &i : outputCounts) {
    componentName = "axi_splitter_" + std::to_string(i.first);
    int numInputPorts = 1; // NOTE hardcoded here that AXI splitter components all have 1 input
    int numOutputPorts = i.first;
    // every component requires clock and reset ports
    outputStream << "component " << componentName << " is\n"
                 << "generic (\n"
                 << "    " << "bit_width : natural := ram_width\n"
                 << ");\n"
                 << "port (\n"
                 << "    " << "clk : in std_logic;\n"
                 << "    " << "rst : in std_logic;\n"
                 << std::endl;
    // Specify ready, valid, and data ports for each input port:
    for (auto i = 0; i < numInputPorts; i++) {
      std::string portName = "    in";
      outputStream << portName + "_ready_" + std::to_string(i) + " : out std_logic;\n"
                   << portName + "_valid_" + std::to_string(i) + " : in std_logic;\n"
                   << portName + "_data_" + std::to_string(i) + " : in std_logic_vector("
                   << "ram_width - 1 downto 0);\n"
                   << std::endl;
    }

    // Specify ready, valid, and data ports for each output port:
    for (auto i = 0; i < numOutputPorts; i++) {
      std::string portName = "    out";
      outputStream << portName + "_ready_" + std::to_string(i) + " : in std_logic;\n"
                   << portName + "_valid_" + std::to_string(i) + " : out std_logic;\n";
      if (i + 1 == numOutputPorts) {
        outputStream << portName + "_data_" + std::to_string(i) + " : out std_logic_vector("
                     << "ram_width - 1 downto 0)\n" << std::endl; // last line of port declaration has no terminating semicolon
      } else {
        outputStream << portName + "_data_" + std::to_string(i) + " : out std_logic_vector("
                     << "ram_width - 1 downto 0);\n" << std::endl;
      }
    }

    outputStream << "); end component;\n" << std::endl;
  }
  return outputStream.str();
}

// Copy FloPoCo-AXI interface component specification from reference files to generated subdirectory
void algorithms::generateAXIInterfaceComponents(std::string compDir,
                                                std::string referenceDir,
                                                bool isBufferless) {
  std::ofstream vhdlOutput;
  // names of reference files required to copy into project; add/remove as required
  std::vector<std::string> componentNames = {"axi_merger", "delay",
                                             "store_send", "axi_merger_negate",
                                             "axi_merger_one"};
  if (!isBufferless) {
    componentNames.push_back("axi_fifo");
    componentNames.push_back("axi_fifo_n");
    componentNames.push_back("axi_fifo_one");
    componentNames.push_back("axi_fifo_zero");
  }

  for (const auto &component : componentNames) {
    vhdlOutput.open(compDir + component + ".vhd");
    std::ifstream compReference(referenceDir + component + ".vhd");
    std::string fileContent;
    if (compReference.is_open()) {
      while (std::getline(compReference, fileContent)) {
        vhdlOutput << fileContent << std::endl;
      }
      compReference.close();
      vhdlOutput.close();
    } else {
      std::cout << "Reference file for " << component
                << " does not exist/not found!" << std::endl; // TODO turn into assert
    }
  }
}

std::vector<std::string> algorithms::generateSendSigNames(std::string srcPort,
                                                          VHDLCircuit circuit) {
  std::vector<std::string> sendSignals(3);

  if (circuit.getOutputPorts().count(srcPort)) {
    sendSignals = circuit.getOutputPorts()[srcPort];
  } else {
    sendSignals[VALID] = srcPort + "_VALID";
    sendSignals[READY] = srcPort + "_READY";
    sendSignals[DATA] = srcPort + "_DATA";
  }
  return sendSignals;
}

std::vector<std::string> algorithms::generateReceiveSigNames(std::string dstPort,
                                                             VHDLCircuit circuit) {
  std::vector<std::string> receiveSignals(3);

  if (circuit.getInputPorts().count(dstPort)) {
    receiveSignals = circuit.getInputPorts()[dstPort];
  } else {
    receiveSignals[VALID] = dstPort + "_VALID";
    receiveSignals[READY] = dstPort + "_READY";
    receiveSignals[DATA] = dstPort + "_DATA";
  }

  return receiveSignals;
}

// Generate port mapping for each operator in the circuit
std::string algorithms::generatePortMapping(VHDLCircuit circuit,
                                            bool isBufferless, bool noOperators) {
  std::stringstream outputStream;
  std::string componentName;
  std::map<std::string, int> opCount(circuit.getOperatorMap());

  outputStream << "begin\n" << std::endl;
  // initialise counts
  for (auto &c : opCount) {
    c.second = 0;
  }
  for (auto &op : circuit.getComponentMap()) {
    if (op.second.getType() != "INPUT" && op.second.getType() != "OUTPUT") {
      std::string opName = op.second.getType();
      if (op.second.isConst()) {
        opName = op.second.getType() + "_" + std::to_string((op.second).getOutputPorts().size());
        componentName = "const_value_" + std::to_string((op.second).getOutputPorts().size());
      } else if (op.second.getType() == "Proj") {
        opName = "axi_splitter_" + std::to_string((op.second).getOutputPorts().size());
        componentName = opName;
      } else if (op.second.getType() == "int2float" || op.second.getType() == "float2int") {
        componentName = "axi_" + opName;
      } else {
        componentName = opName;
      }
      // reset/clock mappings
      outputStream << opName << "_" + std::to_string(opCount[opName])
                   << " : " << componentName << "\n" << std::endl;
      if (op.second.isConst()) {
        outputStream << "generic map (\n"
                     << "    " << "value => " << "\""
                     << op.second.getBinaryValue() << "\"" << "\n)\n"
                     << std::endl;
      }
      outputStream << "port map (\n"
                   << "    " << "clk => " << "clk,\n"
                   << "    " << "rst => " << "rst,\n"
                   << std::endl;
      // input/output mappings
      int inPortCount = 0;
      int outPortCount = 0;
      std::vector<std::string> inputSignals;
      std::vector<std::string> outputSignals;
      if (isBufferless) {
        inputSignals = op.second.getInputEdges();
        outputSignals = op.second.getOutputEdges();
      } else {
        inputSignals = op.second.getInputPorts();
        outputSignals = op.second.getOutputPorts();
      }
      if (op.second.getArgOrder().size()) { // match input signals to argument ordering given by name
        assert(op.second.getArgOrder().size() == inputSignals.size());
        inputSignals.clear();
        for (auto i = 0; i < op.second.getArgOrder().size(); i++) {
          std::string srcActorName = circuit.getComponentFullName(op.second.getArgOrder()[i]);
          std::string dstActorName = op.second.getName();
          if (isBufferless) {
            inputSignals.push_back(circuit.getConnectionNameFromComponents(srcActorName,
                                                                           dstActorName));
          } else {
            inputSignals.push_back(circuit.getDstPortBetweenComponents(srcActorName,
                                                                       dstActorName));
          }
        }
      }
      for (auto &inPort : inputSignals) {
        std::vector<std::string> receiveSigs(3);
        std::string sigPrefix = "op_";
        if (op.second.isConst() || op.second.getType() == "Proj") {
          sigPrefix = "";
        }
        receiveSigs = generateReceiveSigNames(inPort, circuit);
        outputStream << "    " << sigPrefix << "in_ready_" << std::to_string(inPortCount)
                     << " => " << receiveSigs[READY] << ",\n"
                     << "    " << sigPrefix << "in_valid_" << std::to_string(inPortCount)
                     << " => " << receiveSigs[VALID] << ",\n"
                     << "    " << sigPrefix << "in_data_" << std::to_string(inPortCount)
                     << " => " << receiveSigs[DATA] << ",\n" << std::endl;
        inPortCount++;
      }
      std::string lineEnder = ",";
      size_t numOutPorts = outputSignals.size();
      size_t iteration = 1;
      for (auto &outPort : outputSignals) {
        std::vector<std::string> sendSigs(3);
        std::string sigPrefix = "op_";
        if (op.second.isConst() || op.second.getType() == "Proj") {
          sigPrefix = "";
        }
        sendSigs = generateSendSigNames(outPort, circuit);
        if (iteration == numOutPorts) {
          lineEnder = "";  // last port mapping terminates without a comma
        }
        outputStream << "    " << sigPrefix << "out_ready_" << std::to_string(outPortCount)
                     << " => " << sendSigs[READY] << ",\n"
                     << "    " << sigPrefix << "out_valid_" << std::to_string(outPortCount)
                     << " => " << sendSigs[VALID] << ",\n"
                     << "    " << sigPrefix << "out_data_" << std::to_string(outPortCount)
                     << " => " << sendSigs[DATA] << lineEnder
                     << std::endl;
        outPortCount++;
        iteration++;
      }

      outputStream << ");\n" << std::endl;
      opCount[opName]++;
    }
  }

  // buffer port mappings
  if (!isBufferless) {
    int bufferCount = 0;
    std::string bCompName = "axi_fifo";
    std::string bName = "buffer";
    for (auto &buffer : circuit.getConnectionMap()) {
      // only generate buffer between non-input/output components
      if (!(circuit.getInputPorts().count(buffer.second.getDstPort()) ||
            circuit.getOutputPorts().count(buffer.second.getSrcPort()))) {
        std::vector<std::string> sendSigs;
        std::vector<std::string> receiveSigs;
        if (!noOperators) {
          sendSigs = generateSendSigNames(buffer.second.getSrcPort(), circuit);
          receiveSigs = generateReceiveSigNames(buffer.second.getDstPort(), circuit);
        } else { // NOTE workaround to deal with case where only inputs/outputs in Faust program
          sendSigs = generateReceiveSigNames(buffer.second.getSrcPort(), circuit);
          receiveSigs = generateSendSigNames(buffer.second.getDstPort(), circuit);
        }
        // ram width/depth, reset, and clock mappings
        outputStream << "fifo_" + std::to_string(bufferCount)
                     << " : " << bCompName << "\n"
                     <<"generic map (\n"
                     << "    " << "ram_width => ram_width,\n"
                     << "    " << "ram_depth => " << buffer.second.getBufferSize() << ",\n"
                     << "    " << "ram_init => " << buffer.second.getInitialTokenCount() << "\n)\n"
                     << std::endl;
        outputStream << "port map (\n"
                     << "    " << bName << "_clk => " << "clk,\n"
                     << "    " << bName << "_rst => " << "rst,\n"
                     << std::endl;
        // input/output mappings
        outputStream << "    " << bName << "_in_ready => " << sendSigs[READY] << ",\n"
                     << "    " << bName << "_in_valid => " << sendSigs[VALID] << ",\n"
                     << "    " << bName << "_in_data => " << sendSigs[DATA] << ",\n"
                     << std::endl;
        outputStream << "    " << bName << "_out_ready => " << receiveSigs[READY] << ",\n"
                     << "    " << bName << "_out_valid => " << receiveSigs[VALID] << ",\n"
                     << "    " << bName << "_out_data => " << receiveSigs[DATA] << "\n);\n"
                     << std::endl;
        bufferCount++;
      }
    }
  } else {
    if (noOperators) { // just need to pass through data if no operators
      std::vector<std::string> sendSigs;
      std::vector<std::string> receiveSigs;
      for (auto &buffer : circuit.getConnectionMap()) {
        sendSigs = generateReceiveSigNames(buffer.second.getSrcPort(), circuit);
        receiveSigs = generateSendSigNames(buffer.second.getDstPort(), circuit);
      }
      outputStream << receiveSigs[VALID] << "<=" << sendSigs[VALID] << ";\n";
      outputStream << receiveSigs[DATA] << "<=" << sendSigs[DATA] << ";\n";
    }
  }

  return outputStream.str();
}

// generate a bash script to distribute the outputs of certain actors using additional Proj operators
void algorithms::generateMergingScript(std::vector<std::string> actorNames, std::string graphName,
                                          std::string dirName, std::string referenceDir) {
  std::ofstream bashOutput;
  std::string scriptName = "merge_outs";
  std::stringstream nameList;
  std::string wordsToReplace[] = {"#OUTPUT_DIR", "#GRAPH_NAME",
                                  "#MERGED_NAME", "#ACTOR_NAMES"};
  for (auto& name : actorNames) {
    nameList << name << " ";
  }
  std::map<std::string, std::string> replacementWords = {{"#OUTPUT_DIR", "\"" + dirName + "\""},
                                                         {"#GRAPH_NAME", "\"" + graphName + ".dsp-sig.xml" + "\""},
                                                         {"#MERGED_NAME", "\"" + graphName + "_merged.xml" + "\""},
                                                         {"#ACTOR_NAMES", nameList.str()}};
  std::ifstream scriptRef(referenceDir + scriptName + ".sh");
  std::string scriptContent;
  bashOutput.open(dirName + scriptName + "_" + graphName + ".sh");
  if (scriptRef.is_open()) {
    while (std::getline(scriptRef, scriptContent)) {
      for (const std::string &word : wordsToReplace) {
        size_t pos = scriptContent.find(word);
        if (pos != std::string::npos) {
          scriptContent.replace(pos, word.length(),
                                replacementWords[word]);
        }
      }
      bashOutput << scriptContent << std::endl;
    }
    bashOutput.close();
  } else {
      std::cout << "Reference file for " << scriptName
                << " does not exist/not found!" << std::endl; // TODO turn into assert
  }
}
