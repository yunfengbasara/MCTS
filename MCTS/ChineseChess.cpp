#include "ChineseChess.h"
#include <tuple> 
using namespace ChineseChess;

Position ChineseChess::ReversePosition(const Position& pos)
{
	return Position(pos.x, BOARDHEIGHT - pos.y);
}

void ChineseChess::Interval(int& y1, int& y2, int& s, int& e)
{
	if (y1 > y2) {
		s = y2 + 1;
		e = y1 - 1;
	}
	else {
		s = y1 + 1;
		e = y2 - 1;
	}
}

bool ChineseChess::CheckPosition(const Position& pos)
{
	if (pos.x < 0 ||
		pos.x > BOARDWIDTH ||
		pos.y < 0 ||
		pos.y > BOARDHEIGHT) {
		return false;
	}
	return true;
}

Man::Man(Position pos, ManType type, SideType side)
	: m_nPosition(pos)
	, m_nManType(type)
	, m_nSideType(side)
{
}

Man::~Man()
{
}

Position& Man::GetPosition()
{
	return m_nPosition;
}

ManType& Man::GetManType()
{
	return m_nManType;
}

SideType& Man::GetSideType()
{
	return m_nSideType;
}

std::vector<Position> Man::LegalPosition(Board* pBoard)
{
	std::vector<Position> legalPosition;

	return legalPosition;
}

bool Man::CheckKillGeneral(Board* pBoard)
{
	return false;
}

Board::Board()
	: m_nRedGeneral(nullptr)
	, m_nBlackGeneral(nullptr)
{
	m_nBoard.resize(BOARDWIDTH + 1);
	for (auto& line : m_nBoard) {
		line.resize(BOARDHEIGHT + 1);
	}
}

Board::~Board()
{
	delete m_nRedGeneral;
	delete m_nBlackGeneral;
	for (auto pMan : m_nRedMan) {
		delete pMan;
	}
	for (auto pMan : m_nBlackMan) {
		delete pMan;
	}
}

Board::Board(const Board& board)
{
	m_nBoard.resize(BOARDWIDTH + 1);
	for (auto& line : m_nBoard) {
		line.resize(BOARDHEIGHT + 1);
	}

	AddGeneral(board.m_nRedGeneral->GetPosition(), board.m_nRedGeneral->GetSideType());
	AddGeneral(board.m_nBlackGeneral->GetPosition(), board.m_nBlackGeneral->GetSideType());

	for (auto& pMan : board.m_nRedMan) {
		AddMan(pMan->GetPosition(), pMan->GetManType(), pMan->GetSideType());
	}

	for (auto& pMan : board.m_nBlackMan) {
		AddMan(pMan->GetPosition(), pMan->GetManType(), pMan->GetSideType());
	}
}

Man* Board::CreateMan(Position pos, ManType type, SideType side)
{
	Man* pMan = nullptr;
	switch (type) {
	case ManType::General:	pMan = new General(pos, side);	break;
	case ManType::Guarder:	pMan = new Guarder(pos, side);	break;
	case ManType::Elephant:	pMan = new Elephant(pos, side); break;
	case ManType::Horse:	pMan = new Horse(pos, side);	break;
	case ManType::Vehicle:	pMan = new Vehicle(pos, side);	break;
	case ManType::Gun:		pMan = new Gun(pos, side);		break;
	case ManType::Soldier:	pMan = new Soldier(pos, side);	break;
	}
	return pMan;
}

void Board::AddGeneral(Position pos, SideType side)
{
	Man* pMan = CreateMan(pos, ManType::General, side);

	if (side == SideType::Red) {
		m_nRedGeneral = pMan;
	}
	else {
		m_nBlackGeneral = pMan;
	}

	m_nBoard[pos.x][pos.y] = pMan;
}

void Board::AddMan(Position pos, ManType type, SideType side)
{
	Man* pMan = CreateMan(pos, type, side);

	if (side == SideType::Red) {
		m_nRedMan.push_back(pMan);
	}
	else {
		m_nBlackMan.push_back(pMan);
	}

	m_nBoard[pos.x][pos.y] = pMan;
}

