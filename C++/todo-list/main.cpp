#include <iostream>
#include <fstream>
#include <unordered_map>
#include <cstdio>

class TodoList
{
    std::unordered_map<size_t, std::pair<std::string, bool>> mp;
    size_t max_entry_id = 0;
public:
    void saveToText(std::string path)
    {
        std::ofstream outf(path);
        if (!outf.is_open()) {
            std::cout << "Error opening file: " << path << std::endl;
            exit(1);
        }
        outf << max_entry_id << std::endl;
        for (int i = 0; i < max_entry_id; ++i) {
            if (!mp.count(i)) continue;
            const auto &entry = mp[i];
            outf << i << "," << entry.second << "," << entry.first << std::endl;
        }
        std::cout << "Written " << mp.size() << " entries\n";
        outf.close();
    }
    void loadFromText(std::string path)
    {
        std::ifstream inpf(path);
        if (!inpf.is_open()) {
            std::cerr << "Error opening file: " << path << std::endl;
            exit(1);
        }
        std::string line;
        int line_no = 0;
        std::getline(inpf, line);
        max_entry_id = std::stoul(line);
        while (std::getline(inpf, line)) {
            ++line_no;
            size_t delimiterPos1 = line.find(",");
            if (delimiterPos1 == std::string::npos) {
                std::cout << "Error reading line " << line_no << " of file: " << path << std::endl;
                exit(1);
            }
            size_t delimiterPos2 = line.find(",", delimiterPos1 + 1);
            if (delimiterPos2 == std::string::npos) {
                std::cout << "Error reading line " << line_no << " of file: " << path << std::endl;
                exit(1);
            }
            try {
                size_t id = std::stoul(line.substr(0, delimiterPos1));
                bool status = (bool) std::stoul(line.substr(delimiterPos1 + 1, delimiterPos2 - delimiterPos1 - 1));
                std::string info = line.substr(delimiterPos2 + 1);
                mp[id] = std::make_pair(info, status);
            } catch (std::invalid_argument e) {
                std::cout << "Error reading line " << line_no << " of file: " << path << std::endl;
                exit(1);
            }
        }
        inpf.close();
        std::cout << "\nLoaded " << mp.size() << " entries\n";
    }
    void addEntry(std::string info)
    {
        mp[max_entry_id] = { info, false };
        ++max_entry_id;
    }
    void viewEntries()
    {
        std::cout << "\n   Id |  Status | Info\n";
        for (int i = 0; i < max_entry_id; ++i) {
            if (!mp.count(i)) continue;
            /* format and align o/p: 5 char id, min 7 char
               status, max 30 char info */
            printf("%5d | %7s | %.30s%s\n",
                i,
                mp[i].second ? "done" : "pending",
                mp[i].first.c_str(),
                mp[i].first.size() > 30 ? "..." : "");
        }
        std::cout << "\nShowing " << mp.size() << " entries\n";
    }
    void viewEntry(int id)
    {
        if (!mp.count(id)) {
            std::cout << "No entry for Id '" << id << "'\n";
            return;
        }
        std::cout << "\nStatus: " << (mp[id].second ? "done" : "pending") << "\n"
                  << "  " << mp[id].first << "\n\n";
    }
    void markDone(int id)
    {
        if (!mp.count(id)) {
            std::cout << "No entry for Id '" << id << "'\n";
            return;
        }
        mp[id].second = true;
    }
    void markPending(int id)
    {
        if (!mp.count(id)) {
            std::cout << "No entry for Id '" << id << "'\n";
            return;
        }
        mp[id].second = false;
    }
    void delEntry(int id)
    {
        if (mp.count(id)) mp.erase(id);
    }
};

int main()
{
    typedef enum { END = 0, LOAD, SAVE, NEW, VIEW,
                   VIEW_ID, MARK_DONE, MARK_PENDING,
                   DELETE } choice_t;
    TodoList todo;
    while (true) {
        std::cout <<
            "\nChoices:\n"
            "   Enter " << LOAD         << " to LOAD existing list\n"
            "   Enter " << SAVE         << " to SAVE current list\n"
            "   Enter " << NEW          << " to CREATE todo entry\n"
            "   Enter " << VIEW         << " to VIEW all entries\n"
            "   Enter " << VIEW_ID      << " to VIEW an entry by Id\n"
            "   Enter " << MARK_DONE    << " to MARK as done\n"
            "   Enter " << MARK_PENDING << " to MARK as pending\n"
            "   Enter " << DELETE       << " to DELETE entry\n"
            "   Enter " << END          << " to EXIT\n"
            "Enter your choice: ";

        /* taking input to an enum */
        choice_t choice = (choice_t) ({ int ch; std::cin >> ch; ch; });

        switch (choice) {
            case LOAD: {
                std::cout << "\nWARNING:\n"
                          << "  This will overwrite current list\n"
                          << "  You are recommended to save your list first\n"
                          << "Enter Y to load, cancel otherwise: ";
                 char ch; std::cin >> ch;
                 switch (ch) {
                    case 'y': case 'Y': {
                        std::cout << "Enter file path: ";
                        std::string path;
                        std::cin.ignore();
                        getline(std::cin, path);
                        todo.loadFromText(path);
                    }
                    default: break;
                }
                break;
            }
            case SAVE: {
                std::cout << "Enter file path: ";
                std::cin.ignore();
                std::string path; getline(std::cin, path);
                todo.saveToText(path);
                break;
            }
            case NEW: {
                std::cout << "Enter todo details: ";
                std::cin.ignore();
                std::string info; getline(std::cin, info);
                todo.addEntry(info);
                break;
            }
            case VIEW:
                todo.viewEntries();
                break;
            case VIEW_ID: {
                std::cout << "Enter todo Id: ";
                int id; std::cin >> id;
                todo.viewEntry(id);
                break;
            }
            case MARK_DONE: {
                std::cout << "Enter todo Id: ";
                int id; std::cin >> id;
                todo.markDone(id);
                break;
            }
            case MARK_PENDING: {
                std::cout << "Enter todo Id: ";
                int id; std::cin >> id;
                todo.markPending(id);
                break;
            }
            case DELETE: {
                std::cout << "Enter todo Id: ";
                int id; std::cin >> id;
                todo.delEntry(id);
                break;
            }
            case END: exit(0);
            default:
                std::cout << "\nInvalid input, try again\n";
        }
    }
    return 0;
}
