#include "GameComponents.h"

std::string resultToString(GameResult result);

int main(int argc, char* argv[])
{
	uint32_t boardWidth = 16;
	uint32_t boardHeight = 16;
	uint32_t minesNum = 40;
	if (argc > 3)
	{
		boardWidth = atoi(argv[1]);
		boardHeight = atoi(argv[2]);
		minesNum = atoi(argv[3]);
	}

	GameResult gameStatus = GameResult::PENDING;
	Board game(boardWidth, boardHeight, minesNum, gameStatus);
	game.begin();

	while (gameStatus == GameResult::PENDING)
	{
		int32_t x = -1, y = -1, move = static_cast<int32_t>(Action::REVEAL);
		std::cout << "Move: ";
		std::cin >> x >> y >> move;
		if (x < 0) 
		{
			gameStatus = GameResult::EXIT;
			break;
		}
		game.input(x, y, static_cast<Action>(move));
	}

	std::cout << resultToString(gameStatus);

	std::cin.get();
	return 0;
}

std::string resultToString(GameResult result)
{
	switch (result)
	{
	case GameResult::WIN:
		return "You win!";
		break;
	case GameResult::LOSE:
		return "You lose :(";
		break;
	case GameResult::EXIT:
		return "You quit!";
		break;
	case GameResult::PENDING:
		return "Ongoing...";
		break;
	default:
		break;
	}
}