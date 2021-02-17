#include <iostream>

#include "ChineseChess.h"
#include "MCTS.h"

int main()
{
	MCTS mcts;
	mcts.Init();
	for (int trytime = 0; trytime < 1000; trytime++) {
		Node* pNode = mcts.GetRoot();
		pNode = mcts.Selection(pNode);
		pNode = mcts.Expand(pNode);
		ChineseChess::FinishType finish = mcts.Simulate(pNode);
		mcts.Backpropagation(pNode, finish);
	}

	Node* pRoot = mcts.GetRoot();
	Node* pBest = mcts.GetBestNode(pRoot);
	std::cout << "Hello World!\n";
}