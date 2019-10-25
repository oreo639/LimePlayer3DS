#ifndef __APP_LIME_HPP__
#define __APP_LIME_HPP__

#include "explorer.hpp"
#include "player.hpp"

class App {
	public:
		static bool exit;
		static playbackInfo_t pInfo;
		App();
		~App();
		static int MainLoop();
		static void Update();
	private:
		static void LibInit();
		static void LibExit();
};
#endif
