#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Windows.h>
#include <time.h>
#include <string>
#include <vector>

namespace util {
	//Width and height of window
	const int windowWidth = 800;
	const int windowHeight = 800;

	static sf::Vector2i mousePos = sf::Vector2i();

	static float CurrentTime = 0.0f;
}

struct PrintString {
	sf::Vector2i m_pos;
	std::string m_string;
	int m_col;

	PrintString(sf::Vector2i _pos, std::string _message, int _col) : m_pos(_pos), m_string(_message), m_col(_col){};
	PrintString(int x, int y, std::string _message, int _col) : m_pos(sf::Vector2i(x, y)), m_string(_message), m_col(_col) {};
};