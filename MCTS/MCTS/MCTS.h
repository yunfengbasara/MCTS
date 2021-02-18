#pragma once
#include "ChineseChess.h"
#include <vector>
class Node
{
public:
	Node(Node* pParent = nullptr);
	~Node();
	float WinRate();
	float UCB();
	bool IsSimulate();
	bool IsFinish();
	bool IsExpand();
	void Update(int win, int total);
	std::vector<Node*>&	GetChildren();
	ChineseChess::Action& GetAction();
	Node*	GetParent();

private:
	int		m_nWinTimes;
	int		m_nTotalTimes;
	float	m_fUCB;
	Node*	m_pParent;
	std::vector<Node*> m_lChildren;
	ChineseChess::Action m_nAction;
};

class MCTS
{
public:
	MCTS();
	~MCTS();

	void Init();
	Node* GetRoot();
	Node* Selection(Node* pCurrent = nullptr);
	Node* Expand(Node*);
	ChineseChess::FinishType Simulate(Node*);
	void Backpropagation(Node*, ChineseChess::FinishType);

	Node* GetBestNode(Node* pCurrent = nullptr);
	Node* SelectChildNode(Node* pCurrent, int bx, int by, int ex, int ey);

private:
	ChineseChess::Board	m_nBoard;
	Node*				m_pRoot;
};