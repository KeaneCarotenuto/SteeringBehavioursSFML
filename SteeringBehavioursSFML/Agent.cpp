#include "Agent.h"
std::map<std::string, Agent*> Agent::allAgents;
std::map<std::string, Obstacle*> Agent::allObstacles;
std::map<std::string, Target*> Agent::allTargets;
std::map<std::string, Path*> Agent::allPaths;

std::map<std::string, Target*> Path::allTargets;

sf::RenderWindow* Agent::mainWindow;

SteeringType Agent::behaviour = SteeringType::Seek;

/// <summary>
/// Add agent to list
/// </summary>
/// <param name="_agent"></param>
void Agent::AddAgent(Agent* _agent)
{
	allAgents[_agent->GetName()] = _agent;
}

/// <summary>
/// Add obstacle to list
/// </summary>
/// <param name="_obstacle"></param>
void Agent::AddObstacle(Obstacle* _obstacle)
{
	allObstacles[_obstacle->m_name] = _obstacle;
}

/// <summary>
/// Add target to list
/// </summary>
/// <param name="_target"></param>
void Agent::AddTarget(Target* _target)
{
	allTargets[_target->m_name] = _target;
}

/// <summary>
/// Add path to list
/// </summary>
/// <param name="_path"></param>
void Agent::AddPath(Path* _path)
{
	allPaths[_path->m_name] = _path;
}

/// <summary>
/// Returns closest target to supplied point
/// </summary>
/// <param name="_point"></param>
/// <returns></returns>
Target* Path::GetClosestTarget(sf::Vector2f _point)
{
	//store best option
	float d = HUGE_VALF;
	Target* _closest = nullptr;

	//loop through all options and find best
	std::map<std::string, Target*>::iterator it;
	for (it = allTargets.begin(); it != allTargets.end(); ++it) {
		float newD = util::distance(_point, it->second->m_position);
		if (newD < d) {
			_closest = it->second;
			d = newD;
		}
	}

	//return best 
	return _closest;
}

