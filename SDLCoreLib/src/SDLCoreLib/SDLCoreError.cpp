#pragma once
#include <string>
#include <CoreLib/FormatUtils.h>

#include "SDLCoreTypes.h"

namespace SDLCore {

	static std::string s_errorMsg;

	std::string GetError(SDLResult result) {
		std::string msg;
		switch (result)
		{
		case 0:
			msg = "Application closed successfully";
			break;
		case 1:
			msg = "Application SDL3 error!\n" + s_errorMsg;
			break;
		case 2:
			msg = "Application SDL3_ttf error!\n" + s_errorMsg;
			break;
		case 3:
			msg = "Application SDL3_mixer error!\n" + s_errorMsg;
			break;
		default:
			msg = "Result '" + FormatUtils::toString(result) + "' is Unknon!";
		}

		return msg;
	}

	std::string GetError() {
		return s_errorMsg;
	}

	void SetError(const std::string& msg) {
		s_errorMsg = msg;
	}

	void AddError(const std::string& msg) {
		s_errorMsg += msg;
	}

}