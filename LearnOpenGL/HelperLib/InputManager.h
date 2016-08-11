#pragma once

#include <unordered_map>

namespace HelperLib {

	class InputManager
	{
	public:
		InputManager();
		~InputManager();

		void update();

		void pressKey(unsigned int keyID);
		void releaseKey(unsigned int keyID);

		bool wasKeyDown(unsigned int keyID);

		// Returns true if the key was just presed (we need to check was it pressed the freme before)
		bool isKeyPressed(unsigned int keyID);

		// Returns true if the key is held down
		bool isKeyDown(unsigned int keyID);

	private:

		std::unordered_map<unsigned int, bool> _keyMap;
		std::unordered_map<unsigned int, bool> _previousKeyMap;
		

	};

}



