
#include <iostream>
#include <fstream>
#include "appData.h"
#include <SDL3/SDL.h>
#include "../platform/platform_data.h"
#include "../platform/platform.h"

namespace application {

	AppState gAppState;

	std::string ReadShaderFile(const char* _filename)
	{
		// Open the file for reading
		std::ifstream file(_filename, std::ios::binary);

		// Ensure the file is open and readable
		if (!file.good()) {

			// #TODO maybe change this std::string usage to be the future arena allocators. 

			// Create the error string
			std::string mainString = "Cannot read file: ";
			mainString.append(_filename);

			// Log the error 
			std::cout << mainString.c_str();

			return "";
		}

		std::string shaderCode((std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());
		/*Log((std::string("Loaded shader file: ") + _filename +
			" (" + std::to_string(shaderCode.size()) + " bytes)").c_str());*/
			/*std::string success = "Loaded shader file: " + std::string(_filename) +
				" (" + std::to_string(size) + " bytes)";
			Log(success.c_str());*/

		return shaderCode;
	}
}