/// <summary>
/// Add point to list of paths
/// </summary>
/// <param name="_path"></param>
/// <param name="_pos"></param>
/// <param name="_slowingDist"></param>
void Path::AddToPath(Path* _path, sf::Vector2f _pos, float _slowingDist)
{
	//Make new target
	Target* t = new Target("t" + std::to_string(Agent::allTargets.size()), _pos, sf::Vector2f(0, 0), _slowingDist, (_path->endTarget != nullptr ? _path->endTarget : nullptr) , (_path->endTarget != nullptr ? _path->endTarget->nextTarget : nullptr));
	Agent::AddTarget(t);
	allTargets[t->m_name] = t;
	
	//Update current path targets (next, prev, start, end)
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

void Agent::UpdateBehaviour(void _textUpdate(std::string))
{
	std::map<std::string, Agent*>::iterator it;
	for (it = Agent::allAgents.begin(); it != Agent::allAgents.end(); ++it) {
		if (it->second->GetName() == "a0") continue;

		switch (Agent::behaviour)
		{
		case SteeringType::Seek: {
			it->second->SetTarget(Agent::allTargets["a0"]);
			_textUpdate("SEEK");
			break;
		}
		case SteeringType::Flee: {
			it->second->SetTarget(Agent::allTargets["a0"]);
			_textUpdate("FLEE");
			break;
		}
		case SteeringType::Pursue: {
			it->second->SetTarget(Agent::allTargets["a0"]);
			_textUpdate("PURSUE");
			break;
		}
		case SteeringType::Evade: {
			it->second->SetTarget(Agent::allTargets["a0"]);
			_textUpdate("EVADE");
			break;
		}
		case SteeringType::Wander: {
			it->second->SetTarget(nullptr);
			_textUpdate("WANDER");
			break;
		}
		case SteeringType::Arrival: {
			Agent::allTargets["a0"]->m_slowingDistance = 100.0f;
			it->second->SetTarget(Agent::allTargets["a0"]);
			_textUpdate("ARRIVE");
			break;
		}
		case SteeringType::PathFollowing: {
			it->second->SetTarget(Agent::allPaths["p0"]->GetClosestTarget(it->second->GetPos()));
			_textUpdate("PATH FOLLOW");
			break;
		}
		case SteeringType::Flocking: {
			it->second->SetTarget(nullptr);
			_textUpdate("FLOCK - WANDER");
			break;
		}
		case SteeringType::CrowdPathFollowing: {
			it->second->SetTarget(Agent::allPaths["p0"]->GetClosestTarget(it->second->GetPos()));
			_textUpdate("CROWD PATH FOLLOW");
			break;
		}
		default:
			break;
		}

	}
}

/// <summary>
/// Update all agents
/// </summary>
/// <param name="deltaTime"></param>
void Agent::UpdateAll(float deltaTime)
{
	std::map<std::string, Agent*>::iterator it;
	for (it = Agent::allAgents.begin(); it != Agent::allAgents.end(); ++it) {
		it->second->Update(deltaTime);
	}
}

/// <summary>
/// Render all agents, obstacles, targets, and paths
/// </summary>
void Agent::RenderAll()
{
	std::map<std::string, Agent*>::iterator itAgents;
	for (itAgents = Agent::allAgents.begin(); itAgents != Agent::allAgents.end(); ++itAgents) {
		itAgents->second->Render();

		if (util::debugMode && itAgents->second->m_target) {
			sf::VertexArray lines(sf::LinesStrip, 2);
			lines[0].position = itAgents->second->m_position;
			lines[0].color = sf::Color::Color(0,255,0, 10);
			lines[1].position = itAgents->second->m_target->m_position;
			lines[1].color = sf::Color::Color(0, 255, 0, 10);
			mainWindow->draw(lines);
		}
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

			sf::CircleShape shape(itTarg->second->m_slowingDistance);
			shape.setOrigin(itTarg->second->m_slowingDistance, itTarg->second->m_slowingDistance);
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

void Agent::DeleteAll() {
	std::map<std::string, Agent*>::iterator itAgents;
	for (itAgents = Agent::allAgents.begin(); itAgents != Agent::allAgents.end(); ++itAgents) {
		//delete itAgents->second->m_target;

		itAgents->second->m_target = nullptr;
		itAgents->second->texture = nullptr;

		delete itAgents->second;
	}
	Agent::allAgents.clear();

	std::map<std::string, Obstacle*>::iterator itObs;
	for (itObs = Agent::allObstacles.begin(); itObs != Agent::allObstacles.end(); ++itObs) {
		delete itObs->second;
	}
	Agent::allObstacles.clear();

	std::map<std::string, Target*>::iterator itTarg;
	for (itTarg = Agent::allTargets.begin(); itTarg != Agent::allTargets.end(); ++itTarg) {
		//delete itTarg->second->prevTarget;
		//delete itTarg->second->nextTarget;

		itTarg->second->prevTarget = nullptr;
		itTarg->second->nextTarget = nullptr;

		delete itTarg->second;
	}
	Agent::allTargets.clear();

	std::map<std::string, Path*>::iterator itPath;
	for (itPath = Agent::allPaths.begin(); itPath != Agent::allPaths.end(); ++itPath) {

		//delete itPath->second->startTarget;
		//delete itPath->second->endTarget;

		itPath->second->startTarget = nullptr;
		itPath->second->endTarget = nullptr;

		itPath->second->allTargets.clear();

		delete itPath->second;
	}
	Agent::allPaths.clear();
}

/// <summary>
/// Returns all agents within radius
/// </summary>
/// <param name="_pos"></param>
/// <param name="_rad"></param>
/// <returns></returns>
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

/// <summary>
/// Seek to agent target
/// </summary>
/// <returns></returns>
sf::Vector2f Agent::Seek()
{
	if (!m_target) return sf::Vector2f(0, 0);

	sf::Vector2f desiredVel = util::normalize(m_target->m_position - m_position) * m_maxAcceleration;

	desiredVel = desiredVel - m_velocity;

	return desiredVel;
}

/// <summary>
/// Follow path associated with current target
/// </summary>
/// <returns></returns>
sf::Vector2f Agent::PathFollow()
{
	if (m_target) {
		//If close enough to target, set target to next point
		if (m_target->nextTarget != nullptr) {


			if (util::distance(m_target->m_position, m_position) < m_target->m_slowingDistance) {
				m_target = m_target->nextTarget;
			}
		}

		sf::Vector2f desiredVel = util::normalize(m_target->m_position - m_position) * m_maxAcceleration;

		desiredVel = desiredVel - m_velocity;

		return desiredVel;
	}

	return sf::Vector2f(0, 0);
	
}

/// <summary>
/// Pursue agents target
/// </summary>
/// <returns></returns>
sf::Vector2f Agent::Pursue()
{
	sf::Vector2f desiredVel = util::normalize(m_target->m_position - m_position + m_target->m_velocity) * m_maxAcceleration;

	desiredVel = desiredVel - m_velocity;

	return desiredVel;
}

/// <summary>
/// Flee from agents target
/// </summary>
/// <returns></returns>
sf::Vector2f Agent::Flee()
{
	sf::Vector2f desiredVel = util::normalize(m_position - m_target->m_position) * m_maxAcceleration;

	desiredVel = desiredVel - m_velocity;

	return desiredVel;
}

/// <summary>
/// Evade from agents target
/// </summary>
/// <returns></returns>
sf::Vector2f Agent::Evade()
{
	sf::Vector2f desiredVel = util::normalize(m_position - m_target->m_position - m_target->m_velocity) * m_maxAcceleration;

	desiredVel = desiredVel - m_velocity;

	return desiredVel;
}

/// <summary>
/// Arrive at argents target
/// </summary>
/// <returns></returns>
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

/// <summary>
/// Wander randomly
/// </summary>
/// <returns></returns>
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

/// <summary>
/// Allign with all other agents in radius
/// </summary>
/// <returns></returns>
sf::Vector2f Agent::Align()
{
	//get all agents in rad
	std::vector<Agent*> nearby = GetAllInRad(m_position, 200);

	sf::Vector2f averageVel = sf::Vector2f(0, 0);

	//Get average velocity
	int skipcount = 0;
	for (Agent* _agent : nearby) {
		if (_agent == this || _agent->m_name == "a0") {
			skipcount++;
			continue;
		}

		averageVel += _agent->GetVel();
	}

	if (util::length(averageVel) <= 0.1f) {
		return sf::Vector2f(0, 0);
	}

	averageVel *= 1.0f / (float)(nearby.size() - skipcount);

	//set desired vel to average vel
	sf::Vector2f desiredVel = util::normalize(averageVel) * m_maxAcceleration;

	desiredVel = desiredVel - m_velocity;

	return desiredVel;
}

/// <summary>
/// Seperate from agents within radius
/// </summary>
/// <returns></returns>
sf::Vector2f Agent::Seperate()
{
	//Get all nearby agents within rad
	std::vector<Agent*> nearby = GetAllInRad(m_position, 50);

	//Make average diff var
	sf::Vector2f averageDiff = sf::Vector2f(0, 0);

	//Loop through all agents, except this one, and player controlled one, then add diff to average
	int skipcount = 0;
	for (Agent* _agent : nearby) {
		if (_agent == this || _agent->m_name == "a0") {
			skipcount++;
			continue;
		}

		//Calc distance between agents
		float d = util::distance(m_position, _agent->m_position);

		//Calc diff, divide by distance
		sf::Vector2f diff = util::normalize(m_position - _agent->m_position) / d;

		//add diff
		averageDiff += diff;
	}

	//If dif is near 0, return nothing
	if (util::length(averageDiff) <= 0.0001f) {
		return sf::Vector2f(0, 0);
	}

	//Average out total dif
	averageDiff /= (float)(nearby.size() - skipcount);

	//Calc desired velocity
	sf::Vector2f desiredVel = util::normalize(averageDiff) * m_maxAcceleration;

	desiredVel = desiredVel - m_velocity;

	return desiredVel;
}

/// <summary>
/// Move towards average position of nearby agents
/// </summary>
/// <returns></returns>
sf::Vector2f Agent::Cohesion()
{
	std::vector<Agent*> nearby = GetAllInRad(m_position, 800);

	sf::Vector2f averagePos = sf::Vector2f(0, 0);

	//calc average position
	int skipcount = 0;
	for (Agent* _agent : nearby) {
		if (_agent == this || _agent->m_name == "a0") {
			skipcount++;
			continue;
		}

		averagePos += _agent->GetPos();
	}

	if (util::length(averagePos) <= 0.1f) {
		return sf::Vector2f(0, 0);
	}
	
	averagePos *= 1.0f / (float)(nearby.size() - skipcount);

	//calc distance to average pos
	float distToAPos = util::distance(averagePos, m_position);

	//set desired vel to be towards the average position, stronger from further
	sf::Vector2f desiredVel = util::normalize(averagePos - m_position) * m_maxAcceleration * distToAPos;

	desiredVel = desiredVel - m_velocity;

	return desiredVel;
}

/// <summary>
/// Create flocks that group up, but stay just appart, and align directions
/// </summary>
/// <returns></returns>
sf::Vector2f Agent::Flock() {
	sf::Vector2f desiredVel = sf::Vector2f(0, 0);

	desiredVel += Seperate() * 5.0f;
	desiredVel += Cohesion() * 0.02f;
	desiredVel += Align();

	return desiredVel;
}

/// <summary>
/// Follow a path as a group
/// </summary>
/// <returns></returns>
sf::Vector2f Agent::GroupPathFollow() {
	//find all nearby agents
	std::vector<Agent*> nearby = GetAllInRad(m_position, 1000);

	//empty list of possible targets
	std::map<std::string, int> possibleTargets;

	//average position of all agents
	sf::Vector2f averagePos = sf::Vector2f(0, 0);

	//Tally up all of the different targets that agents are trying to get to
	int skipcount = 0;
	for (Agent* _agent : nearby) {
		if (_agent == this || _agent->m_name == "a0") {
			skipcount++;
			continue;
		}

		averagePos += _agent->m_position;

		if (_agent->m_target) possibleTargets[_agent->m_target->m_name]++;
	}

	//Find the most popular target to get to
	std::pair<std::string, int> _mostPop = { "",0 };

	std::map<std::string, int>::iterator it;
	for (it = possibleTargets.begin(); it != possibleTargets.end(); ++it) {
		if (it->second > _mostPop.second) {
			_mostPop = *it;
		}
	}

	//Average the positions
	averagePos /= (float)(nearby.size() - skipcount);

	if (m_target) m_target->m_slowingDistance = 30 * glm::sqrt(nearby.size() / glm::pi<float>());

	//If majority of agents hit the target, then all other agents should move on
	//Else, If the average position of all agents hits the target, all should move on
	if (_mostPop.first != "" && _mostPop.second != NULL && m_target && m_target->nextTarget && m_target->nextTarget->m_name == _mostPop.first) {
		m_target = Agent::allTargets[_mostPop.first];
	}
	else if (m_target && m_target->nextTarget && util::distance(averagePos, m_target->m_position) <= m_target->m_slowingDistance) {
		m_target = m_target->nextTarget;
	}

	sf::Vector2f desiredVel = sf::Vector2f(0, 0);

	//Follow the path
	desiredVel += PathFollow() * 2.0f;

	//Flock to other agents
	desiredVel += Flock();

	return desiredVel;
}

/// <summary>
/// Attempt to avoid a collision by braking, and steering away
/// </summary>
/// <returns></returns>
sf::Vector2f Agent::ColAvoid()
{
	sf::Vector2f laterallForce;
	sf::Vector2f brakingForce;

	//Loop through all obstacles
	std::map<std::string, Obstacle*>::iterator it;
	for (it = Agent::allObstacles.begin(); it != Agent::allObstacles.end(); ++it) {
		//check distance to object, as well as the distance to the edge of the obstacle
		float distToOb = util::distance(m_position, it->second->m_position);
		float distToEdge = util::distance(m_position, it->second->m_position) - it->second->m_radius;

		//If the distance to obstacle is within magnitude of velocity, check for imminent collision
		if (distToOb <= (util::length(m_velocity) + it->second->m_radius)) {
			//Find the closest point along velocity line, to the center of the circle
			sf::Vector2f closestPoint = util::closestPoint(it->second->m_position, m_position, m_position + m_velocity*2.0f);

			//Calc distance from closest point to the obstacle pos
			float distToClosestPoint = util::distance(closestPoint, it->second->m_position);

			//If distance to closest point is half the width of the agent, attempot to avoid
			if (distToClosestPoint < it->second->m_radius + sprite.getGlobalBounds().width/2.0f) {
				laterallForce =  util::normalize(closestPoint - it->second->m_position) * (1.0f / distToEdge) * it->second->m_radius * 1000.0f;
				brakingForce = -util::normalize(m_velocity) * (1.0f/ distToEdge) * it->second->m_radius * 100.0f;
			}
		}
	}

	return (laterallForce + brakingForce);
}

/// <summary>
/// If touching an obstacle, move the agent to the nearest outside of the obstacle, and reflect thier velocity about the normal of the collision
/// </summary>
void Agent::Collisions() {
	//Loop through all obstacles
	std::map<std::string, Obstacle*>::iterator it;
	for (it = Agent::allObstacles.begin(); it != Agent::allObstacles.end(); ++it) {
		//Calc vec towards agent from obstacle
		sf::Vector2f outwardsVec = util::normalize(m_position - it->second->m_position) * it->second->m_radius;

		//Calc distance from agent to obstacle
		float distance = util::distance(m_position, it->second->m_position);

		//If distance is within radius  of obstacle, do the bounce effect
		if (distance <= it->second->m_radius) {
			if (util::angle(m_velocity, outwardsVec) > 90) {
				//set pos and reflect velocity
				m_position = it->second->m_position + outwardsVec;
				m_velocity = util::reflect(m_velocity, util::normalize(outwardsVec));
			}
		}
	}
}

/// <summary>
/// Update an individual agent
/// </summary>
/// <param name="deltaTime"></param>
void Agent::Update(float deltaTime)
{
	//set acc to 0
	m_acceleration = sf::Vector2f(0, 0);

	//If player controlled, seek mouse, otherwise perform desired behaviour
	if (m_name == "a0") {
		m_acceleration = Seek();
	}
	else {
		switch (behaviour)
		{
		case SteeringType::Seek:
			m_acceleration += Seek();
			break;

		case SteeringType::Flee:
			m_acceleration += Flee();
			break;

		case SteeringType::Pursue:
			m_acceleration += Pursue();
			break;

		case SteeringType::Evade:
			m_acceleration += Evade();
			break;

		case SteeringType::Wander:
			m_acceleration += Wander();
			break;

		case SteeringType::Arrival:
			m_acceleration += Arrive();
			break;

		case SteeringType::PathFollowing:
			m_acceleration += PathFollow();
			break;

		case SteeringType::Flocking:
			m_acceleration += Wander();
			m_acceleration += Flock();
			break;

		case SteeringType::CrowdPathFollowing:
			m_acceleration += GroupPathFollow();
			break;

		default:
			m_acceleration += Wander();
			break;
		}
	}

	//Avoid Collisions
	m_acceleration += ColAvoid();

	//Clamp acceleration
	if (util::length(m_acceleration) > m_maxAcceleration)
	{
		m_acceleration = util::normalize(m_acceleration);
		m_acceleration *= m_maxAcceleration;
	}

	//Apply acceleration
	m_velocity += m_acceleration * deltaTime;


	//Clamp velocity
	if (util::length(m_velocity) > m_maxVelocity)
	{
		m_velocity = util::normalize(m_velocity);
		m_velocity *= m_maxVelocity;
	}

	//Perform collision check before applying vel
	Collisions();

	//Apply velocity
	m_position += m_velocity * deltaTime;

	//Loop around of outside bounding box
	if (m_position.x > util::windowWidth) m_position.x = 0;
	if (m_position.x < 0) m_position.x = util::windowWidth;
	if (m_position.y > util::windowHeight) m_position.y = 0;
	if (m_position.y < 0) m_position.y = util::windowHeight;

	//Apply drag
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