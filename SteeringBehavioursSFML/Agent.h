#pragma once
#include "Utility.h"

class Agent
{
private:
	std::string m_name;

	float mass;

	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	sf::Vector2f m_acceleration;

	sf::Vector2f m_targetPos;

	float m_mass = 1;

	float m_maxVelocity = 30.0f;
	float m_maxAcceleration = 10.0f;


	sf::RenderWindow* window;
	sf::Sprite sprite;
	sf::Texture* texture;
	

public:
	Agent(std::string _name, sf::Vector2f _pos, float _mass, float _maxVel, float _maxAcc, sf::RenderWindow* _window, sf::Texture* _texture);

	void SetTarget(sf::Vector2f _target) {
		m_targetPos = _target;
	}

	std::string GetName() {
		return m_name;
	}


	sf::Vector2f Seek();

	sf::Vector2f Flee();

	void Update(float deltaTime);

	void Render();
};

