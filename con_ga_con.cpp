/**
 * tictactoe.cpp: implement AI engine for tictactoe game
 * - Tạo state: vector<vector<char>>
 * - Kiểm tra kết thúc
 * 
 * 
 * 
 * 2. Giao diện người dùng:
 * - Chọn 1 trong 2 chế độ dễ và khó
 * - Chọn X hoặc O
 * - Nhập vị trí ô từng lượt đi
 * - (Các phần trên đã bao gồm kiểm tra nếu nhập sai)
 * - In sau mỗi lượt đi của người và máy
 */ 

#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

typedef vector<vector<char>> State;
const int SIZE = 3;

// In bàn chơi sau từng lượt
void renderGame(const State& state)
{
    cout << "    A   B   C  " << endl;
    cout << "  -------------" << endl;
    for (int row = 0; row < SIZE; row++) {
        cout << row + 1;
        for (int col = 0; col < SIZE; col++) {
            cout << " | " << state[row][col];
        }
        cout << " |" << endl << "  -------------" << endl;
    }
}

// Trả về người chơi tiếp theo
char getNextPlayer(const State& s)
{
    int countX = 0, countO = 0;
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (s[row][col] == 'X') {
                countX++;
            }
            else if (s[row][col] == 'O') {
                countO++;
            }
        }
    }

    if (countX == countO) {
        return 'X';
    }
    else {
        return 'O';
    }
}

// Trả về vector lưu trạng thái mới sau từng lượt
State play(const State& s, const int& row, const int& col, const char& player)
{
    State newState = State(3, vector<char>(3, ' '));
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            newState[row][col] = s[row][col];
        }
    }
    newState[row][col] = player;

    return newState;
}

// Trả về một vector chứa các khả năng có thể xảy ra trong lượt tiếp theo 
vector<State> getNextStates(const State& s)
{
    char nextPlayer = getNextPlayer(s);
    vector<State> states;
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (s[row][col] == ' ') {
                State nextState = play(s, row, col, nextPlayer);
                states.push_back(nextState);
            }
        }
    }

    return states;
}

// Trả về một vector 8 phần tử chứa số lượng X và số O trên từng dòng, cột, đường chéo
vector<vector<int>> countX_O(const State& s)
{
    // Count X(countXO[0]) and O(countXO[1]) in rows and cols
    vector<vector<int>> countXO(2, vector<int>(SIZE * 2 + 2, 0));
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (s[row][col] == 'X') {
                countXO[0][row]++; 
                countXO[0][col + 3]++; 
            }
            if (s[row][col] == 'O') {
                countXO[1][row]++; 
                countXO[1][col + 3]++; 
            }
        }
    }
    // Count X and O in diagonals
    for (int i = 0; i < SIZE; i++) {
        // Principal diagonal
        if (s[i][i] == 'X') {
            countXO[0][SIZE * 2]++;
        }
        if (s[i][i] == 'O') {
            countXO[1][SIZE * 2]++;
        }
        // Secondary diagonal
        if (s[i][SIZE - 1 - i] == 'X') {
            countXO[0][SIZE * 2 + 1]++;
        }
        if (s[i][SIZE - 1 - i] == 'O') {
            countXO[1][SIZE * 2 + 1]++;
        }
    }

    return countXO;
}

// Trả về đúng nếu có người thắng hoặc cả 2 cùng hòa
bool isFinalState(const State& s)
{
    vector<vector<int>> countXO(2, vector<int>(SIZE * 2 + 2, 0));
    countXO = countX_O(s);
    for (int i = 0; i < SIZE * 2 + 2; i++) {
        if (countXO[0][i] == 3 || countXO[1][i] == 3) {
            return true;
        }
    }

    for (int i = 0; i < SIZE * 2 + 2; i++) {
        if (countXO[0][i] == 0 || countXO[1][i] == 0) {
            return false;
        }
    }

    return true;
}

/* Trả về số điểm : Nếu X thắng: 1
 *                  Nếu O thắng: -1
 *                  Nếu hòa: 0
 */      
int getScoreFinalState(const State& s)
{
    vector<vector<int>> countXO(2, vector<int>(SIZE * 2 + 2, 0));
    countXO = countX_O(s);
    for (int i = 0; i < SIZE * 2 + 2; i++) {
        if (countXO[0][i] == 3) {
            return 1;
        }
        if (countXO[1][i] == 3) {
            return -1;
        }
    }

    return 0;
}

/**
 * Nếu kết thúc, trả về số điểm và trạng thái tương ứng
 * 
 * Hàm sẽ tính điêm của một trạng thái bằng cách:
 * Nếu lượt X thì sẽ lấy điểm cao nhất của các trạng thái tiếp theo từ trạng thái này
 * Nếu lượt O thì sẽ lấy điểm thấp nhất của các trạng thái tiếp theo từ trạng thái này
 * 
 * Cuối cùng trả về trạng thái tối ưu nhất cho lượt tiếp theo
 */
