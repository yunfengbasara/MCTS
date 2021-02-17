#include "MCTS.h"
#include <list>
#include <tuple> 
#include <time.h>
using namespace ChineseChess;

float g_fConfident = 1.0f / 1.41f ;

Node::Node(Node* pParent)
	: m_pParent(pParent)
	, m_nWinTimes(0)
	, m_nTotalTimes(0)
	, m_fUCB(0.0f)
{
}

Node::~Node()
{
	for (auto pNode : m_lChildren) {
		delete pNode;
		pNode = nullptr;
	}
}

float Node::WinRate()
{
	if (m_nTotalTimes == 0) {
		return 0.0f;
	}
	
	return (float)m_nWinTimes / m_nTotalTimes;
}

float Node::UCB()
{
	return m_fUCB;
}

bool Node::IsSimulate()
{
	return m_nTotalTimes == 0 ? false : true;
}

bool Node::IsFinish()
{
	return m_nAction.m_nFinishType == FinishType::Unknown ? false : true;
}

bool Node::IsExpand()
{
	return m_lChildren.empty() ? false : true;
}

void Node::Update(int win, int total)
{
	m_nWinTimes += win;
	m_nTotalTimes += total;

	if (m_pParent == nullptr) {
		return;
	}

	float fWinRate = (float)m_nWinTimes / m_nTotalTimes;
	float parentSim = log((float)m_pParent->m_nTotalTimes);
	float lambda = sqrt(parentSim / m_nTotalTimes);
	m_fUCB = fWinRate + lambda * g_fConfident;
}

std::vector<Node*>& Node::GetChildren()
{
	return m_lChildren;
}

Action& Node::GetAction()
{
	return m_nAction;
}

Node* Node::GetParent()
{
	return m_pParent;
}

MCTS::MCTS()
{
	m_pRoot = new Node();
	m_pRoot->GetAction().m_nTurnType = SideType::Black;
	srand(unsigned(time(NULL)));
}

MCTS::~MCTS()
{
	delete m_pRoot;
	m_pRoot = nullptr;
}

void MCTS::Init()
{
	Position RedPosGeneral(4, 0);
	m_nBoard.AddGeneral(RedPosGeneral, SideType::Red);

	Position RedPosElephant1(4, 2);
	m_nBoard.AddMan(RedPosElephant1, ManType::Elephant, SideType::Red);

	Position RedPosElephant2(2, 0);
	m_nBoard.AddMan(RedPosElephant2, ManType::Elephant, SideType::Red);

	Position RedPosVehicle(6, 4);
	m_nBoard.AddMan(RedPosVehicle, ManType::Vehicle, SideType::Red);
	
	Position RedPosHorse(3, 7);
	m_nBoard.AddMan(RedPosHorse, ManType::Horse, SideType::Red);

	Position RedPosSoldier(5, 6);
	m_nBoard.AddMan(RedPosSoldier, ManType::Soldier, SideType::Red);
	
	Position BlackPosGeneral(5, 8);
	m_nBoard.AddGeneral(BlackPosGeneral, SideType::Black);

	Position BlackPosGuarder(3, 9);
	m_nBoard.AddMan(BlackPosGuarder, ManType::Guarder, SideType::Black);

	Position BlackPosElephant1(2, 9);
	m_nBoard.AddMan(BlackPosElephant1, ManType::Elephant, SideType::Black);

	Position BlackPosElephant2(4, 7);
	m_nBoard.AddMan(BlackPosElephant2, ManType::Elephant, SideType::Black);

	Position BlackPosVehicle(1, 3);
	m_nBoard.AddMan(BlackPosVehicle, ManType::Vehicle, SideType::Black);

	Position BlackPosHorse(2, 2);
	m_nBoard.AddMan(BlackPosHorse, ManType::Horse, SideType::Black);

	Position BlackPosSoldier(3, 1);
	m_nBoard.AddMan(BlackPosSoldier, ManType::Soldier, SideType::Black);
}

Node* MCTS::GetRoot()
{
	return m_pRoot;
}

Node* MCTS::Selection(Node* pCurrent)
{
	if (pCurrent == nullptr) {
		pCurrent = m_pRoot;
	}

	auto IsNormalNode = [](Node* pNode) {
		// 没有模拟的节点
		if (!pNode->IsSimulate()) {
			return false;
		}

		// 结束的节点
		if (pNode->IsFinish()) {
			return false;
		}

		// 没有扩展的节点
		if (!pNode->IsExpand()) {
			return false;
		}

		// 普通节点
		return true;
	};

	// 找到最佳节点
	while (IsNormalNode(pCurrent)) {
		float bestUCB = -0.1f;
		auto& children = pCurrent->GetChildren();
		for (auto& pNode : children) {
			if (!IsNormalNode(pNode)) {
				pCurrent = pNode;
				break;
			}
			if (pNode->UCB() > bestUCB) {
				pCurrent = pNode;
				bestUCB = pCurrent->UCB();
			}
		}
	}

	return pCurrent;
}