void Board::MoveMan(Position ori, Position dst)
{
	if (ori == dst) {
		return;
	}
	auto pMan = GetMan(ori);
	MoveMan(pMan, dst);
}

Man* Board::GetMan(Position pos)
{
	return m_nBoard[pos.x][pos.y];
}

void Board::MoveMan(Man* pMan, Position newpos)
{
	Position oldpos = pMan->GetPosition();
	m_nBoard[oldpos.x][oldpos.y] = nullptr;

	KillMan(m_nBoard[newpos.x][newpos.y]);

	pMan->GetPosition() = newpos;
	m_nBoard[newpos.x][newpos.y] = pMan;
}

void Board::KillMan(Man* pMan)
{
	if (pMan == nullptr) {
		return;
	}

	if (pMan->GetSideType() == SideType::Red) {
		if (pMan == m_nRedGeneral) {
			delete m_nRedGeneral;
			m_nRedGeneral = nullptr;
			return;
		}

		auto it = find(m_nRedMan.begin(), m_nRedMan.end(), pMan);
		if (it != m_nRedMan.end()) {
			m_nRedMan.erase(it);
		}
		delete pMan;
		return;
	}

	if (pMan->GetSideType() == SideType::Black) {
		if (pMan == m_nBlackGeneral) {
			delete m_nBlackGeneral;
			m_nBlackGeneral = nullptr;
			return;
		}

		auto it = find(m_nBlackMan.begin(), m_nBlackMan.end(), pMan);
		if (it != m_nBlackMan.end()) {
			m_nBlackMan.erase(it);
		}
		delete pMan;
		return;
	}
}

FinishType Board::CheckFinishType()
{
	if (m_nRedGeneral == nullptr) {
		return FinishType::BlackVictory;
	}

	if (m_nBlackGeneral == nullptr) {
		return FinishType::RedVictory;
	}

	return FinishType::Unknown;
}

bool Board::CheckFaceToFace()
{
	int rx = m_nRedGeneral->GetPosition().x;
	int bx = m_nBlackGeneral->GetPosition().x;
	if (rx != bx) {
		return false;
	}

	int ry = m_nRedGeneral->GetPosition().y;
	int by = m_nBlackGeneral->GetPosition().y;

	int s, e;
	Interval(ry, by, s, e);
	for (int y = s; y <= e; y++) {
		if (m_nBoard[rx][y] != nullptr) {
			return false;
		}
	}

	return true;
}

bool Board::LegalSideCheck(SideType& side, Position& pos)
{
	Man* posMan = m_nBoard[pos.x][pos.y];
	if (posMan == nullptr) {
		return true;
	}

	if (posMan->GetSideType() != side) {
		return true;
	}

	return false;
}

bool Board::CheckPosHasMan(Position& pos)
{
	Man* posMan = m_nBoard[pos.x][pos.y];
	if (posMan == nullptr) {
		return false;
	}

	return true;
}

SideType& Board::CheckPosSide(Position& pos)
{
	Man* posMan = m_nBoard[pos.x][pos.y];
	return posMan->GetSideType();
}

std::list<Man*>& Board::RedMan()
{
	return m_nRedMan;
}

std::list<Man*>& Board::BlackMan()
{
	return m_nBlackMan;
}

Man* Board::RedGeneral()
{
	return m_nRedGeneral;
}

Man* Board::BlackGeneral()
{
	return m_nBlackGeneral;
}

std::vector<std::vector<Man*>>& Board::GetBoard()
{
	return m_nBoard;
}

