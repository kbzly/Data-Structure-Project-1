// Project Identifier: 950181F63D0A883F183EC0A5CC67B19928FE896A
#include <iostream>
#include <vector>
#include <deque>
#include <getopt.h>
#include <string>
#include <sstream>

struct node {
    int l, r, c;
    node(int level = 0, int row = 0, int column = 0) : l(level), r(row), c(column) {}
};



class FileReader {
public:
    char mode; // 'M' or 'L'
    int level;
    int size;
    node startNode;
    std::vector<std::vector<std::vector<char>>> spaceshipMap;

    void InitializeShapeShipMap(int level, int size);
    void registerMap(int l, int r, int c, char temp);
    void readInput();
};

class RoutingScheme {
public:
    char mode; // 's' or 'q'
    int level;
    int size;
    node startNode;
    node endNode;
    std::vector<std::vector<std::vector<char>>>* Map;
    std::vector<std::vector<std::vector<char>>> visitedPath;

    std::deque<node> container;
    
    void InitializeVisitedPath(int level, int size);
    void containerPush(node CurrentNode);
    void containerPop();
    node containerTop();
    bool addNeighbors(node currentNode);
    bool findhangar();
    void backTracking_Map(std::vector<std::vector<std::vector<char>>>* spaceshipMap);
    void backTracking_List();
};

void outputMap(std::vector<std::vector<std::vector<char>>>* visitedMap, node startNode, int level, int size);
void outputList(std::vector<std::vector<std::vector<char>>>* visitedPath, node startNode, bool solution);


static option long_options[] = {
    {"stack",      no_argument      , nullptr, 's'},
    {"queue",      no_argument      , nullptr, 'q'},
    {"output",     required_argument, nullptr, 'o'},
    {"help",       no_argument,       nullptr, 'h'},
    {nullptr,      0,                 nullptr,  0}
};

char get_optarg_argument_as_char() {
    if (optarg == nullptr) {
        throw "required argument is missing";
    }
    std::string str(optarg); // convert from const char* to std::string.

    // assume the command line will otherwise be correct
    if (str == "M") {
        return 'M';
    } else if (str == "L") {
        return 'L';
    } else {
        // no this case will get error
        throw "Invalid output mode specified";
    }
}

int main(int argc, char** argv) {
    std::ios_base::sync_with_stdio(false); // Turning off Synchronized I/O to speed up
    try
    {
        char output_format = 'M';
        char container_mode;

        // we remember whether we've already set them in these variables:
        bool stack_was_set = false;
        bool queue_was_set = false;


        int choice = 0;
        while ((choice = getopt_long(argc, argv, "sqo:h", long_options, nullptr)) != -1) {
            switch (choice) {
            case 's':
                if (stack_was_set) {
                    throw "Multiple routing modes specified";
                }
                stack_was_set = true;
                break;
            case 'q':
                if (queue_was_set) {
                    throw "Multiple routing modes specified";
                }
                queue_was_set = true;
                break;
            case 'o':
                output_format = get_optarg_argument_as_char();
                break;
            case 'h':
                std::cout <<
                " --stack or -s\n"
                "         Uses stack as the search container and the routing scheme will perform a depth first search (DFS).\n"
                " --queue or -q\n"
                "         Uses queue as the search container and the routing scheme will perform a breadth first search (BFS).\n"
                " --output (M|L) or -o (M|L)\n"
                "         Indicates the output file format by the required argument, M (map format) or L (coordinate list format).\n"
                "         If this option is not provided when running the program, the default output format is map format.\n"
                " --help or -h\n"
                "         Prints this input specification.\n";
                return 0; // return from main with success
            default:
                // unrecognized option
                throw "unrecognized option";
            }
        }

        if (!stack_was_set && !queue_was_set) {
            throw "No routing mode specified";
        }
        if (stack_was_set && queue_was_set) {
            throw "Multiple routing modes specified";
        }
        container_mode = (stack_was_set == true) ? 's' : 'q'; 
        // I have output_format, container_mode

        FileReader inputReader;
        inputReader.readInput();
        // I get inputReader.spaceshipMap, inputReader.startNode, inputReader.level, inputReader.size

        RoutingScheme escape;
        escape.Map = &inputReader.spaceshipMap;
        escape.level = inputReader.level;
        escape.size = inputReader.size;
        escape.mode = container_mode;
        escape.startNode = inputReader.startNode;

        bool solution = escape.findhangar();
        // now I get a back Path from hangar to start
        // reverse to front Path for cout


        if (output_format == 'M') {
            if (solution) {
                escape.backTracking_Map(&inputReader.spaceshipMap);
            }
            outputMap(&inputReader.spaceshipMap, inputReader.startNode, inputReader.level, inputReader.size);
        } else {
            if (solution) {
                escape.backTracking_List();
            }
            // have a data struct easy to print
            outputList(&escape.visitedPath, inputReader.startNode, solution);
        }

    }
    catch(const char* err)
    {
        std::cerr << err << std::endl; // print the error message to std::cerr
        return 1; // returning 1 from main indicates that an error occurred
    }
}

