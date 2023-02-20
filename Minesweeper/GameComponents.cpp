#include "GameComponents.h"

Board::Board(uint32_t width, uint32_t height, uint32_t minesNum, GameResult& result)
	:m_width(width), m_height(height), m_minesNum(minesNum), m_result(result)
{
	m_squares.resize(height);
	std::ranges::for_each(m_squares, [&width](std::vector<Square>& xSquares) {xSquares.resize(width); });

	generateMines(m_minesNum);
	countMines();
}

Board::~Board()
{
}

void Board::begin()
{
	print();
}

void Board::input(uint32_t x, uint32_t y, Action action)
{
	if (!isValidSquare(x, y)) return;

	switch (action)
	{
	case Action::FLAG:
		m_squares.at(y).at(x).flag();
		break;

	case Action::REVEAL:
		revealSquare(x, y);
		break;

	default:
		std::cerr << "ERROR: invalid move";
		break;
	}

	refreshState();
	print();
}

void Board::print()
{
	std::cout << '\n' << std::setw(2) << "   ";
	if (m_width >= 10)
		for (size_t x = 0; x < m_width; ++x)
			std::cout << std::setw(2) << (x % 10 == 0 ? std::to_string(x / 10) : " ");
	std::cout << '\n' << std::setw(2) << "   ";
	for (size_t x = 0; x < m_width; ++x)
		std::cout << std::setw(2) << x % 10;
	std::cout << '\n' << std::setw(2) << "   ";
	for (size_t x = 0; x < m_width; ++x)
		std::cout << std::setw(2) << '|';
	std::cout << '\n';

	for (size_t y = 0; y < m_height; ++y)
	{
		std::cout << std::setw(2) << y << "-";
		for (size_t x = 0; x < m_width; ++x)
			std::cout << std::setw(2) << m_squares.at(y).at(x).print();
		std::cout << '\n';
	}

#ifdef _DEBUG
	std::cout << '\n';
	for (size_t y = 0; y < m_height; ++y)
	{
		for (size_t x = 0; x < m_width; ++x)
			std::cout << std::setw(2) << (int)m_squares.at(y).at(x).isMine;
		std::cout << '\n';
	}
#endif // _DEBUG
}

void Board::generateMines(uint32_t num)
{
	std::mt19937_64 generator(std::random_device{}());
	std::uniform_int_distribution<> xDistribution(0, m_width - 1);
	std::uniform_int_distribution<> yDistribution(0, m_height - 1);

	for (size_t i = 0; i < num; )
	{
		uint8_t x = xDistribution(generator);
		uint8_t y = yDistribution(generator);
		Square& chosenSquare = m_squares.at(y).at(x);

		if (chosenSquare.isMine) continue;

		chosenSquare.isMine = true;
		++i;
	}
}

void Board::revealSquare(int32_t x, int32_t y)
{
	Square& selectedSquare = m_squares.at(y).at(x);

	// check if the first move is on a mine
	if (selectedSquare.isMine && !m_revealedNum)
	{
		generateMines(1);
		selectedSquare.isMine = false;
	}

	switch (selectedSquare.reveal())
	{
	case SquareState::MINE:
		m_result = GameResult::LOSE;
		return;
	case SquareState::SAFE:
		++m_revealedNum;
		if (!selectedSquare.adjacentMineNum)
			openAdjacent(x, y);
		return;
	default:
		break;
	}
}

void Board::countMines()
{
	for (auto [y, yIt] = std::pair(0, m_squares.begin()); yIt != m_squares.end(); ++yIt, ++y)
		for (auto [x, xIt] = std::pair(0, yIt->begin()); xIt != yIt->end(); ++xIt, ++x)
			for (auto& [xChange, yChange] : detectionSequence)
			{
				int32_t newX = x + xChange;
				int32_t newY = y + yChange;
				if (!isValidSquare(newX, newY)) continue;
				if (m_squares.at(newY).at(newX).isMine)
					++xIt->adjacentMineNum;
			}
}

bool Board::isValidSquare(int32_t x, int32_t y)
{
	if (x < 0 || y < 0 || std::cmp_greater_equal(x, m_width) || std::cmp_greater_equal(y, m_height))
		return false;
	return true;
}

void Board::openAdjacent(uint32_t x, uint32_t y)
{
	std::stack<Vector> emptySquares;
	emptySquares.emplace(x, y);

	while (!emptySquares.empty())
	{
		auto [thisX, thisY] = emptySquares.top();
		emptySquares.pop();
		for (auto& [xChange, yChange] : detectionSequence)
		{
			int32_t newX = thisX + xChange;
			int32_t newY = thisY + yChange;

			if (!isValidSquare(newX, newY)) continue;

			Square& newSquare = m_squares.at(newY).at(newX);
			if (newSquare.reveal() != SquareState::SAFE) continue;
			++m_revealedNum;
			if (!newSquare.adjacentMineNum)
				emptySquares.emplace(newX, newY);
		}
	}
}

void Board::refreshState()
{
	uint32_t remainSquares = m_width * m_height - m_revealedNum;
	if (remainSquares <= m_minesNum)
		m_result = GameResult::WIN;
}

std::string Square::print()
{
	if (isFlagged) return MARKED_SQUARE_SYMBOL;
	if (!isRevealed) return COVERED_SQUARE_SYMBOL;
	if (isMine) return MINE_SQUARE_SYMBOL;
	if (adjacentMineNum) return std::to_string(adjacentMineNum);
	return SAFE_SQUARE_SYMBOL;
}

SquareState Square::reveal()
{
	if (isFlagged || isRevealed) return SquareState::INVALID;
	if (isMine)
	{
		isRevealed = true;
		return SquareState::MINE;
	}
	isRevealed = true;
	return SquareState::SAFE;
}

void Square::flag()
{
	if (isRevealed) return;
	isFlagged = !isFlagged;
}