std::vector<std::tuple<Position, Position, FinishType>> Board::GetLegalPosition(SideType sideType)
{
	std::vector<std::tuple<Position, Position, FinishType>> legalPosition;

	if (sideType == SideType::Red) {
		auto generalPosLst = m_nRedGeneral->LegalPosition(this);
		for (auto& pos : generalPosLst) {
			FinishType finish = FinishType::Unknown;
			if (pos == m_nBlackGeneral->GetPosition()) {
				finish = FinishType::RedVictory;
			}
			legalPosition.push_back(std::make_tuple(m_nRedGeneral->GetPosition(), pos, finish));
		}
		
		for (auto& pMan : m_nRedMan) {
			auto manPosLst = pMan->LegalPosition(this);
			for (auto& pos : manPosLst) {
				FinishType finish = FinishType::Unknown;
				if (pos == m_nBlackGeneral->GetPosition()) {
					finish = FinishType::RedVictory;
				}
				legalPosition.push_back(std::make_tuple(pMan->GetPosition(), pos, finish));
			}
		}
	}
	else {
		auto generalPosLst = m_nBlackGeneral->LegalPosition(this);
		for (auto& pos : generalPosLst) {
			FinishType finish = FinishType::Unknown;
			if (pos == m_nRedGeneral->GetPosition()) {
				finish = FinishType::BlackVictory;
			}
			legalPosition.push_back(std::make_tuple(m_nBlackGeneral->GetPosition(), pos, finish));
		}

		for (auto& pMan : m_nBlackMan) {
			auto manPosLst = pMan->LegalPosition(this);
			for (auto& pos : manPosLst) {
				FinishType finish = FinishType::Unknown;
				if (pos == m_nRedGeneral->GetPosition()) {
					finish = FinishType::BlackVictory;
				}
				legalPosition.push_back(std::make_tuple(pMan->GetPosition(), pos, finish));
			}
		}
	}

	return legalPosition;
}

bool Board::GetRandomAction(SideType sideType, std::pair<Position, Position>& posPair)
{
	std::vector<Man*> manLst;
	if (sideType == SideType::Red) {
		manLst.insert(manLst.begin(), m_nRedMan.begin(), m_nRedMan.end());
		manLst.push_back(m_nRedGeneral);
	}
	else {
		manLst.insert(manLst.begin(), m_nBlackMan.begin(), m_nBlackMan.end());
		manLst.push_back(m_nBlackGeneral);
	}

RETRY_RANDOM:
	if (manLst.size() == 0) {
		return false;
	}
	int randomManIndex = rand() % manLst.size();
	Man* pRandomMan = manLst[randomManIndex];
	auto manPosLst = pRandomMan->LegalPosition(this);
	if (manPosLst.size() == 0) {
		manLst.erase(manLst.begin() + randomManIndex);
		goto RETRY_RANDOM;
	}
	int randomPosIndex = rand() % manPosLst.size();
	posPair = std::make_pair(pRandomMan->GetPosition(), manPosLst[randomPosIndex]);
	return true;
}

bool Board::CheckKillGeneral(SideType sideType, std::pair<Position, Position>& posPair)
{
	Position enemyGeneralPos;
	std::vector<Man*> manLst;
	if (sideType == SideType::Red) {
		manLst.insert(manLst.begin(), m_nRedMan.begin(), m_nRedMan.end());
		manLst.push_back(m_nRedGeneral);
		enemyGeneralPos = m_nBlackGeneral->GetPosition();
	}
	else {
		manLst.insert(manLst.begin(), m_nBlackMan.begin(), m_nBlackMan.end());
		manLst.push_back(m_nBlackGeneral);
		enemyGeneralPos = m_nRedGeneral->GetPosition();
	}

	for (auto& pMan : manLst) {
		if (pMan->CheckKillGeneral(this)) {
			posPair = std::make_pair(pMan->GetPosition(), enemyGeneralPos);
			return true;
		}
	}

	return false;
}

General::General(Position pos, SideType side)
	: Man(pos, ManType::General, side)
{
}

General::~General()
{
}

