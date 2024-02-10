

#include "cfgParser.h"

void cutEmpty(std::string& str, size_t strSize) {
	while (str.find(" ") != -1) {
		str.erase(str.find(" "), 1);
	}
}

int readParam(std::string& row, size_t rowSize,
	std::string& paramName,
	std::string& paramValue)
{
	bool flagAfterEqual = false;
	

	auto nameIter = paramName.begin();
	auto valueIter = paramValue.begin();

	for (int i = 0; i < rowSize; i++) {
		if (row[i] == ' ')
			continue;
		if (row[i] ==  '=') {
			flagAfterEqual = true;
			continue;
		}
		if (!flagAfterEqual)
			*nameIter++ = row[i];
		else
			*valueIter++ = row[i];
	}
	if (flagAfterEqual || paramName.empty() || paramValue.empty()) {
		cutEmpty(paramName, rowSize);
		cutEmpty(paramValue, rowSize);
		return 1;
	}
	return 0;
}

int CfgParser::parseCfg(std::string target) {

	//std::string configPath = "./entSrvVs.conf";
	std::string configPath = CfgParser::configPath;
	
	std::ifstream fileIn;
	fileIn.open(configPath);


	if (!fileIn.is_open()) {
		std::cout << "Cannot open config file" << std::endl;
		return 1;
	}
	
	std::string row;
	
	while (!fileIn.eof()) {
		std::getline(fileIn,row);
		
		std::string param(row.size(),' ');

		std::string value(row.size(), ' ');

		if (row.empty() || row.find("=")==-1)
			continue;

		readParam(row, row.size(), param, value);
		if (param == target)
			return std::stoi(value); // cast value to int
		
		
		if (row.find("#") != -1) {
			// целиком комментарий
			void;
			// есть переменная

		}

	}


	return 0;
}