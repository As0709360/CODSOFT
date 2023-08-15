#include <iostream>
#include <ctime>

int main()
{
    std::cout << "Enter guess limit: ";
    int lim; std::cin >> lim;
    srand( time(0) ^ clock() );
    const int num = rand() % lim;
    while (true) {
        std::cout << "Enter your guess: ";
        int guess; std::cin >> guess;
        if (guess < num) {
            std::cout << "Your guess is too small, please retry\n";
            continue;
        } else if (guess > num) {
            std::cout << "Your guess is too large, please retry\n";
            continue;
        } else {
            std::cout << "You have guessed correctly!\n";
            break;
        }
    }
    return 0;
}
