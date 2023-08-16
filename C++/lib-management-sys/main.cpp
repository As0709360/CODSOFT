#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <algorithm>

/* helper methods */
namespace helper {
    std::string upper_string(const std::string &str)
    {
        std::string upper;
        std::transform(str.begin(), str.end(), std::back_inserter(upper), ::toupper);
        return upper;
    }
    std::string::size_type find_str_ci(const std::string &str, const std::string &substr)
    {
        return upper_string(str).find(upper_string(substr));
    }
}

class Book
{
    std::string isbn;
    std::string author;
    std::string title;
    bool borrowed;
    double fineRate;

public:
    Book()
        : isbn(nullptr), author(nullptr), title(nullptr), borrowed(false), fineRate(5.0) {}
    Book(const std::string isbn, const std::string author, const std::string title)
        : isbn(isbn), author(author), title(title), borrowed(false), fineRate(5.0) {}

    std::string getISBN() const   { return isbn; }
    std::string getAuthor() const { return author; }
    std::string getTitle() const  { return title; }
    bool isBorrowed() const       { return borrowed; }
    double getFineRate() const    { return fineRate; }

    void setAuthor(const std::string str) { author = str; }
    void setTitle(const std::string str)  { title = str; }
    void setBorrowed(bool status)         { borrowed = status; }
    void setFineRate(double rate)         { fineRate = rate; }
};

class Library
{
    std::unordered_map<std::string, Book*> books;

public:
    ~Library()
    {
        for (auto &p : books)
            delete p.second;
        books.clear();
    }

    void saveToText(const std::string path) const
    {
        std::ofstream outf(path);
        if (!outf.is_open()) {
            std::cout << "Error opening file: " << path << std::endl;
            exit(1);
        }
        for (const auto &pair : this->books) {
            const Book *book = pair.second;
            outf << book->getISBN()   << "||,"
                 << book->getAuthor() << "||,"
                 << book->getTitle()  << "||,"
                 << (book->isBorrowed() ? "1" : "0") << std::endl;
        }
        std::cout << "Written " << books.size() << " entries\n";
        outf.close();
    }

    void loadFromText(const std::string path)
    {
        std::ifstream inpf(path);
        if (!inpf.is_open()) {
            std::cerr << "Error opening file: " << path << std::endl;
            exit(1);
        }
        std::string line;
        int numBooks = 0;
        std::getline(inpf, line);
        this->~Library();
        while (std::getline(inpf, line)) {
            ++numBooks;
            size_t delimiterPos1 = line.find("||,");
            size_t delimiterPos2 = line.find("||,", delimiterPos1 + 1);
            size_t delimiterPos3 = line.find("||,", delimiterPos2 + 1);
            if (delimiterPos1 == std::string::npos || 
                delimiterPos2 == std::string::npos || 
                delimiterPos3 == std::string::npos) {
                std::cout << "Error reading line " << numBooks << " of file: " << path << std::endl;
                exit(1);
            }
            std::string isbn = line.substr(0, delimiterPos1);
            std::string author = line.substr(delimiterPos1 + 1, delimiterPos2 - delimiterPos1 - 1);
            std::string title = line.substr(delimiterPos2 + 1, delimiterPos3 - delimiterPos2 - 1);
            bool borrowed = (line.substr(delimiterPos3 + 1) == "1");
            this->addBook(isbn, author, title);
            if (borrowed)
                this->borrowBook(isbn);
        }
        inpf.close();
        std::cout << "Loaded " << numBooks << " entries\n";
    }

    void addBook(const std::string isbn, std::string author, std::string title)
    {
        if (books.count(isbn)) {
            std::cout << "ISBN " << isbn << " already exists\n";
            return;
        }
        delete books[isbn];
        books[isbn] = new Book(isbn, author, title);
    }

