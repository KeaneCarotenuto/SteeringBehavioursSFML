#include "Agent.h"
std::map<std::string, Agent*> Agent::allAgents;
std::map<std::string, Obstacle*> Agent::allObstacles;
std::map<std::string, Target*> Agent::allTargets;
std::map<std::string, Path*> Agent::allPaths;

std::map<std::string, Target*> Path::allTargets;

sf::RenderWindow* Agent::mainWindow;

void Agent::AddAgent(Agent* _agent)
{
	allAgents[_agent->GetName()] = _agent;
}

void Agent::AddObstacle(Obstacle* _obstacle)
{
	allObstacles[_obstacle->m_name] = _obstacle;
}

void Agent::AddTarget(Target* _target)
{
	allTargets[_target->m_name] = _target;
}

void Agent::AddPath(Path* _path)
{
	allPaths[_path->m_name] = _path;
}

Target* Path::GetClosestTarget(sf::Vector2f _point)
{
	float d = HUGE_VALF;
	Target* _closest = nullptr;

	std::map<std::string, Target*>::iterator it;
	for (it = allTargets.begin(); it != allTargets.end(); ++it) {
		float newD = util::distance(_point, it->second->m_position);
		if (newD < d) {
			_closest = it->second;
			d = newD;
		}
	}

	return _closest;
}

void Path::AddToPath(Path* _path, sf::Vector2f _pos, float _slowingDist)
{
	Target* t = new Target("t" + std::to_string(Agent::allTargets.size()), _pos, sf::Vector2f(0, 0), 0, (_path->endTarget != nullptr ? _path->endTarget : nullptr) , (_path->endTarget != nullptr ? _path->endTarget->nextTarget : nullptr));
	Agent::AddTarget(t);
	allTargets[t->m_name] = t;
	
	if (_path->startTarget == nullptr) {
		_path->startTarget = t;
	}

	if (_path->endTarget != nullptr) {
		_path->endTarget->nextTarget = t;
		t->prevTarget = _path->endTarget;
	}

	_path->endTarget = t;

	if (_path->m_loops) {
		_path->startTarget->prevTarget = _path->endTarget;
		_path->endTarget->nextTarget = _path->startTarget;
	}

	
	
}

void Agent::UpdateAll(float deltaTime)
{
	std::map<std::string, Agent*>::iterator it;
	for (it = Agent::allAgents.begin(); it != Agent::allAgents.end(); ++it) {
		it->second->Update(deltaTime);
	}
}

void Agent::RenderAll()
{
	std::map<std::string, Agent*>::iterator itAgents;
	for (itAgents = Agent::allAgents.begin(); itAgents != Agent::allAgents.end(); ++itAgents) {
		itAgents->second->Render();
	}


	std::map<std::string, Obstacle*>::iterator itObs;
	for (itObs = Agent::allObstacles.begin(); itObs != Agent::allObstacles.end(); ++itObs) {
		sf::CircleShape shape(itObs->second->m_radius);
		shape.setOrigin(itObs->second->m_radius, itObs->second->m_radius);
		shape.setPosition(itObs->second->m_position);
		shape.setFillColor(sf::Color(255, 163, 72));

		mainWindow->draw(shape);
	}

	if (util::debugMode) {
		std::map<std::string, Target*>::iterator itTarg;
		for (itTarg = Agent::allTargets.begin(); itTarg != Agent::allTargets.end(); ++itTarg) {
			float rad = std::max(itTarg->second->m_slowingDistance, 20.0f);

			sf::CircleShape shape(rad);
			shape.setOrigin(rad, rad);
			shape.setPosition(itTarg->second->m_position);
			shape.setFillColor(sf::Color(53, 132, 228, 100));

			mainWindow->draw(shape);

			if (itTarg->second->nextTarget) {
				sf::VertexArray lines(sf::LinesStrip, 2);
				lines[0].position = itTarg->second->m_position;
				lines[0].color = sf::Color::Red;
				lines[1].position = itTarg->second->nextTarget->m_position;
				lines[1].color = sf::Color::Green;
				mainWindow->draw(lines);
			}
		}
	}
}

std::vector<Agent*> Agent::GetAllInRad(sf::Vector2f _pos, float _rad)
{
	std::vector<Agent*> tempVec;

	std::map<std::string, Agent*>::iterator it;
	for (it = Agent::allAgents.begin(); it != Agent::allAgents.end(); ++it) {
		if (util::distance(_pos, it->second->GetPos()) <= _rad) {
			tempVec.push_back(it->second);
		}
	}

	return tempVec;
}

