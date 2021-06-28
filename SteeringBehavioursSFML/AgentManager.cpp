//#include "AgentManager.h"
//
//std::map<std::string, Agent*> AgentManager::allAgents;
//
//void AgentManager::AddAgent(Agent* _agent)
//{
//	allAgents[_agent->GetName()] = _agent;
//}
//
//void AgentManager::UpdateAll(float deltaTime)
//{
//	std::map<std::string, Agent*>::iterator it;
//	for (it = AgentManager::allAgents.begin(); it != AgentManager::allAgents.end(); ++it) {
//		it->second->Update(deltaTime);
//	}
//}
//
//void AgentManager::RenderAll()
//{
//	std::map<std::string, Agent*>::iterator it;
//	for (it = AgentManager::allAgents.begin(); it != AgentManager::allAgents.end(); ++it) {
//		it->second->Render();
//	}
//}
