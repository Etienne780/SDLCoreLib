#pragma once
#include <string>

namespace SDLCore {

	/**
	* @brief Gets the msg to the result
	* @param result The  result of the Application
	* @return The result msg
	*/
	std::string GetError(SDLResult result);

}