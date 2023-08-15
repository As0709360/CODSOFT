#include <iostream>

class Game
{
public:
    /** Possible player states */
    typedef enum {
        PLAYER_X,
        PLAYER_O
    } Player;

    /** Input and verify player value */
    static Player inputPlayer(std::string prompt)
    {
        std::cout << prompt;
        char ch; std::cin >> ch;
        switch (ch) {
            case 'x': case 'X': return PLAYER_X;
            case 'o': case 'O': return PLAYER_O;
            default:
               std::cout << "Invalid player value: '" << ch << "'\n";
               exit(1);
        }
        return PLAYER_X;
    }

    Game(Player p1, Player p2)
    {

    }

    void inputCell()
    {

    }

    void calcWinner()
    {

    }

    bool hasEnded()
    {

    }

    void switchPlayer()
    {
        if (now == PLAYER_X) now =  PLAYER_O;
        else now = PLAYER_X;
    }

    void displayWinner()
    {
        std::cout << "The winner is Player" << (now == PLAYER_X ? 'X' : 'O') << std::endl;
    }

private:
    Player board[3][3];
    Player now;
};

int main()
{
    Game game(
        Game::inputPlayer("Enter player 1 (X/O): "),
        Game::inputPlayer("Enter player 2 (X/O): "));
    while (true) {
        game.inputCell();
        game.calcWinner();
        if (game.hasEnded()) break;
        game.switchPlayer();
    }
    game.displayWinner();
    return 0;
}
