#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>

using namespace std;
typedef struct Point {
	unsigned int x;
	unsigned int y;
	Point(unsigned int x, unsigned int y) :x(x), y(y) {}
}Point;
class Maze {
private:
	vector<vector<bool> > board;
	unsigned int SIZE_X, SIZE_Y;
	unsigned int IN_X, IN_Y, OUT_X, OUT_Y;
	vector<Point> leaves;
	bool set_door(unsigned int x, unsigned int y) {
		if (x == 0) {
			this->board[0][y * 2 + 1] = true;
			this->board[1][y * 2 + 1] = true;
			return true;
		} else if (x == SIZE_X - 1) {
			this->board[SIZE_X * 2][y * 2 + 1] = true;
			this->board[SIZE_X * 2 - 1][y * 2 + 1] = true;
			return true;
		}
		if (y == 0) {
			this->board[x * 2 + 1][0] = true;
			this->board[x * 2 + 1][1] = true;
			return true;
		} else if (y == SIZE_Y - 1) {
			this->board[x * 2 + 1][SIZE_Y * 2] = true;
			this->board[x * 2 + 1][SIZE_Y * 2 - 1] = true;
			return true;
		}
		return false;

	}
	void setStatus(unsigned int x, unsigned int y, bool flag) {
		this->board[x * 2 + 1][y * 2 + 1] = flag;
	}
	bool getStatus(unsigned int x, unsigned int y) {
		return this->board[x * 2 + 1][y * 2 + 1];
	}
	void growth() {
		unsigned int index = -1;
		vector<Point> nerbor;
		unsigned int x, y;
		while (nerbor.empty()) {
			if (leaves.empty()) {
				return;
			}
			index = rand() % this->leaves.size();
			x = this->leaves[index].x;
			y = this->leaves[index].y;
			if (x > 0 && !this->getStatus(x - 1, y)) {
				nerbor.push_back(Point(x - 1, y));
			}
			if (x < this->SIZE_X - 1 && !this->getStatus(x + 1, y)) {
				nerbor.push_back(Point(x + 1, y));
			}
			if (y > 0 && !this->getStatus(x, y - 1)) {
				nerbor.push_back(Point(x, y - 1));
			}
			if (y < this->SIZE_Y - 1 && !this->getStatus(x, y + 1)) {
				nerbor.push_back(Point(x, y + 1));
			}
			if (nerbor.empty()) {
				leaves.erase(leaves.begin() + index);
			}
		}
		unsigned int index_nerbor = rand() % nerbor.size();
		unsigned int nx = nerbor[index_nerbor].x, ny = nerbor[index_nerbor].y;
		this->setStatus(nx, ny, true);
		this->board[nx + x + 1][ny + y + 1] = true;
		if (nerbor.size() == 1) {
			leaves.erase(leaves.begin() + index);
		}
		leaves.push_back(Point(nx, ny));
	}
public:
	bool initial() {
		return this->initial(100, 100);
	}
	bool initial(unsigned int SIZE_X, unsigned int SIZE_Y) {
		return this->initial(SIZE_X, SIZE_Y, 0, 0, SIZE_X - 1, SIZE_Y - 1);
	}
	bool initial(unsigned int SIZE_X, unsigned int SIZE_Y, unsigned int IN_X, unsigned int IN_Y, unsigned int OUT_X, unsigned int OUT_Y) {
		this->SIZE_X = SIZE_X;
		this->SIZE_Y = SIZE_Y;
		this->IN_X = IN_X;
		this->IN_Y = IN_Y;
		this->OUT_X = OUT_X;
		this->OUT_Y = OUT_Y;
		this->board.resize(SIZE_X * 2 + 1);
		for (unsigned int i = 0; i < SIZE_X * 2 + 1; ++i) {
			this->board[i].assign(SIZE_Y * 2 + 1, false);
		}
		if (!(this->set_door(IN_X, IN_Y) && this->set_door(OUT_X, OUT_Y))) {
			return false;
		}
		this->setStatus(OUT_X, OUT_Y, false);
		leaves.push_back(Point(IN_X, IN_Y));
		srand((unsigned int)time(NULL));
		this->generate();
		return true;
	}
	void generate() {
		while (!leaves.empty()) {
			this->growth();
		}
	}
	void show(string filename) {
		ofstream file(filename, ios::out);
		for (unsigned int i = 0; i < this->SIZE_X * 2 + 1; ++i) {
			for (unsigned int j = 0; j < this->SIZE_Y * 2 + 1; ++j) {
				file << (this->board[i][j] ? "  " : "¡ö");
			}
			file << endl;
		}
		file.close();
	}
	void show() {
		for (unsigned int i = 0; i < this->SIZE_X * 2 + 1; ++i) {
			for (unsigned int j = 0; j < this->SIZE_Y * 2 + 1; ++j) {
				cout << (this->board[i][j] ? "  " : "¡ö");
			}
			cout << endl;
		}
	}
};
int main(int argc, char** argv) {
	Maze game;
	game.initial(25, 25);
	game.show();
	system("pause");
	return 0;
}