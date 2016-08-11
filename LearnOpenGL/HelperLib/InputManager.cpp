#include "InputManager.h"

namespace HelperLib {

	InputManager::InputManager()
	{
	}


	InputManager::~InputManager()
	{
	}

	void InputManager::setMouseCoords(float x, float y) {
		_mouseCoords.x = x;
		_mouseCoords.y = y;
	}

	void InputManager::update()
	{
		// Loop through keyMap using foreach and copy it over to _previousKeyMap
		for (auto& it : _keyMap) {
			_previousKeyMap[it.first] = it.second;
		}
	}

	void InputManager::pressKey(unsigned int keyID) {
		_keyMap[keyID] = true;
	}

	void InputManager::releaseKey(unsigned int keyID) {
		_keyMap[keyID] = false;
	}


	bool InputManager::wasKeyDown(unsigned int keyID)
	{
		auto it = _previousKeyMap.find(keyID);
		if (it != _previousKeyMap.end()) {
			// Found the key
			return it->second;
		}
		else {
			// Didn't find the key
			return false;
		}
	}

	bool InputManager::isKeyPressed(unsigned int keyID)
	{
		// Check if it was pressed this frame, and wasn't pressed last frame
		if (isKeyDown(keyID) == true && wasKeyDown(keyID) == false)
			return true;
		else
			return false;
	}

	bool InputManager::isKeyDown(unsigned int keyID) {
		auto it = _keyMap.find(keyID);
		if (it != _keyMap.end()) {
			// Found the key
			return it->second;
		}
		else {
			// Didn't find the key
			return false;
		}
	}

}