pair<int, State> getScore(const State& s)
{
    if (isFinalState(s)) {
        return make_pair(getScoreFinalState(s), s);
    }

    char player = getNextPlayer(s);
    vector<State> states = getNextStates(s);
    int bestScore = -10;
    State bestState;
    for (State ns : states) {
        pair<int, State> p = getScore(ns);
        int score = p.first;
        if (player == 'X') {
            if (bestScore < score || bestScore == -10) {
                bestScore = score;
                bestState = ns;
                if (bestScore == 1) {
                    break;
                }
            }
        }
        else {
            if (bestScore > score || bestScore == -10) {
                bestScore = score;
                bestState = ns;
                if (bestScore == -1) {
                    break;
                }
            }
        }
    }

    return make_pair(bestScore, bestState);
}

pair<int, int> randomCellAddress()
{
    srand(time(NULL));
    int row = (rand() % 3);
    int col = (rand() % 3);
    return make_pair(row, col);
}

/**
 * Chon ngẫu nhiên một địa chỉ trong lượt đầu nếu máy là X
 * (do bắt đầu bằng ô nào không quan trọng)
 * 
 * Với chế độ dễ, máy sẽ đi thêm một lượt ngẫu nhiên.
 * 
 * Còn lại sẽ chọn cách đi tốt nhất dựa vào hàm getScore(const State&)
 */ 
pair<int, int> getComputerPlay(const State& s, const int& numberOfHumanPlay, const int& mode)
{
    if ((mode == 1) && (numberOfHumanPlay == 1)) {
        while (true) {
            pair<int, int> w = randomCellAddress();
            int row = w.first;
            int col = w.second;
            if (s[row][col] == ' ') {
                return make_pair(row, col);
            }
        }
    }

    if (numberOfHumanPlay == 0) {
        return randomCellAddress();
    }

    pair<int, State> p = getScore(s);
    int score = p.first;
    State newState = p.second;
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (newState[row][col] != s[row][col]) {
                return make_pair(row, col);
            }

        }
    }

    return make_pair(-1, -1);
}

void fix()
{
    cin.clear();
    cin.ignore(32767, '\n');
}

int selectMode()
{
    int mode = 0;
    do {
        cout << "Select mode (1 : Easy, 2 : Difficult): ";
        cin >> mode;
        if (cin.fail()) {
            fix();
        }
    } while ((1 > mode) || (2 < mode));

    return mode;
}

char selectPlayer()
{
    char humanPlayer = ' ';
    while (true) {
        cout << "Choose your player (X or O): ";
        cin >> humanPlayer;
        if (('X' == humanPlayer) || ('x' == humanPlayer)) {
            humanPlayer = 'X';
            break;
        }
        if (('O' == humanPlayer) || ('o' == humanPlayer)) {
            humanPlayer = 'O';
            break;
        }
    }

    return humanPlayer;
}

pair<int, int> enterCellAddress(const State& s)
{
    int row = -1, col = -1;
    char a;
    do {
        do {
            cout << "Enter your play: ";
            cin >> a >> row;
            row = --row;
        
            if (('A' == a) || ('a' == a)) {
                col = 0;
            }
            if (('B' == a) || ('b' == a)) {
                col = 1;
            }
            if (('C' == a) || ('c' == a)) {
                col = 2;
            }

            if (cin.fail()) {
                fix();
            }
        } while ((0 > row) || (row > 2) || (0 > col) || (col > 2));
    } while (s[row][col] != ' ');

    return make_pair(row, col);
}

void printResult(const int& score, const char& humanPlayer)
{
    if (score > 0) {
        if ('X' == humanPlayer) {
            cout << "You won" << endl;
        }
        else {
            cout << "You lost" << endl;
        }
    }
    else if (score < 0) {
        if ('O' == humanPlayer) {
            cout << "You won" << endl;
        }
        else {
            cout << "You lost" << endl;
        }
    }
    else {
        cout << "A draw" << endl;
    }
}

int main()
{
    system("color 0a");
    State s = State(3, vector<char>(3, ' '));

    int mode = selectMode();
    char currentPlayer = 'X';
    char humanPlayer = selectPlayer();
    cout << "You are " << humanPlayer << " player" << endl << endl;

    renderGame(s);
    while (true)
    {
        int row = -1, col = -1;
        static int numberOfHumanPlay = 0;
        if (humanPlayer == currentPlayer) {
            pair<int, int> cellAddress = enterCellAddress(s);
            row = cellAddress.first;
            col = cellAddress.second;
            numberOfHumanPlay++;
            system("cls");
            cout << "You" << "(" << humanPlayer << ") " << "play " << char(col + 65) << row + 1 << endl << endl;
        }
        else
        {
            pair<int, int> move = getComputerPlay(s, numberOfHumanPlay, mode);
            row = move.first;
            col = move.second;
            cout << "I" << "(" << currentPlayer << ") " << "play " << char(col + 65) << row + 1 << endl << endl;
        }
        s = play(s, row, col, currentPlayer);
        renderGame(s);

        if (isFinalState(s)) {
            int score = getScoreFinalState(s);
            printResult(score, humanPlayer);
            break;
        }
        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
    }

    return 0;
}
