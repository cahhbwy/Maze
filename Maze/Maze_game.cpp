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
	vector<vector<bool> > board;	//迷宫布局
	unsigned int SIZE_X, SIZE_Y;	//迷宫尺寸，包括外墙的大小，鉴于墙体厚度和道路宽度相同，尺寸为奇数，至少为3
	unsigned int IN_X, IN_Y, OUT_X, OUT_Y;		//入口和出口位置
	vector<Point> leaves;			//生成迷宫过程使用的记录叶子结点的vector
	Point player = Point(0, 0);		//玩家所在位置
	// mode con:cols=120 lines=45	//宽60个中文字符，高45个中文字符
	int BOARD_LEFT, BOARD_RIGHT, BOARD_TOP, BOARD_BOTTOM;	//画面显示的边界

	bool set_door(unsigned int x, unsigned int y, bool flag) {
		if (x == 0) {
			this->board[0][y] = true;
			this->board[1][y] = flag;
			if (flag) {
				leaves.push_back(Point(1, y));
			}
			return true;
		} else if (x == SIZE_X - 1) {
			this->board[x][y] = true;
			this->board[x - 1][y] = flag;
			if (flag) {
				leaves.push_back(Point(x - 1, y));
			}
			return true;
		}
		if (y == 0) {
			this->board[x][0] = true;
			this->board[x][1] = flag;
			if (flag) {
				leaves.push_back(Point(x, 1));
			}
			return true;
		} else if (y == SIZE_Y - 1) {
			this->board[x][y] = true;
			this->board[x][y - 1] = flag;
			if (flag) {
				leaves.push_back(Point(x, y - 1));
			}
			return true;
		}
		return false;
	}
	void setBoard() {
		if (this->SIZE_X > 45) {
			if (this->player.x < 22) {
				this->BOARD_TOP = 0;
				this->BOARD_BOTTOM = 44;
			} else if (this->player.x > this->SIZE_X - 23) {
				this->BOARD_TOP = this->SIZE_X - 45;
				this->BOARD_BOTTOM = this->SIZE_X - 1;
			} else {
				this->BOARD_TOP = this->player.x - 22;
				this->BOARD_BOTTOM = this->player.x + 22;
			}
		} else {
			this->BOARD_TOP = 0;
			this->BOARD_BOTTOM = 44;
		}
		if (this->SIZE_Y > 60) {
			if (this->player.y < 30) {
				this->BOARD_LEFT = 0;
				this->BOARD_RIGHT = 59;
			} else if (this->player.y > this->SIZE_Y - 30) {
				this->BOARD_LEFT = this->SIZE_Y - 60;
				this->BOARD_RIGHT = this->SIZE_Y - 1;
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
			if (x > 1 && !this->board[x - 2][y]) {
				nerbor.push_back(Point(x - 2, y));
			}
			if (x < this->SIZE_X - 2 && !this->board[x + 2][y]) {
				nerbor.push_back(Point(x + 2, y));
			}
			if (y > 1 && !this->board[x][y - 2]) {
				nerbor.push_back(Point(x, y - 2));
			}
			if (y < this->SIZE_Y - 2 && !this->board[x][y + 2]) {
				nerbor.push_back(Point(x, y + 2));
			}
			if (nerbor.empty()) {
				leaves.erase(leaves.begin() + index);
			}
		}
		unsigned int index_nerbor = rand() % nerbor.size();
		unsigned int nx = nerbor[index_nerbor].x, ny = nerbor[index_nerbor].y;
		this->board[nx][ny] = true;
		this->board[(nx + x) / 2][(ny + y) / 2] = true;
		if (nerbor.size() == 1) {
			leaves.erase(leaves.begin() + index);
		}
		leaves.push_back(Point(nx, ny));
	}
	void _draw(HANDLE handle_out, int col, int row, char *s) {
		DWORD bytes = 0;
		COORD coord = { 2 * (row - this->BOARD_LEFT), col - this->BOARD_TOP };
		if (strcmp(s, "●") == 0) {
			FillConsoleOutputAttribute(handle_out, FOREGROUND_RED | FOREGROUND_INTENSITY, 2, coord, &bytes);
		}
		WriteConsoleOutputCharacterA(handle_out, s, 2, coord, &bytes);
	}
	void drawPlayer(HANDLE handle_out) {
		this->_draw(handle_out, player.x, player.y, "●");
	}
public:
	bool initial() {
		return this->initial(51, 51);
	}
	bool initial(unsigned int SIZE_X, unsigned int SIZE_Y) {
		SIZE_X = SIZE_X | 0x01;
		SIZE_Y = SIZE_Y | 0x01;
		return this->initial(SIZE_X, SIZE_Y, 0, 1, SIZE_X - 1, SIZE_Y - 2);
	}
	bool initial(unsigned int SIZE_X, unsigned int SIZE_Y, unsigned int IN_X, unsigned int IN_Y, unsigned int OUT_X, unsigned int OUT_Y) {
		SIZE_X = SIZE_X | 0x01;
		SIZE_Y = SIZE_Y | 0x01;
		if (SIZE_X < 3 || SIZE_Y < 3) {
			cout << "size must be larger than 3" << endl;
			return false;
		}
		this->SIZE_X = SIZE_X;
		this->SIZE_Y = SIZE_Y;
		this->IN_X = IN_X;
		this->IN_Y = IN_Y;
		this->OUT_X = OUT_X;
		this->OUT_Y = OUT_Y;
		this->board.resize(SIZE_X);
		for (unsigned int i = 0; i < SIZE_X; ++i) {
			this->board[i].assign(SIZE_Y, false);
		}
		if (!(this->set_door(IN_X, IN_Y, true) && this->set_door(OUT_X, OUT_Y, false))) {
			return false;
		}
		this->player.x = IN_X;
		this->player.y = IN_Y;
		srand((unsigned int)time(NULL));
		this->generate();
		system("mode con:cols=120 lines=45");
		return true;
	}
	void generate() {
		while (!leaves.empty()) {
			this->growth();
		}
	}
	void show(string filename) {
		ofstream file(filename.c_str(), std::ios::out);
		for (unsigned int i = 0; i < this->SIZE_X; ++i) {
			for (unsigned int j = 0; j < this->SIZE_Y; ++j) {
				file << (this->board[i][j] ? "　" : "■");
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
		FillConsoleOutputAttribute(handle_buf, FOREGROUND_BLUE | FOREGROUND_GREEN, 5400, { 0,0 }, &bytes);
		for (int i = max(0, this->BOARD_TOP); i <= min((int)this->SIZE_X - 1, this->BOARD_BOTTOM); ++i) {
			buffer.clear();
			for (int j = max(0, this->BOARD_LEFT); j <= min((int)this->SIZE_Y - 1, this->BOARD_RIGHT); ++j) {
				buffer += (this->board[i][j] ? "　" : "■");
			}
			while (buffer.size() < 120) {
				buffer += "　";
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
						case 72:
							if (this->player.x > 0 && this->board[this->player.x - 1][this->player.y])
								--this->player.x;
							break;
						case 80:
							if (this->player.x < this->SIZE_X - 1 && this->board[this->player.x + 1][this->player.y])
								++this->player.x;
							break;
						case 75:
							if (this->player.y > 0 && this->board[this->player.x][this->player.y - 1])
								--this->player.y;
							break;
						case 77:
							if (this->player.y < this->SIZE_Y - 1 && this->board[this->player.x][this->player.y + 1])
								++this->player.y;
							break;
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
	cout << "输入迷宫大小(高度 宽度)\n";
	int size_x, size_y;
	cin >> size_x >> size_y;
	game.initial(size_x, size_y);
	game.play();
	//game.show("board.txt");
	return 0;
}
