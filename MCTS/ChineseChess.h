#pragma once
#include <utility>
#include <vector>
#include <list>

namespace ChineseChess 
{
	enum class ManType {
		General,	// 将
		Guarder,	// 士
		Elephant,	// 象
		Horse,		// 马
		Vehicle,	// 车
		Gun,		// 炮
		Soldier,	// 兵
	};

	enum class SideType {
		Red,		// 红
		Black,		// 黑
	};

	enum class FinishType {
		RedVictory,
		BlackVictory,
		Draw,
		Unknown,
	};

	const int BOARDWIDTH		= 8;	// 0 - 8
	const int BOARDHEIGHT		= 9;	// 0 - 9

	struct Position {
		Position() {
			this->x = 0;
			this->y = 0;
		}

		Position(int x, int y) {
			this->x = x;
			this->y = y;
		}

		bool operator==(const Position& pos) {
			if (this->x == pos.x &&
				this->y == pos.y) {
				return true;
			}
			return false;
		}

		int x;
		int y;
	};

	const Position GeneralStd(4, 0);
	const Position GuarderStdL(3, 0);
	const Position GuarderStdR(5, 0);
	const Position ElephantStdL(2, 0);
	const Position ElephantStdR(6, 0);
	const Position HorseStdL(1, 0);
	const Position HorseStdR(7, 0);
	const Position VehicleStdL(0, 0);
	const Position VehicleStdR(8, 0);
	const Position GunStdL(1, 2);
	const Position GunStdR(7, 2);
	const Position SoldierStd1(0, 3);
	const Position SoldierStd2(2, 3);
	const Position SoldierStd3(4, 3);
	const Position SoldierStd4(6, 3);
	const Position SoldierStd5(8, 3);
	
	inline Position ReversePosition(const Position& pos);
	inline void Interval(int& y1, int& y2, int& s, int& e);
	inline bool CheckPosition(const Position& pos);

	class Board;
	class Man
	{
	public:
		Man(Position, ManType, SideType);
		virtual ~Man();

		bool operator==(const Man& man) {
			if (this->m_nPosition == man.m_nPosition) {
				return true;
			}
			return false;
		}

		Position&	GetPosition();
		ManType&	GetManType();
		SideType&	GetSideType();

		virtual std::vector<Position> LegalPosition(Board*);
		virtual bool CheckKillGeneral(Board*);

	private:
		Position	m_nPosition;
		ManType		m_nManType;
		SideType	m_nSideType;
	};

	class General : public Man 
	{
	public:
		General(Position, SideType);
		virtual ~General();

		virtual std::vector<Position> LegalPosition(Board*);
		virtual bool CheckKillGeneral(Board*);
	};

	class Guarder : public Man
	{
	public:
		Guarder(Position, SideType);
		virtual ~Guarder();

		virtual std::vector<Position> LegalPosition(Board*);
		virtual bool CheckKillGeneral(Board*);
	};

	class Elephant : public Man
	{
	public:
		Elephant(Position, SideType);
		virtual ~Elephant();

		virtual std::vector<Position> LegalPosition(Board*);
		virtual bool CheckKillGeneral(Board*);
	};

	class Horse : public Man
	{
	public:
		Horse(Position, SideType);
		virtual ~Horse();

		virtual std::vector<Position> LegalPosition(Board*);
		virtual bool CheckKillGeneral(Board*);
	};

	class Vehicle : public Man
	{
	public:
		Vehicle(Position, SideType);
		virtual ~Vehicle();

		virtual std::vector<Position> LegalPosition(Board*);
		virtual bool CheckKillGeneral(Board*);
	};

	class Gun : public Man
	{
	public:
		Gun(Position, SideType);
		virtual ~Gun();

		virtual std::vector<Position> LegalPosition(Board*);
		virtual bool CheckKillGeneral(Board*);
	};

	class Soldier : public Man
	{
	public:
		Soldier(Position, SideType);
		virtual ~Soldier();

		virtual std::vector<Position> LegalPosition(Board*);
		virtual bool CheckKillGeneral(Board*);
	};

	class Board
	{
	public:
		Board();
		virtual ~Board();
		Board(const Board&);

		Man* CreateMan(Position, ManType, SideType);
		void AddGeneral(Position, SideType);
		void AddMan(Position, ManType, SideType);
		void MoveMan(Position, Position);
		Man* GetMan(Position);
		void MoveMan(Man*, Position);
		void KillMan(Man*);
		FinishType CheckFinishType();
		bool CheckFaceToFace();

		bool LegalSideCheck(SideType&, Position&);
		bool CheckPosHasMan(Position&);
		SideType& CheckPosSide(Position&);

		std::list<Man*>& RedMan();
		std::list<Man*>& BlackMan();
		Man* RedGeneral();
		Man* BlackGeneral();
		std::vector<std::vector<Man*>>& GetBoard();

		std::vector<std::tuple<Position, Position, FinishType>> GetLegalPosition(SideType);
		bool GetRandomAction(SideType, std::pair<Position, Position>&);
		bool CheckKillGeneral(SideType, std::pair<Position, Position>&);

	private:
		Man*							m_nRedGeneral;
		Man*							m_nBlackGeneral;
		std::list<Man*>					m_nRedMan;
		std::list<Man*>					m_nBlackMan;
		std::vector<std::vector<Man*>>	m_nBoard;
	};

	class Action
	{
	public:
		Action(SideType turn = SideType::Red);
		virtual ~Action();

		SideType	m_nTurnType;
		Position	m_nOriPos;
		Position	m_nDstPos;
		FinishType	m_nFinishType;
	};
}