    void viewAllBooks() const
    {
        printf("%11s | %16s | %20s | %s\n", "ISBN", "Author", "Title", "Status");
        for (const auto& entry : books) {
            std::string isbn = entry.first;
            std::string author = entry.second->getAuthor();
            std::string title = entry.second->getTitle();
            bool available = !entry.second->isBorrowed();
            if (isbn.length() > 10)
                isbn = isbn.substr(0, 7) + "...";
            if (author.length() > 15)
                author = author.substr(0, 12) + "...";
            if (title.length() > 19)
                title = title.substr(0, 16) + "...";
            printf("%11s | %16s | %20s | %s\n", isbn.c_str(), author.c_str(), title.c_str(), available ? "available" : "borrowed");
        }
    }

    void updateBook(std::string isbn, std::string author, std::string title)
    {
        if (!books.count(isbn)) {
            std::cout << "ISBN " << isbn << " not found\n";
            return;
        }
        books[isbn]->setAuthor(author);
        books[isbn]->setTitle(title);
    }

    void delBook(std::string isbn)
    {
        if (!books.count(isbn)) {
            std::cout << "ISBN " << isbn << " not found\n";
            return;
        }
        delete books[isbn];
        books.erase(isbn);
    }

    void updateFineRate(std::string isbn, double rate)
    {
        if (!books.count(isbn)) {
            std::cout << "ISBN " << isbn << " not found\n";
            return;
        }
        books[isbn]->setFineRate(rate);
    }

    void borrowBook(const std::string isbn)
    {
        if (books.count(isbn) && !books[isbn]->isBorrowed()) {
            books[isbn]->setBorrowed(true);
            std::cout << "Borrowed ISBN " << isbn << std::endl;
        }
        else std::cout << "ISBN " << isbn << " is not in catalog\n";
    }

    void returnBook(const std::string isbn)
    {
        if (books.count(isbn) && books[isbn]->isBorrowed()) {
            books[isbn]->setBorrowed(false);
            std::cout << "Returned ISBN " << isbn << std::endl;
        }
        else std::cout << "ISBN " << isbn << " is already in catalog\n";
    }

    void checkAvailability(std::string isbn)
    {
        if (books.count(isbn)) {
            if (books[isbn]->isBorrowed())
                std::cout << "ISBN " << isbn << " is currently borrowed\n";
            else std::cout << "ISBN " << isbn << " is available for borrowing\n";
        }
        else std::cout << "ISBN " << isbn << " not found\n";
    }

    void searchBooks(const std::string searchTerm) const
    {
        std::cout << "\nSearch results:\n";
        int srch_cnt = 0;
        for (const auto &pair : books) {
            if (pair.second->getISBN() == searchTerm ||
                helper::find_str_ci(pair.second->getAuthor(), searchTerm) != std::string::npos ||
                helper::find_str_ci(pair.second->getTitle(), searchTerm) != std::string::npos) {
                ++srch_cnt;
                std::cout << "  ISBN:   " << pair.second->getISBN() << "\n"
                          << "  Author: " << pair.second->getAuthor() << "\n"
                          << "  Title:  " << pair.second->getTitle() << "\n\n";
            }
        }
        if (!srch_cnt) std::cout << "  Empty\n";
    }

    double calcFine(const std::string &isbn, int lateDays)
    {
        if (!books.count(isbn)) {
            std::cout << "ISBN " << isbn << " not found\n";
            return -1;
        }
        return lateDays * books[isbn]->getFineRate();
    }
};

