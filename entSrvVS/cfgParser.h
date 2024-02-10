#pragma once

#include <iostream>
#include <string>
#include <fstream>

class CfgParser
{
	std::string configPath = "./entSrvVs.conf";
public:
	int parseCfg(std::string target);

};