Agent::Agent(std::string _name, sf::Vector2f _pos, float _mass, float _maxVel, float _maxAcc, sf::Texture* _texture)
{
	m_name = _name;
	m_position = _pos;
	m_mass = _mass;
	m_maxVelocity = _maxVel;
	m_maxAcceleration = _maxAcc;

	texture = _texture;
	sprite.setTexture(*texture);
	sprite.setOrigin(16, 16);
}

sf::Vector2f Agent::Seek()
{
	sf::Vector2f desiredVel = util::normalize(m_target->m_position - m_position) * m_maxAcceleration;

	desiredVel = desiredVel - m_velocity;

	return desiredVel;
}

sf::Vector2f Agent::PathFollow()
{
	if (m_target) {
		if (m_target->nextTarget != nullptr) {
			float rad = std::max(m_target->m_slowingDistance, 20.0f);


			if (util::distance(m_target->m_position, m_position) < rad) {
				m_target = m_target->nextTarget;
			}
		}

		sf::Vector2f desiredVel = util::normalize(m_target->m_position - m_position) * m_maxAcceleration;

		desiredVel = desiredVel - m_velocity;

		return desiredVel;
	}

	return sf::Vector2f(0, 0);
	
}

sf::Vector2f Agent::Pursue()
{
	sf::Vector2f desiredVel = util::normalize(m_target->m_position - m_position + m_target->m_velocity) * m_maxAcceleration;

	desiredVel = desiredVel - m_velocity;

	return desiredVel;
}

sf::Vector2f Agent::Flee()
{
	sf::Vector2f desiredVel = util::normalize(m_position - m_target->m_position) * m_maxAcceleration;

	desiredVel = desiredVel - m_velocity;

	return desiredVel;
}

sf::Vector2f Agent::Evade()
{
	sf::Vector2f desiredVel = util::normalize(m_position - m_target->m_position - m_target->m_velocity) * m_maxAcceleration;

	desiredVel = desiredVel - m_velocity;

	return desiredVel;
}

sf::Vector2f Agent::Arrive()
{
	sf::Vector2f offset = m_target->m_position - m_position;

	float distance = util::length(offset);

	float rampedSpeed = m_maxVelocity * (distance / m_target->m_slowingDistance);

	float clampedSpeed = std::min(rampedSpeed, m_maxVelocity);

	sf::Vector2f desiredVel = (clampedSpeed / distance) * offset;

	desiredVel = desiredVel - m_velocity;

	return desiredVel;
}

sf::Vector2f Agent::Wander()
{
	float _circleDistance = 100;
	float _circleRad = 100;

	sf::Vector2f circleCenter = util::normalize(m_velocity) * _circleDistance;

	sf::Vector2f displacementForce = sf::Vector2f(0, 1) * _circleRad;

	m_wanderAngle += (util::random() * 2 - 1) * m_wanderAngleChange;

	displacementForce = util::rotate(displacementForce, m_wanderAngle);

	return displacementForce;
}

sf::Vector2f Agent::Align()
{
	std::vector<Agent*> nearby = GetAllInRad(m_position, 200);

	sf::Vector2f averageVel = sf::Vector2f(0, 0);

	for (Agent* _agent : nearby) {
		if (_agent == this) continue;
		averageVel += _agent->GetVel();
	}

	if (util::length(averageVel) <= 0.1f) {
		return sf::Vector2f(0, 0);
	}

	averageVel *= 1.0f / (float)(nearby.size());

	sf::Vector2f desiredVel = util::normalize(averageVel) * m_maxAcceleration;

	desiredVel = desiredVel - m_velocity;

	return desiredVel;
}

sf::Vector2f Agent::Seperate()
{
	std::vector<Agent*> nearby = GetAllInRad(m_position, 100);

	sf::Vector2f averagePos = sf::Vector2f(0, 0);

	for (Agent* _agent : nearby) {
		if (_agent == this) continue;
		averagePos += _agent->GetPos();
	}

	if (util::length(averagePos) <= 0.1f) {
		return sf::Vector2f(0, 0);
	}

	averagePos *= 1.0f / (float)(nearby.size() - 1);

	float distToAPos = util::distance(averagePos, m_position);

	sf::Vector2f desiredVel = util::normalize(m_position - averagePos) * m_maxAcceleration / (distToAPos/100);

	desiredVel = desiredVel - m_velocity;

	return desiredVel;
}

