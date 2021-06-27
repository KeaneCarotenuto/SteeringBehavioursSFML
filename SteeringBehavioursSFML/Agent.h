#pragma once
#include "Utility.h"

class Agent
{
private:
	float mass;

	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	sf::Vector2f m_acceleration;

	sf::Vector2f m_targetPos;
	

public:
	void Seek();
};

