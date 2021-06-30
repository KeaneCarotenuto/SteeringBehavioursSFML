#pragma once
#include "Utility.h"

struct Target {
	Target(std::string _name, sf::Vector2f _pos, sf::Vector2f _vel, float _slowingDist) {
		m_name = _name;
		m_position = _pos;
		m_velocity = _vel;
		m_slowingDistance = _slowingDist;
	}

	Target(std::string _name, sf::Vector2f _pos, sf::Vector2f _vel, float _slowingDist, Target* _prev, Target* _next) {
		m_name = _name;
		m_position = _pos;
		m_velocity = _vel;
		m_slowingDistance = _slowingDist;
	}

	std::string m_name = "";

	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	float m_slowingDistance;

	Target* prevTarget = nullptr;
	Target* nextTarget = nullptr;
};

struct Path {
	Path(std::string _name, bool _doesLoop = true) {
		m_name = _name;
		m_loops = _doesLoop;
	}

	std::string m_name = "";

	Target* startTarget = nullptr;
	Target* endTarget = nullptr;

	Target* GetClosestTarget(sf::Vector2f _point);

	bool m_loops = true;

	static std::map<std::string, Target*> allTargets;

	static void AddToPath(Path* _path, sf::Vector2f _pos, float _slowingDist);
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

	Target* m_target = nullptr;

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
	static std::map<std::string, Target*> allTargets;
	static std::map<std::string, Path*> allPaths;

	static sf::RenderWindow* mainWindow;

	static void AddAgent(Agent* _agent);
	static void AddObstacle(Obstacle* _obstacle);
	static void AddTarget(Target* _target);
	static void AddPath(Path* _path);

	static void UpdateAll(float deltaTime);
	static void RenderAll();

	static std::vector<Agent*> GetAllInRad(sf::Vector2f _pos, float _rad);


	Agent(std::string _name, sf::Vector2f _pos, float _mass, float _maxVel, float _maxAcc, sf::Texture* _texture);

	Target* GetTarget() {
		return m_target;
	}
	void SetTarget(Target* _target) {
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

	sf::Vector2f PathFollow();

	sf::Vector2f Pursue();

	sf::Vector2f Flee();

	sf::Vector2f Evade();

	sf::Vector2f Arrive();

	sf::Vector2f Wander();

	sf::Vector2f Align();

	sf::Vector2f Seperate();

	sf::Vector2f Cohesion();

	sf::Vector2f ColAvoid();

	void Collisions();

	void Update(float deltaTime);

	void Render();
};

