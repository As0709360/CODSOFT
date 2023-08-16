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

public:
    Book()
        : isbn(nullptr), author(nullptr), title(nullptr), borrowed(false) {}
    Book(const std::string isbn, const std::string author, const std::string title)
        : isbn(isbn), author(author), title(title), borrowed(false) {}
    std::string getISBN() const   { return isbn; }
    std::string getAuthor() const { return author; }
    std::string getTitle() const  { return title; }
    bool isBorrowed() const       { return borrowed; }
    void setBorrowed(bool status) { borrowed = status; }
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
        books.clear();
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
            std::cout << "ISBN " << isbn << " already exists";
            return;
        }
        updateBook(isbn, author, title);
    }

    void updateBook(std::string isbn, std::string author, std::string title)
    {
        books[isbn] = new Book(isbn, author, title);
    }

    void borrowBook(const std::string isbn)
    {
        if (books.count(isbn) && !books[isbn]->isBorrowed()) {
            books[isbn]->setBorrowed(true);
            std::cout << "Borrowed ISBN " << isbn << std::endl;
        }
        else std::cout << "ISBN " << isbn << " is not in catalogue\n";
    }

    void returnBook(const std::string isbn)
    {
        if (books.count(isbn) && books[isbn]->isBorrowed()) {
            books[isbn]->setBorrowed(false);
            std::cout << "Returned ISBN " << isbn << std::endl;
        }
        else std::cout << "ISBN " << isbn << " is already in catalogue\n";
    }

    void checkAvailability(std::string isbn)
    {
        if (books.count(isbn)) {
            if (books[isbn]->isBorrowed())
                std::cout << "ISBN " << isbn << " is currently borrowed\n";
            else std::cout << "ISBN " << isbn << " is available for borrowing\n";
        }
        else std::cout << "ISBN " << isbn << " not found.\n";
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
};

int main() {
    Library library;

    library.addBook("123456789", "J.K. Rowling", "Harry Potter and the Sorcerer's Stone");
    library.addBook("987654321", "George Orwell", "1984");

    library.borrowBook("123456789");
    library.returnBook("123456789");
    library.returnBook("123456789");

    library.searchBooks("x");

    return 0;
}
