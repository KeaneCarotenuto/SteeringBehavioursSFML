#pragma once
#include "Utility.h"

struct Target {
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	float m_slowingDistance;
};

struct Obstacle {
	Obstacle(std::string _name, sf::Vector2f _pos, float _rad) {
		m_name = _name;
		m_position = _pos;
		m_radius = _rad;
	}

	std::string m_name;
	sf::Vector2f m_position;
	float m_radius;
};

class Agent
{
private:
	std::string m_name;

	float mass;

	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	sf::Vector2f m_acceleration;

	Target m_target;

	float m_mass = 1;

	float m_maxVelocity = 30.0f;
	float m_maxAcceleration = 10.0f;

	float m_wanderAngle = 0;
	float m_wanderAngleChange = 10;


	sf::Sprite sprite;
	sf::Texture* texture;
	

public:
	//Static class things, uses for management
	static std::map<std::string, Agent*> allAgents;
	static std::map<std::string, Obstacle*> allObstacles;

	static sf::RenderWindow* mainWindow;

	static void AddAgent(Agent* _agent);
	static void AddObstacle(Obstacle* _obstacle);

	static void UpdateAll(float deltaTime);
	static void RenderAll();



	Agent(std::string _name, sf::Vector2f _pos, float _mass, float _maxVel, float _maxAcc, sf::Texture* _texture);

	void SetTarget(Target _target) {
		m_target = _target;
	}

	std::string GetName() {
		return m_name;
	}

	sf::Vector2f GetPos() {
		return m_position;
	}
	sf::Vector2f GetVel() {
		return m_velocity;
	}

	void SetMaxVel(float _vel) {
		m_maxVelocity = _vel;
	}

	void SetMaxAcc(float _acc) {
		m_maxAcceleration = _acc;
	}

	sf::Vector2f Seek();

	sf::Vector2f Pursue();

	sf::Vector2f Flee();

	sf::Vector2f Evade();

	sf::Vector2f Arrive();

	sf::Vector2f Wander();

	sf::Vector2f ColAvoid();

	void Update(float deltaTime);

	void Render();
};