std::vector<Position> General::LegalPosition(Board* pBoard)
{
	std::vector<Position> legalPosition;

	Position& pos = GetPosition();
	SideType& side = GetSideType();

	Position right(pos.x + 1, pos.y);
	if (right.x <= 5 && pBoard->LegalSideCheck(side, right)) {
		legalPosition.push_back(right);
	}

	Position left(pos.x - 1, pos.y);
	if (left.x >= 3 && pBoard->LegalSideCheck(side, left)) {
		legalPosition.push_back(left);
	}

	Position up(pos.x, pos.y + 1);
	if ((side == SideType::Red && up.y <= 2 && pBoard->LegalSideCheck(side, up)) ||
		(side == SideType::Black && up.y <= BOARDHEIGHT && pBoard->LegalSideCheck(side, up))) {
		legalPosition.push_back(up);
	}

	Position down(pos.x, pos.y - 1);
	if ((side == SideType::Red && down.y >= 0 && pBoard->LegalSideCheck(side, down)) ||
		(side == SideType::Black && down.y >= BOARDHEIGHT - 2 && pBoard->LegalSideCheck(side, down))) {
		legalPosition.push_back(down);
	}

	if (pBoard->CheckFaceToFace()) {
		if (side == SideType::Red) {
			legalPosition.push_back(pBoard->BlackGeneral()->GetPosition());
		}
		else {
			legalPosition.push_back(pBoard->RedGeneral()->GetPosition());
		}
	}

	return legalPosition;
}

bool General::CheckKillGeneral(Board* pBoard)
{
	return pBoard->CheckFaceToFace();
}

Guarder::Guarder(Position pos, SideType side)
	: Man(pos, ManType::Guarder, side)
{
}

Guarder::~Guarder()
{
}

std::vector<Position> Guarder::LegalPosition(Board* pBoard)
{
	std::vector<Position> legalPosition;

	Position& pos = GetPosition();
	SideType& side = GetSideType();

	Position lu(pos.x - 1, pos.y + 1);
	if ((side == SideType::Red && lu.x >= 3 && lu.y <= 2 && pBoard->LegalSideCheck(side, lu)) ||
		(side == SideType::Black && lu.x >= 3 && lu.y <= BOARDHEIGHT && pBoard->LegalSideCheck(side, lu))) {
		legalPosition.push_back(lu);
	}

	Position ru(pos.x + 1, pos.y + 1);
	if ((side == SideType::Red && ru.x <= 5 && ru.y <= 2 && pBoard->LegalSideCheck(side, ru)) ||
		(side == SideType::Black && ru.x <= 5 && ru.y <= BOARDHEIGHT && pBoard->LegalSideCheck(side, ru))) {
		legalPosition.push_back(ru);
	}

	Position ld(pos.x - 1, pos.y - 1);
	if ((side == SideType::Red && ld.x >= 3 && ld.y >= 0 && pBoard->LegalSideCheck(side, ld)) ||
		(side == SideType::Black && ld.x >= 3 && ld.y >= BOARDHEIGHT - 2 && pBoard->LegalSideCheck(side, ld))) {
		legalPosition.push_back(ld);
	}

	Position rd(pos.x + 1, pos.y - 1);
	if ((side == SideType::Red && rd.x <= 5 && rd.y >= 0 && pBoard->LegalSideCheck(side, rd)) ||
		(side == SideType::Black && rd.x <= 5 && rd.y >= BOARDHEIGHT - 2 && pBoard->LegalSideCheck(side, rd))) {
		legalPosition.push_back(rd);
	}

	return legalPosition;
}

bool Guarder::CheckKillGeneral(Board *)
{
	return false;
}

Elephant::Elephant(Position pos, SideType side)
	: Man(pos, ManType::Elephant, side)
{
}

Elephant::~Elephant()
{
}

