#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <ctime>
#include <Windows.h>
#include <conio.h>

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
	Point player = Point(0, 0);
	int BOARD_LEFT, BOARD_RIGHT, BOARD_TOP, BOARD_BOTTOM;

	bool set_door(unsigned int x, unsigned int y, bool flag) {
		if (x == 0) {
			this->board[0][y * 2 + 1] = true;
			this->setStatus(x, y, flag);
			if (flag) {
				player.x = 0;
				player.y = y * 2 + 1;
			}
			return true;
		} else if (x == SIZE_X - 1) {
			this->board[SIZE_X * 2][y * 2 + 1] = true;
			this->setStatus(x, y, flag);
			if (flag) {
				player.x = SIZE_X * 2;
				player.y = y * 2 + 1;
			}
			return true;
		}
		if (y == 0) {
			this->board[x * 2 + 1][0] = true;
			this->setStatus(x, y, flag);
			if (flag) {
				player.x = x * 2 + 1;
				player.y = 0;
			}
			return true;
		} else if (y == SIZE_Y - 1) {
			this->board[x * 2 + 1][SIZE_Y * 2] = true;
			this->setStatus(x, y, flag);
			if (flag) {
				player.x = x * 2 + 1;
				player.y = SIZE_Y * 2;
			}
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
	void setBoard() {
		if (this->SIZE_X >= 15) {
			if (this->player.x < 15) {
				this->BOARD_TOP = 0;
				this->BOARD_BOTTOM = 29;
			} else if (this->player.x > this->SIZE_X * 2 - 15) {
				this->BOARD_BOTTOM = SIZE_X * 2;
				this->BOARD_TOP = SIZE_X * 2 - 29;
			} else {
				this->BOARD_TOP = this->player.x - 15;
				this->BOARD_BOTTOM = this->player.x + 14;
			}
		} else {
			this->BOARD_TOP = 0;
			this->BOARD_BOTTOM = 30;
		}
		if (this->SIZE_Y >= 30) {
			if (this->player.y < 30) {
				this->BOARD_LEFT = 0;
				this->BOARD_RIGHT = 59;
			} else if (this->player.y > this->SIZE_Y * 2 - 30) {
				this->BOARD_RIGHT = SIZE_Y * 2;
				this->BOARD_LEFT = SIZE_Y * 2 - 59;
			} else {
				this->BOARD_LEFT = this->player.y - 30;
				this->BOARD_RIGHT = this->player.y + 29;
			}
		} else {
			this->BOARD_LEFT = 0;
			this->BOARD_RIGHT = 60;
		}
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
	void _draw(HANDLE handle_out, int x, int y, char *s) {
		DWORD bytes = 0;
		COORD coord = { 2 * (y - this->BOARD_LEFT), x - this->BOARD_TOP };
		if (strcmp(s, "¡ñ") == 0) {
			FillConsoleOutputAttribute(handle_out, FOREGROUND_RED | FOREGROUND_INTENSITY, 2, coord, &bytes);
		}
		WriteConsoleOutputCharacterA(handle_out, s, 2, coord, &bytes);
	}
	void drawPlayer(HANDLE handle_out) {
		this->_draw(handle_out, player.x, player.y, "¡ñ");
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
		if (!(this->set_door(IN_X, IN_Y, true) && this->set_door(OUT_X, OUT_Y, false))) {
			return false;
		}
		leaves.push_back(Point(IN_X, IN_Y));
		srand((unsigned int)time(NULL));
		this->generate();
		system("mode con:cols=120 lines=30");
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
				file << (this->board[i][j] ? "¡¡" : "¡ö");
			}
			file << endl;
		}
		file.close();
	}
	void doubleBuffer(HANDLE &handle_buf1, HANDLE &handle_buf2) {
		handle_buf1 = CreateConsoleScreenBuffer(
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			CONSOLE_TEXTMODE_BUFFER,
			NULL
		);
		handle_buf2 = CreateConsoleScreenBuffer(
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			CONSOLE_TEXTMODE_BUFFER,
			NULL
		);
		CONSOLE_CURSOR_INFO cci;
		cci.bVisible = 0;
		cci.dwSize = 1;
		SetConsoleCursorInfo(handle_buf1, &cci);
		SetConsoleCursorInfo(handle_buf2, &cci);
	}
	void show(HANDLE &handle_buf1, HANDLE &handle_buf2, bool &bufindex) {
		HANDLE handle_buf;
		if (bufindex) {
			handle_buf = handle_buf1;
		} else {
			handle_buf = handle_buf2;
		}
		COORD coord = { 0,0 };
		DWORD bytes = 0;
		string buffer;
		this->setBoard();
		FillConsoleOutputAttribute(handle_buf, FOREGROUND_BLUE | FOREGROUND_GREEN, 3600, { 0,0 }, &bytes);
		for (int i = max(0, this->BOARD_TOP); i <= min(this->SIZE_X * 2, this->BOARD_BOTTOM); ++i) {
			buffer.clear();
			for (int j = max(0, this->BOARD_LEFT); j <= min(this->SIZE_Y * 2, this->BOARD_RIGHT); ++j) {
				buffer += (this->board[i][j] ? "¡¡" : "¡ö");
			}
			while (buffer.size() < 120) {
				buffer += "¡¡";
			}
			WriteConsoleOutputCharacterA(handle_buf, buffer.c_str(), 120, { 0,(short)(i - max(0, this->BOARD_TOP)) }, &bytes);
		}
		this->drawPlayer(handle_buf);
		SetConsoleActiveScreenBuffer(handle_buf);
		bufindex = !bufindex;
	}
	bool getControl() {
		int ch;
		if ((ch = getch()) != 0x1B) {
			switch (ch) {
				case 0xE0:
					switch (ch = getch()) {
						case 72:if (this->player.x > 0 && this->board[this->player.x - 1][this->player.y])--this->player.x; break;
						case 80:if (this->player.x < this->SIZE_X * 2 && this->board[this->player.x + 1][this->player.y])++this->player.x; break;
						case 75:if (this->player.y > 0 && this->board[this->player.x][this->player.y - 1])--this->player.y; break;
						case 77:if (this->player.y < this->SIZE_Y * 2 && this->board[this->player.x][this->player.y + 1])++this->player.y; break;
						default:
							break;
					}
				default:
					break;
			}
		} else {
			return false;
		}
		return true;
	}
	void play() {
		HANDLE handle_out, handle_buf;
		bool bufIndex = false;
		this->doubleBuffer(handle_out, handle_buf);
		do {
			this->show(handle_out, handle_buf, bufIndex);
		} while (this->getControl());
	}
};
int main(int argc, char** argv) {
	Maze game;
	game.initial(30, 30);
	game.play();
	//game.show("board.txt");
	return 0;
}