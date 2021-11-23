/*
 * SDF3Wrapper.cpp
 *
 *  Created on: Jun 3, 2020
 *      Author: toky
 */

#include <commons/verbose.h>
#include <printers/SDF3Wrapper.h>
#include "SDF3Wrapper.h"


TIME_UNIT algorithms::runSDF3Throughput(models::Dataflow* const dataflow, std::string SDF3_binary) {
	static ARRAY_INDEX counter = 0;
	std::string tmp_filename = "/tmp/tmp" + std::to_string(counter++) +  ".xml";
	printers::writeSDF3File (tmp_filename,dataflow);
	std::string cmd = SDF3_binary + " --graph " + tmp_filename + " --algo throughput";

	VERBOSE_INFO (" Running SDF3: " << cmd);

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    std::string result;
    std::array<char, 128> buffer;
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    VERBOSE_INFO(result);

    std::vector<std::string> lines = commons::split<std::string>(result,'\n');
    if (not (lines.size() == 2)) return 0;

    std::string throughput_line = lines[0]; // thr(autogen) = 0.0434783
    std::vector<std::string> thr_fields = commons::split<std::string>(throughput_line,'=');
    if (not (thr_fields.size() == 2)) return 0;
    TIME_UNIT throughput = commons::fromString<TIME_UNIT>(thr_fields[1]);

    return throughput;
}



