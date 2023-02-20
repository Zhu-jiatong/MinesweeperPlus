#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <ranges>
#include <iomanip>
#include <array>
#include <stack>

struct Vector
{
	int32_t x, y;
};

enum class Action
{
	FLAG, REVEAL
};

enum class GameResult
{
	WIN, LOSE, EXIT, PENDING
};

enum class SquareState
{
	SAFE, MINE, INVALID
};

const std::string MINE_SQUARE_SYMBOL = "x";
const std::string SAFE_SQUARE_SYMBOL = " ";
const std::string COVERED_SQUARE_SYMBOL = "#";
const std::string MARKED_SQUARE_SYMBOL = "!";

const std::array<Vector, 8> detectionSequence{ {
	{1, 1},
	{1, 0},
	{1, -1},
	{0, -1},
	{-1, -1},
	{-1, 0},
	{-1, 1},
	{0, 1}
} };

struct Square
{
	bool isMine = false;
	bool isRevealed = false;
	bool isFlagged = false;
	uint8_t adjacentMineNum = 0;

	std::string print();
	SquareState reveal();
	void flag();
};

class Board
{
public:
	Board(uint32_t width, uint32_t height, uint32_t minesNum, GameResult& result);
	~Board();

	void begin();
	void input(uint32_t x, uint32_t y, Action action);

private:
	std::vector<std::vector<Square>> m_squares;
	uint32_t m_width, m_height, m_minesNum, m_revealedNum = 0;
	GameResult& m_result;

	void print();
	void generateMines(uint32_t num);
	void revealSquare(int32_t x, int32_t y);
	void countMines();
	bool isValidSquare(int32_t x, int32_t y);
	void openAdjacent(uint32_t x, uint32_t y);
	void refreshState();
};
