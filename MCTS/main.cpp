#include <iostream>

#include "ChineseChess.h"
#include "MCTS.h"

int main()
{
	MCTS mcts;
	mcts.Init();
	for (int trytime = 0; trytime < 4000; trytime++) {
		Node* pNode = mcts.GetRoot();
		pNode = mcts.Selection(pNode);
		pNode = mcts.Expand(pNode);
		ChineseChess::FinishType finish = mcts.Simulate(pNode);
		mcts.Backpropagation(pNode, finish);
	}

	Node* pBest = mcts.GetRoot();
	while (pBest != nullptr) {
		pBest = mcts.GetBestNode(pBest);
		auto ori = pBest->GetAction().m_nOriPos;
		auto dst = pBest->GetAction().m_nDstPos;
		std::cout << "from:" << ori.x << "," << ori.y << std::endl;
		std::cout << "to:" << dst.x << "," << dst.y << std::endl;

		if (pBest->IsFinish()) {
			std::cout << "Game Over Red Win" << std::endl;
			break;
		}

	REINPUT:
		int input;
		std::cin >> input;
		int bx, by, ex, ey;
		ey = input % 10;
		input = input / 10;
		ex = input % 10;
		input = input / 10;
		by = input % 10;
		input = input / 10;
		bx = input % 10;
		Node* pTempBest = mcts.SelectChildNode(pBest, bx, by, ex, ey);
		if (pTempBest == nullptr) {
			std::cout << "Can not find node, please reinput." << std::endl;
			goto REINPUT;
		}

		pBest = pTempBest;
		if (pBest->IsFinish()) {
			std::cout << "Game Over Black Win" << std::endl;
			break;
		}

		for (int trytime = 0; trytime < 1000; trytime++) {
			Node* pNode = mcts.Selection(pBest);
			pNode = mcts.Expand(pNode);
			ChineseChess::FinishType finish = mcts.Simulate(pNode);
			mcts.Backpropagation(pNode, finish);
		}
	}
}