std::vector<Position> Elephant::LegalPosition(Board* pBoard)
{
	std::vector<Position> legalPosition;

	Position& pos = GetPosition();
	SideType& side = GetSideType();

	Position lu(pos.x - 2, pos.y + 2);
	if ((side == SideType::Red && lu.x >= 0 && lu.y <= 4 && pBoard->LegalSideCheck(side, lu)) ||
		(side == SideType::Black && lu.x >= 0 && lu.y <= BOARDHEIGHT && pBoard->LegalSideCheck(side, lu))) {
		legalPosition.push_back(lu);
	}

	Position ru(pos.x + 2, pos.y + 2);
	if ((side == SideType::Red && ru.x <= BOARDWIDTH && ru.y <= 4 && pBoard->LegalSideCheck(side, ru)) ||
		(side == SideType::Black && ru.x <= BOARDWIDTH && ru.y <= BOARDHEIGHT && pBoard->LegalSideCheck(side, ru))) {
		legalPosition.push_back(ru);
	}

	Position ld(pos.x - 2, pos.y - 2);
	if ((side == SideType::Red && ld.x >= 0 && ld.y >= 0 && pBoard->LegalSideCheck(side, ld)) ||
		(side == SideType::Black && ld.x >= 0 && ld.y >= BOARDHEIGHT - 4 && pBoard->LegalSideCheck(side, ld))) {
		legalPosition.push_back(ld);
	}

	Position rd(pos.x + 2, pos.y - 2);
	if ((side == SideType::Red && rd.x <= BOARDWIDTH && rd.y >= 0 && pBoard->LegalSideCheck(side, rd)) ||
		(side == SideType::Black && rd.x <= BOARDWIDTH && rd.y >= BOARDHEIGHT - 4 && pBoard->LegalSideCheck(side, rd))) {
		legalPosition.push_back(rd);
	}

	return legalPosition;
}

bool Elephant::CheckKillGeneral(Board *)
{
	return false;
}

Horse::Horse(Position pos, SideType side)
	: Man(pos, ManType::Horse, side)
{
}

Horse::~Horse()
{
}

std::vector<Position> Horse::LegalPosition(Board* pBoard)
{
	std::vector<Position> legalPosition;

	Position& pos = GetPosition();
	SideType& side = GetSideType();

	Position up1(pos.x - 1, pos.y + 2);
	if (up1.x >= 0 && up1.y <= BOARDHEIGHT && pBoard->LegalSideCheck(side, up1)) {
		Position up(pos.x, pos.y + 1);
		if (!pBoard->CheckPosHasMan(up)) {
			legalPosition.push_back(up1);
		}
	}

	Position up2(pos.x + 1, pos.y + 2);
	if (up2.x <= BOARDWIDTH && up2.y <= BOARDHEIGHT && pBoard->LegalSideCheck(side, up2)) {
		Position up(pos.x, pos.y + 1);
		if (!pBoard->CheckPosHasMan(up)) {
			legalPosition.push_back(up2);
		}
	}

	Position left1(pos.x - 2, pos.y + 1);
	if (left1.x >= 0 && left1.y <= BOARDHEIGHT && pBoard->LegalSideCheck(side, left1)) {
		Position left(pos.x - 1, pos.y);
		if (!pBoard->CheckPosHasMan(left)) {
			legalPosition.push_back(left1);
		}
	}

	Position left2(pos.x - 2, pos.y - 1);
	if (left2.x >= 0 && left2.y >= 0 && pBoard->LegalSideCheck(side, left2)) {
		Position left(pos.x - 1, pos.y);
		if (!pBoard->CheckPosHasMan(left)) {
			legalPosition.push_back(left2);
		}
	}

	Position down1(pos.x - 1, pos.y - 2);
	if (down1.x >= 0 && down1.y >= 0 && pBoard->LegalSideCheck(side, down1)) {
		Position down(pos.x, pos.y - 1);
		if (!pBoard->CheckPosHasMan(down)) {
			legalPosition.push_back(down1);
		}
	}

	Position down2(pos.x + 1, pos.y - 2);
	if (down2.x <= BOARDWIDTH && down2.y >= 0 && pBoard->LegalSideCheck(side, down2)) {
		Position down(pos.x, pos.y - 1);
		if (!pBoard->CheckPosHasMan(down)) {
			legalPosition.push_back(down2);
		}
	}

	Position right1(pos.x + 2, pos.y + 1);
	if (right1.x <= BOARDWIDTH && right1.y <= BOARDHEIGHT && pBoard->LegalSideCheck(side, right1)) {
		Position right(pos.x + 1, pos.y);
		if (!pBoard->CheckPosHasMan(right)) {
			legalPosition.push_back(right1);
		}
	}

	Position right2(pos.x + 2, pos.y - 1);
	if (right2.x <= BOARDWIDTH && right2.y >= 0 && pBoard->LegalSideCheck(side, right2)) {
		Position right(pos.x + 1, pos.y);
		if (!pBoard->CheckPosHasMan(right)) {
			legalPosition.push_back(right2);
		}
	}

	return legalPosition;
}