Node* MCTS::Expand(Node* pCurrent)
{
	if (pCurrent->IsFinish() ||
		pCurrent->IsExpand()) {
		return pCurrent;
	}

	std::list<Action> acLst;
	Node* pTemp = pCurrent;
	while (pTemp != m_pRoot) {
		acLst.push_front(pTemp->GetAction());
		pTemp = pTemp->GetParent();
	}

	Board tempBoard = m_nBoard;
	for (auto& ac : acLst) {
		tempBoard.MoveMan(ac.m_nOriPos, ac.m_nDstPos);
	}

	SideType lastType = pCurrent->GetAction().m_nTurnType;
	SideType nowTurn = lastType == SideType::Red ? SideType::Black : SideType::Red;

	auto posTuple = tempBoard.GetLegalPosition(nowTurn);
	for (auto& ps : posTuple) {
		Action ac(nowTurn);
		ac.m_nOriPos = std::get<0>(ps);
		ac.m_nDstPos = std::get<1>(ps);
		ac.m_nFinishType = std::get<2>(ps);

		Node* pNode = new Node(pCurrent);
		pNode->GetAction() = ac;
		pCurrent->GetChildren().push_back(pNode);
	}

	if (pCurrent->IsExpand()) {
		pCurrent = pCurrent->GetChildren()[0];
	}
	return pCurrent;
}

FinishType MCTS::Simulate(Node* pCurrent)
{
	if (pCurrent->IsFinish()) {
		return pCurrent->GetAction().m_nFinishType;
	}

	std::list<Action> acLst;
	Node* pTemp = pCurrent;
	while (pTemp != m_pRoot) {
		acLst.push_front(pTemp->GetAction());
		pTemp = pTemp->GetParent();
	}

	Board tempBoard = m_nBoard;
	for (auto& ac : acLst) {
		tempBoard.MoveMan(ac.m_nOriPos, ac.m_nDstPos);
	}

	SideType lastType = pCurrent->GetAction().m_nTurnType;
	SideType nowTurn = lastType == SideType::Red ? SideType::Black : SideType::Red;

	FinishType finish = tempBoard.CheckFinishType();
	while (finish == FinishType::Unknown) {
		std::pair<Position, Position> killGeneralPos;
		if (tempBoard.CheckKillGeneral(nowTurn, killGeneralPos)) {
			tempBoard.MoveMan(killGeneralPos.first, killGeneralPos.second);
			finish = tempBoard.CheckFinishType();
			break;
		}

		std::pair<Position, Position> randomPos;
		if (!tempBoard.GetRandomAction(nowTurn, randomPos)) {
			break;
		}		
		tempBoard.MoveMan(randomPos.first, randomPos.second);
		nowTurn = nowTurn == SideType::Red ? SideType::Black : SideType::Red;
		finish = tempBoard.CheckFinishType();
	}

	return finish;
}

void MCTS::Backpropagation(Node* pCurrent, FinishType finish)
{
	while (pCurrent != nullptr) {
		if (finish == FinishType::RedVictory && 
			pCurrent->GetAction().m_nTurnType == SideType::Red) {
			pCurrent->Update(1, 1);
		}
		else if (finish == FinishType::BlackVictory &&
			pCurrent->GetAction().m_nTurnType == SideType::Black) {
			pCurrent->Update(1, 1);
		}
		else if (finish == FinishType::RedVictory &&
			pCurrent->GetAction().m_nTurnType == SideType::Black) {
			pCurrent->Update(0, 1);
		}
		else if (finish == FinishType::BlackVictory &&
			pCurrent->GetAction().m_nTurnType == SideType::Red) {
			pCurrent->Update(0, 1);
		}
		pCurrent = pCurrent->GetParent();
	}
}

Node* MCTS::GetBestNode(Node * pCurrent)
{
	if (pCurrent == nullptr) {
		pCurrent = m_pRoot;
	}

	if (!pCurrent->IsExpand()) {
		return pCurrent;
	}

	float bestWin = -0.1f;
	auto& children = pCurrent->GetChildren();

	for (auto& pNode : children) {
		if (pNode->WinRate() > bestWin) {
			pCurrent = pNode;
			bestWin = pCurrent->WinRate();
		}
	}

	return pCurrent;
}