int main()
{
    Library library;

    typedef enum {
        END = 0, LOAD, SAVE, ADD_BOOK, VIEW_ALL, UPDATE_BOOK, DEL_BOOK,
        SEARCH_BOOK, RETURN_BOOK, BORR_BOOK, CHECK_AVL,
        UPDATE_FINE, CALC_FINE
    } choice_t;

    while (true) {
        std::cout <<
            "\nChoices:\n"
            "   Enter " << LOAD         << " to LOAD existing data\n"
            "   Enter " << SAVE         << " to SAVE data to file\n"
            "   Enter " << ADD_BOOK     << " to ADD a new book entry\n"
            "   Enter " << VIEW_ALL     << " to VIEW all books\n"
            "   Enter " << UPDATE_BOOK  << " to UPDATE a book\n"
            "   Enter " << DEL_BOOK     << " to REMOVE book a entry\n"
            "   Enter " << SEARCH_BOOK  << " to SEARCH books\n"
            "   Enter " << RETURN_BOOK  << " to RETURN a book\n"
            "   Enter " << BORR_BOOK    << " to BORROW a book\n"
            "   Enter " << CHECK_AVL    << " to CHECK availability\n"
            "   Enter " << UPDATE_FINE  << " to UPDATE book fine rate\n"
            "   Enter " << CALC_FINE    << " to CALCULATE late fines\n"
            "   Enter " << END          << " to EXIT\n"
            "Enter your choice: ";

        /* taking input to an enum */
        choice_t choice = (choice_t) ({ int ch; std::cin >> ch; ch; });
        std::cin.ignore();

        switch (choice) {
            case LOAD: {
                std::cout << "\nWARNING:\n"
                          << "  This will overwrite current data in memory\n"
                          << "  You are recommended to save your data first\n"
                          << "Enter Y to load, cancel otherwise: ";
                char ch; std::cin >> ch;
                if (ch == 'y' || ch == 'Y') {
                    std::cout << "Enter file path: ";
                    std::string path;
                    std::cin.ignore();
                    std::getline(std::cin, path);
                    library.loadFromText(path);
                }
                break;
            }
            case SAVE: {
                std::cout << "Enter file path: ";
                std::string path;
                std::getline(std::cin, path);
                library.saveToText(path);
                break;
            }
            case ADD_BOOK: {
                std::cout << "Enter ISBN: ";
                std::string isbn; std::getline(std::cin, isbn);
                std::cout << "Enter author: ";
                std::string author; std::getline(std::cin, author);
                std::cout << "Enter title: ";
                std::string title; std::getline(std::cin, title);
                library.addBook(isbn, author, title);
                break;
            }
            case VIEW_ALL:
                library.viewAllBooks();
                break;
            case UPDATE_BOOK: {
                std::cout << "Enter ISBN: ";
                std::string isbn; std::getline(std::cin, isbn);
                std::cout << "Enter updated author: ";
                std::string author; std::getline(std::cin, author);
                std::cout << "Enter updated title: ";
                std::string title; std::getline(std::cin, title);
                library.updateBook(isbn, author, title);
                break;
            }
            case DEL_BOOK: {
                std::cout << "Enter ISBN: ";
                std::string isbn; std::getline(std::cin, isbn);
                library.delBook(isbn);
                break;
            }
            case SEARCH_BOOK: {
                std::cout << "Enter search query: ";
                std::string searchTerm; std::getline(std::cin, searchTerm);
                library.searchBooks(searchTerm);
                break;
            }
            case RETURN_BOOK: {
                std::cout << "Enter ISBN: ";
                std::string isbn; std::getline(std::cin, isbn);
                library.returnBook(isbn);
                break;
            }
            case BORR_BOOK: {
                std::cout << "Enter ISBN: ";
                std::string isbn; std::getline(std::cin, isbn);
                library.borrowBook(isbn);
                break;
            }
            case CHECK_AVL: {
                std::cout << "Enter ISBN: ";
                std::string isbn; std::getline(std::cin, isbn);
                library.checkAvailability(isbn);
                break;
            }
            case UPDATE_FINE: {
                std::cout << "Enter ISBN: ";
                std::string isbn; std::getline(std::cin, isbn);
                std::cout << "Enter daily late fine: ";
                double rate; std::cin >> rate;
                library.updateFineRate(isbn, rate);
                break;
            }
            case CALC_FINE: {
                std::cout << "Enter ISBN: ";
                std::string isbn; std::getline(std::cin, isbn);
                std::cout << "Enter late days: ";
                int lateDays; std::cin >> lateDays;
                double fine = library.calcFine(isbn, lateDays);
                if (fine < 0) break;
                std::cout << "Fine for " << lateDays <<" D late return: "
                          << fine
                          << std::endl;
                break;
            }
            case END: exit(0);
            default:
                std::cout << "\nInvalid input, try again\n";
        }
    }

    return 0;
}
