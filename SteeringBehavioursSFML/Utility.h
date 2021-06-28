#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Windows.h>
#include <time.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/closest_point.hpp>



namespace util {
	static glm::vec2 v2(sf::Vector2f _v) {
		glm::vec2 toRet{ _v.x, _v.y };
		return toRet;
	}

	static sf::Vector2f v2(glm::vec2 _v) {
		sf::Vector2f toRet{ _v.x, _v.y };
		return toRet;
	}

	//Width and height of window
	const int windowWidth = 800;
	const int windowHeight = 800;

	static sf::Vector2i mousePos = sf::Vector2i();

	static float CurrentTime = 0.0f;

	static sf::Vector2f normalize(sf::Vector2f _vec) {
		return v2(glm::normalize(v2(_vec)));
	}

	static float length(sf::Vector2f _vec) {
		return glm::length(v2(_vec));
	}

	static float angle(sf::Vector2f _vec1, sf::Vector2f _vec2) {
		return glm::degrees(glm::angle(glm::normalize(v2(_vec1)), glm::normalize(v2(_vec2))));
	}
	
	static sf::Vector2f rotate(sf::Vector2f _vec, float _degrees) {
		return v2(glm::rotate(v2(_vec), glm::radians<float>(_degrees)));
	}

	static sf::Vector2f reflect(sf::Vector2f _vec, sf::Vector2f _norm) {
		return v2(glm::reflect(v2(_vec), v2(_norm)));
	}
	
	static sf::Vector2f closestPoint(sf::Vector2f _point, sf::Vector2f _lineStart, sf::Vector2f _lineEnd) {
		return v2(glm::closestPointOnLine<float>(v2(_point), v2(_lineStart), v2(_lineEnd)));
	}

	static float distance(sf::Vector2f _vec1, sf::Vector2f _vec2) {
		return glm::distance(v2(_vec1), v2(_vec2));
	}

	static float random() {
		return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	}
}

struct PrintString {
	sf::Vector2i m_pos;
	std::string m_string;
	int m_col;

	PrintString(sf::Vector2i _pos, std::string _message, int _col) : m_pos(_pos), m_string(_message), m_col(_col){};
	PrintString(int x, int y, std::string _message, int _col) : m_pos(sf::Vector2i(x, y)), m_string(_message), m_col(_col) {};
};