sf::Vector2f Agent::Cohesion()
{
	std::vector<Agent*> nearby = GetAllInRad(m_position, 100);

	sf::Vector2f averagePos = sf::Vector2f(0, 0);

	std::map<std::string, int> possibleTargets;

	for (Agent* _agent : nearby) {
		if (_agent == this) continue;

		if (_agent->m_target) possibleTargets[_agent->m_target->m_name]++;
		averagePos += _agent->GetPos();
	}

	if (util::length(averagePos) <= 0.1f) {
		return sf::Vector2f(0, 0);
	}

	std::pair<std::string, int> _mostPop = {"",0};

	std::map<std::string, int>::iterator it;
	for (it = possibleTargets.begin(); it != possibleTargets.end(); ++it) {
		if (it->second > _mostPop.second) {
			_mostPop = *it;
		}
	}

	if (_mostPop.first != "" && _mostPop.second != NULL && m_target && m_target->nextTarget && m_target->nextTarget->m_name == _mostPop.first) {
		m_target = Agent::allTargets[_mostPop.first];
	}
	

	averagePos *= 1.0f / (float)(nearby.size() - 1);

	float distToAPos = util::distance(averagePos, m_position);

	sf::Vector2f desiredVel = util::normalize(averagePos - m_position) * m_maxAcceleration * (distToAPos / 10);

	desiredVel = desiredVel - m_velocity;

	return desiredVel;
}

sf::Vector2f Agent::ColAvoid()
{
	sf::Vector2f laterallForce;
	sf::Vector2f brakingForce;

	std::map<std::string, Obstacle*>::iterator it;
	for (it = Agent::allObstacles.begin(); it != Agent::allObstacles.end(); ++it) {
		float distToOb = util::distance(m_position, it->second->m_position);
		float distToEdge = util::distance(m_position, it->second->m_position) - it->second->m_radius;

		if (distToOb <= (util::length(m_velocity) + it->second->m_radius)) {
			sf::Vector2f closestPoint = util::closestPoint(it->second->m_position, m_position, m_position + m_velocity*2.0f);

			float distToClosestPoint = util::distance(closestPoint, it->second->m_position);
			if (distToClosestPoint < it->second->m_radius + sprite.getGlobalBounds().width/2.0f) {
				laterallForce =  util::normalize(closestPoint - it->second->m_position) * (1.0f / distToEdge) * it->second->m_radius * 1000.0f;
				brakingForce = -util::normalize(m_velocity) * (1.0f/ distToEdge) * it->second->m_radius * 100.0f;
			}
		}
	}

	return (laterallForce + brakingForce);
}

void Agent::Collisions() {
	std::map<std::string, Obstacle*>::iterator it;
	for (it = Agent::allObstacles.begin(); it != Agent::allObstacles.end(); ++it) {
		sf::Vector2f outwardsVec = util::normalize(m_position - it->second->m_position) * it->second->m_radius;

		float distance = util::distance(m_position, it->second->m_position);

		if (distance <= it->second->m_radius) {
			if (util::angle(m_velocity, outwardsVec) > 90) {
				m_position = it->second->m_position + outwardsVec;
				m_velocity = util::reflect(m_velocity, util::normalize(outwardsVec));
			}
		}
	}
}

void Agent::Update(float deltaTime)
{
	//std::vector<Agent*> nearby = GetAllInRad(m_position, 400);

	if (m_name == "a0") m_acceleration = Seek();
	else {
		m_acceleration = PathFollow() * 2.0f;
		//m_acceleration += Wander();
		m_acceleration += Seperate();
		m_acceleration += Cohesion();
		m_acceleration += Align();
	}

	m_acceleration += ColAvoid();

	

	if (util::length(m_acceleration) > m_maxAcceleration)
	{
		m_acceleration = util::normalize(m_acceleration);
		m_acceleration *= m_maxAcceleration;
	}

	m_velocity += m_acceleration * deltaTime;

	if (util::length(m_velocity) > m_maxVelocity)
	{
		m_velocity = util::normalize(m_velocity);
		m_velocity *= m_maxVelocity;
	}

	Collisions();

	m_position += m_velocity * deltaTime;

	if (m_position.x > util::windowWidth) m_position.x = 0;
	if (m_position.x < 0) m_position.x = util::windowWidth;
	if (m_position.y > util::windowHeight) m_position.y = 0;
	if (m_position.y < 0) m_position.y = util::windowHeight;

	m_velocity *= 0.999f;
}

void Agent::Render()
{
	if (util::debugMode) {
		sf::VertexArray lines(sf::LinesStrip, 2);
		lines[0].position = m_position;
		lines[0].color = sf::Color::Red;
		lines[1].position = m_position + m_velocity;
		lines[1].color = sf::Color::Red;
		mainWindow->draw(lines);

		lines[0].position = m_position;
		lines[0].color = sf::Color::Blue;
		lines[1].position = m_position + m_acceleration;
		lines[1].color = sf::Color::Blue;
		mainWindow->draw(lines);
	}
	

	sprite.setPosition(m_position);
	sprite.setRotation(-atan2(m_velocity.x, m_velocity.y) * 180.0f / glm::pi<float>() + 180);
	mainWindow->draw(sprite);
}