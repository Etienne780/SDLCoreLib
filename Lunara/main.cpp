#include <CoreLib/Log.h>
#include <iostream>

int main() {
	Log::SetLevel(Log::levelWarning);
	
	Log::Print(Log::levelInfo, "Hello World!");
	Log::Info("Hello World!");
	Log::Warn("Hello World!");
	Log::Error("Hello World!");
	Log::Print("");

	return 0;
}