bool Horse::CheckKillGeneral(Board* pBoard)
{
	SideType side = GetSideType();
	Position dstPos;
	if (side == SideType::Red) {
		dstPos = pBoard->BlackGeneral()->GetPosition();
	}
	else {
		dstPos = pBoard->RedGeneral()->GetPosition();
	}

	std::vector<Position> legalPos = LegalPosition(pBoard);
	auto it = std::find(legalPos.begin(), legalPos.end(), dstPos);
	if (it != legalPos.end()) {
		return true;
	}

	return false;
}

Vehicle::Vehicle(Position pos, SideType side)
	: Man(pos, ManType::Vehicle, side)
{
}

Vehicle::~Vehicle()
{
}

std::vector<Position> Vehicle::LegalPosition(Board* pBoard)
{
	std::vector<Position> legalPosition;

	Position& pos = GetPosition();
	SideType& side = GetSideType();

	auto fCheckPos = [&](Position& p) -> bool {
		if (!pBoard->CheckPosHasMan(p)) {
			legalPosition.push_back(p);
			return true;
		}

		if (pBoard->CheckPosSide(p) != side) {
			legalPosition.push_back(p);
		}
		return false;
	};

	for (int y = pos.y + 1; y <= BOARDHEIGHT; y++) {
		Position p(pos.x, y);
		if (fCheckPos(p)) {
			break;
		}
	}

	for (int y = pos.y - 1; y >= 0; y--) {
		Position p(pos.x, y);
		if (!fCheckPos(p)) {
			break;
		}
	}

	for (int x = pos.x - 1; x >= 0; x--) {
		Position p(x, pos.y);
		if (!fCheckPos(p)) {
			break;
		}
	}

	for (int x = pos.x + 1; x <= BOARDWIDTH; x++) {
		Position p(x, pos.y);
		if (!fCheckPos(p)) {
			break;
		}
	}
	return legalPosition;
}

bool Vehicle::CheckKillGeneral(Board* pBoard)
{
	SideType side = GetSideType();
	Position dstPos;
	if (side == SideType::Red) {
		dstPos = pBoard->BlackGeneral()->GetPosition();
	}
	else {
		dstPos = pBoard->RedGeneral()->GetPosition();
	}

	Position oriPos = GetPosition();
	if (oriPos.x != dstPos.x && oriPos.y != dstPos.y) {
		return false;
	}

	if (oriPos.x == dstPos.x) {
		int s, e;
		Interval(oriPos.y, dstPos.y, s, e);

		for (int y = s; y <= e; y++) {
			if (pBoard->GetBoard()[oriPos.x][y] != nullptr) {
				return false;
			}
		}

		return true;
	}
	else if (oriPos.y == dstPos.y) {
		int s, e;
		Interval(oriPos.x, dstPos.x, s, e);

		for (int x = s; x <= e; x++) {
			if (pBoard->GetBoard()[x][oriPos.y] != nullptr) {
				return false;
			}
		}

		return true;
	}

	return false;
}

Gun::Gun(Position pos, SideType side)
	: Man(pos, ManType::Gun, side)
{
}

Gun::~Gun()
{
}