void FileReader::InitializeShapeShipMap(int level, int size) {
    spaceshipMap.resize(level, std::vector<std::vector<char>>(size, std::vector<char>(size, '.')));
}

void FileReader::registerMap(int l, int r, int c, char temp) {
    if (temp != '.' && temp != '#' && temp != 'S' && temp != 'H' && temp != 'E') {
        throw "Invalid map character";
    } else if (temp == 'S') {
        startNode.l = l;
        startNode.r = r;
        startNode.c = c;
    }
    spaceshipMap[l][r][c] = temp;
}

void FileReader::readInput() {
    std::cin >> mode;
    std::cin >> level;
    std::cin >> size;
    
    std::cin.ignore();
    std::string line;
    InitializeShapeShipMap(level, size);
    
    if (mode == 'M') {
        for (int l = 0; l < level; ++l) {
            for (int r = 0; r < size; ++r) {
                char temp;
                std::cin >> temp;
                while (temp == '/') {
                    std::getline(std::cin, line);
                    std::cin >> temp;
                }

                for (int c = 0; c < size; ++c) {
                    registerMap(l, r, c, temp);
                    std::cin.get(temp);
                }
                
            }
        }
    } else if (mode == 'L') {
        char ch;
        while (std::cin >> ch)
        {
            if (ch == '/') {
                std::getline(std::cin, line);
                continue;
            }

            int l, r, c;
            char garbage, temp;
            std::cin >> l >> garbage >> r >> garbage >> c >> garbage >> temp >> garbage;
            // check value, only upper bound
            if (l >= level) {
                throw "Invalid map level";
            } else if (r >= size) {
                throw "Invalid map row";
            } else if (c >= size) {
                throw "Invalid map column";
            }
            registerMap(l, r, c, temp);    
        }
        
    }
}

void RoutingScheme::InitializeVisitedPath(int level, int size) {
    visitedPath.resize(level, std::vector<std::vector<char>>(size, std::vector<char>(size, ' ')));
}

void RoutingScheme::containerPush(node CurrentNode) {
    if (mode == 's') {
        container.push_back(CurrentNode);
    } else if (mode == 'q') {
        container.push_back(CurrentNode);
    }
}

void RoutingScheme::containerPop() {
    if (mode == 's') {
        return container.pop_back();
    } else if (mode == 'q') {
        return container.pop_front();
    }
}

node RoutingScheme::containerTop() {
    if (mode == 's') {
        return container.back();
    } else {
        return container.front();
    }
}


bool RoutingScheme::addNeighbors(node currentNode) {
    // order in nesw and elevtor level from 0 to l-1
    int l = currentNode.l;
    int r = currentNode.r;
    int c = currentNode.c;
    // N
    if (r - 1 >= 0 && (*Map)[l][r - 1][c] != '#' && visitedPath[l][r - 1][c] == ' ') {
        visitedPath[l][r - 1][c] = 'n';
        node tempNode(l, r - 1, c);
        containerPush(tempNode);
        if ((*Map)[l][r - 1][c] == 'H') {
            endNode = tempNode;
            return true;
        }
    }
    // E
    if (c + 1 < size && (*Map)[l][r][c + 1] != '#' && visitedPath[l][r][c + 1] == ' ') {
        visitedPath[l][r][c + 1] = 'e';
        node tempNode(l, r, c + 1);
        containerPush(tempNode);
        if ((*Map)[l][r][c + 1] == 'H') {
            endNode = tempNode;
            return true;
        }
    }
    // S
    if (r + 1 < size && (*Map)[l][r + 1][c] != '#' && visitedPath[l][r + 1][c] == ' ') {
        visitedPath[l][r + 1][c] = 's';
        node tempNode(l, r + 1, c);
        containerPush(tempNode);
        if ((*Map)[l][r + 1][c] == 'H') {
            endNode = tempNode;
            return true;
        }
    }
    // W
    if (c - 1 >= 0 && (*Map)[l][r][c - 1] != '#' && visitedPath[l][r][c - 1] == ' ') {
        visitedPath[l][r][c - 1] = 'w';
        node tempNode(l, r, c - 1);
        containerPush(tempNode);
        if ((*Map)[l][r][c - 1] == 'H') {
            endNode = tempNode;
            return true;
        }
    }
    // Elevtor
    if ((*Map)[l][r][c] == 'E') {
        for (int i = 0; i < level; ++i) {
            if (i == l) continue;
            if ((*Map)[i][r][c] == 'E' && visitedPath[i][r][c] == ' ') {
                visitedPath[i][r][c] = static_cast<char>(l + '0');
                node tempNode(i, r, c);
                containerPush(tempNode);
            }
        }
    }
    return false;
}

bool RoutingScheme::findhangar() {
    container.push_back(startNode);
    InitializeVisitedPath(level, size);
    visitedPath[startNode.l][startNode.r][startNode.c] = 'S';

    while (!container.empty()) {
        node currentNode = containerTop();
        containerPop();
        bool find = addNeighbors(currentNode);
        if (find) return true;
    }
    return false;
}

void RoutingScheme::backTracking_List() {
    // will save mereory
    char pre = 'H';
    int l = endNode.l;
    int r = endNode.r;
    int c = endNode.c;
    while (pre != 'S') {
        switch (visitedPath[l][r][c]) {
        case 'n':
            visitedPath[l][r][c] = pre;
            ++r;
            pre = 'n';
            break;
        case 'e':
            visitedPath[l][r][c] = pre;
            --c;
            pre = 'e';
            break;
        case 's':
            visitedPath[l][r][c] = pre;
            --r;
            pre = 's';
            break;
        case 'w':
            visitedPath[l][r][c] = pre;
            ++c;
            pre = 'w';
            break;
        case 'S':
            visitedPath[l][r][c] = pre;
            pre = 'S';
            break;
        default:
            char temp = visitedPath[l][r][c];
            visitedPath[l][r][c] = pre;
            pre = static_cast<char>(l + '0');
            l = temp - '0';
        }
    }
}

void RoutingScheme::backTracking_Map(std::vector<std::vector<std::vector<char>>>* spaceshipMap) {
    char pre = 'H';
    int l = endNode.l;
    int r = endNode.r;
    int c = endNode.c;
    while (pre != 'S') {
        switch (visitedPath[l][r][c]) {
        case 'n':
            (*spaceshipMap)[l][r][c] = pre;
            ++r;
            pre = 'n';
            break;
        case 'e':
            (*spaceshipMap)[l][r][c] = pre;
            --c;
            pre = 'e';
            break;
        case 's':
            (*spaceshipMap)[l][r][c] = pre;
            --r;
            pre = 's';
            break;
        case 'w':
            (*spaceshipMap)[l][r][c] = pre;
            ++c;
            pre = 'w';
            break;
        case 'S':
            (*spaceshipMap)[l][r][c] = pre;
            pre = 'S';
            break;
        default:
            (*spaceshipMap)[l][r][c] = pre;
            pre = static_cast<char>(l + '0');
            l = visitedPath[l][r][c] - '0';
        }
    }
}

void outputMap(std::vector<std::vector<std::vector<char>>>* visitedMap, node startNode, int level, int size) {
    std::cout << "Start in level " << startNode.l << ", row " << startNode.r << ", column " << startNode.c << '\n';

    for (int l = 0; l < level; ++l) {
        std::cout << "//level " << l << '\n';
        for (int r = 0; r < size; ++r) {
            for (int c = 0; c < size; ++c) {
                std::cout << (*visitedMap)[l][r][c];
            }
            std::cout << '\n';
        }
    }


}

void outputList(std::vector<std::vector<std::vector<char>>>* visitedMap, node startNode, bool solution) {
    int l = startNode.l;
    int r = startNode.r;
    int c = startNode.c;
    std::cout << "//path taken\n";
    if (solution) {
        while ((*visitedMap)[l][r][c] != 'H') {
            std::cout << '(' << l << ',' << r << ',' << c << ',' << (*visitedMap)[l][r][c] << ")\n";
            switch ((*visitedMap)[l][r][c]) {
            case 'n':
                --r;
                break;
            case 'e':
                ++c;
                break;
            case 's':
                ++r;
                break;
            case 'w':
                --c;
                break;
            default:
                l = (*visitedMap)[l][r][c] - '0';
            }
        }
    }
    
}