std::vector<Position> Gun::LegalPosition(Board* pBoard)
{
	std::vector<Position> legalPosition;

	Position& pos = GetPosition();
	SideType& side = GetSideType();
	bool interval = false;

	auto fCheckPos = [&](Position& p) -> bool {
		if (!interval && !pBoard->CheckPosHasMan(p)) {
			legalPosition.push_back(p);
			return true;
		}

		if (!interval) {
			interval = true;
			return true;
		}

		if (!pBoard->CheckPosHasMan(p)) {
			return true;
		}

		if (pBoard->CheckPosSide(p) != side) {
			legalPosition.push_back(p);
		}

		return false;
	};

	interval = false;
	for (int y = pos.y + 1; y <= BOARDHEIGHT; y++) {
		Position p(pos.x, y);
		if (!fCheckPos(p)) {
			break;
		}
	}

	interval = false;
	for (int y = pos.y - 1; y >= 0; y--) {
		Position p(pos.x, y);
		if (!fCheckPos(p)) {
			break;
		}
	}

	interval = false;
	for (int x = pos.x - 1; x >= 0; x--) {
		Position p(x, pos.y);
		if (!fCheckPos(p)) {
			break;
		}
	}

	interval = false;
	for (int x = pos.x + 1; x <= BOARDWIDTH; x++) {
		Position p(x, pos.y);
		if (!fCheckPos(p)) {
			break;
		}
	}
	return legalPosition;
}

bool Gun::CheckKillGeneral(Board* pBoard)
{
	SideType side = GetSideType();
	Position dstPos;
	if (side == SideType::Red) {
		dstPos = pBoard->BlackGeneral()->GetPosition();
	}
	else {
		dstPos = pBoard->RedGeneral()->GetPosition();
	}

	std::vector<Position> legalPos = LegalPosition(pBoard);
	auto it = std::find(legalPos.begin(), legalPos.end(), dstPos);
	if (it != legalPos.end()) {
		return true;
	}

	return false;
}

Soldier::Soldier(Position pos, SideType side)
	: Man(pos, ManType::Soldier, side)
{
}

Soldier::~Soldier()
{
}

std::vector<Position> Soldier::LegalPosition(Board* pBoard)
{
	std::vector<Position> legalPosition;

	Position& pos = GetPosition();
	SideType& side = GetSideType();

	Position left(pos.x - 1, pos.y);
	if ((side == SideType::Red && pos.y > 4 && left.x >= 0 && pBoard->LegalSideCheck(side, left)) ||
		(side == SideType::Black && pos.y < 5 && left.x >= 0 && pBoard->LegalSideCheck(side, left))) {
		legalPosition.push_back(left);
	}

	Position right(pos.x + 1, pos.y);
	if ((side == SideType::Red && pos.y > 4 && right.x <= BOARDWIDTH && pBoard->LegalSideCheck(side, right)) ||
		(side == SideType::Black && pos.y < 5 && right.x <= BOARDWIDTH && pBoard->LegalSideCheck(side, right))) {
		legalPosition.push_back(right);
	}

	Position up(pos.x, pos.y + 1);
	if (side == SideType::Red && up.y <= BOARDHEIGHT && pBoard->LegalSideCheck(side, up)) {
		legalPosition.push_back(up);
	}

	Position down(pos.x, pos.y - 1);
	if (side == SideType::Black && down.y >= 0 && pBoard->LegalSideCheck(side, down)) {
		legalPosition.push_back(down);
	}

	return legalPosition;
}

bool Soldier::CheckKillGeneral(Board* pBoard)
{
	SideType side = GetSideType();
	Position dstPos;
	if (side == SideType::Red) {
		dstPos = pBoard->BlackGeneral()->GetPosition();
	}
	else {
		dstPos = pBoard->RedGeneral()->GetPosition();
	}

	std::vector<Position> legalPos = LegalPosition(pBoard);
	auto it = std::find(legalPos.begin(), legalPos.end(), dstPos);
	if (it != legalPos.end()) {
		return true;
	}

	return false;
}

ChineseChess::Action::Action(SideType turn)
	: m_nFinishType(FinishType::Unknown)
	, m_nTurnType(turn)
{

}

Action::~Action()
{
}
