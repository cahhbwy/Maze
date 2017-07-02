#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <ctime>
#include <Windows.h>
#include <conio.h>

using namespace std;
typedef struct Point {
	int x;
	int y;
	Point(int x, int y) :x(x), y(y) {}
}Point;
class Maze {
private:
	vector<vector<bool> > board;	//迷宫布局
	int SIZE_X, SIZE_Y;	//迷宫尺寸，包括外墙的大小，鉴于墙体厚度和道路宽度相同，尺寸为奇数，至少为3
	int IN_X, IN_Y, OUT_X, OUT_Y;		//入口和出口位置
	vector<Point> leaves;			//生成迷宫过程使用的记录叶子结点的vector
	Point player = Point(0, 0);		//玩家所在位置
	vector<Point> path;
	// mode con:cols=120 lines=45	//宽60个中文字符，高45个中文字符
	int BORDER_LEFT, BORDER_RIGHT, BORDER_TOP, BORDER_BOTTOM;	//画面显示的边界
	HANDLE handle_buf0, handle_buf1;
	int bufferIndex = 0;

	bool set_door(int x, int y, bool flag) {
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
	void setBorder() {
		if (this->SIZE_X > 45) {
			if (this->player.x < 22) {
				this->BORDER_TOP = 0;
				this->BORDER_BOTTOM = 44;
			} else if (this->player.x > this->SIZE_X - 23) {
				this->BORDER_TOP = this->SIZE_X - 45;
				this->BORDER_BOTTOM = this->SIZE_X - 1;
			} else {
				this->BORDER_TOP = this->player.x - 22;
				this->BORDER_BOTTOM = this->player.x + 22;
			}
		} else {
			this->BORDER_TOP = 0;
			this->BORDER_BOTTOM = 44;
		}
		if (this->SIZE_Y > 60) {
			if (this->player.y < 30) {
				this->BORDER_LEFT = 0;
				this->BORDER_RIGHT = 59;
			} else if (this->player.y > this->SIZE_Y - 30) {
				this->BORDER_LEFT = this->SIZE_Y - 60;
				this->BORDER_RIGHT = this->SIZE_Y - 1;
			} else {
				this->BORDER_LEFT = this->player.y - 30;
				this->BORDER_RIGHT = this->player.y + 29;
			}
		} else {
			this->BORDER_LEFT = 0;
			this->BORDER_RIGHT = 60;
		}
	}
	void growth() {
		int index = -1;
		vector<Point> nerbor;
		int x, y;
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
		int index_nerbor = rand() % nerbor.size();
		int nx = nerbor[index_nerbor].x, ny = nerbor[index_nerbor].y;
		this->board[nx][ny] = true;
		this->board[(nx + x) / 2][(ny + y) / 2] = true;
		if (nerbor.size() == 1) {
			leaves.erase(leaves.begin() + index);
		}
		leaves.push_back(Point(nx, ny));
	}
	void _draw(HANDLE handle_out, int col, int row, char *s) {
		DWORD bytes = 0;
		COORD coord = { (short)(2 * (row - this->BORDER_LEFT)), (short)(col - this->BORDER_TOP) };
		if (strcmp(s, "●") == 0) {
			FillConsoleOutputAttribute(handle_out, FOREGROUND_RED | FOREGROUND_INTENSITY, 2, coord, &bytes);
		}
		WriteConsoleOutputCharacterA(handle_out, s, 2, coord, &bytes);
	}
	Point findNotExploredNerbor(Point p, vector<vector<bool> > flag) {
		int x = p.x, y = p.y;
		vector<Point> nerbor;
		if (x > 0 && this->board[x - 1][y] && !flag[x - 1][y]) {
			nerbor.push_back(Point(x - 1, y));
		}
		if (y > 0 && this->board[x][y - 1] && !flag[x][y - 1]) {
			nerbor.push_back(Point(x, y - 1));
		}
		if (x < this->SIZE_X - 1 && this->board[x + 1][y] && !flag[x + 1][y]) {
			nerbor.push_back(Point(x + 1, y));
		}
		if (y < this->SIZE_Y - 1 && this->board[x][y + 1] && !flag[x][y + 1]) {
			nerbor.push_back(Point(x, y + 1));
		}
		if (nerbor.empty()) {
			return Point(-1, -1);
		}
		int minDis = this->SIZE_X + this->SIZE_Y, dis;
		int index = 0;
		for (int i = 0; i < nerbor.size(); ++i) {
			dis = abs(nerbor[i].x - this->player.x) + abs(nerbor[i].y - this->player.y);
			if (dis < minDis) {
				index = i;
				minDis = dis;
			}
		}
		return Point(nerbor[index].x, nerbor[index].y);
	}
	void help() {
		vector<vector<bool> >flag(this->SIZE_X);
		for (int i = 0; i < SIZE_X; ++i) {
			flag[i].assign(this->SIZE_Y, false);
		}
		this->path.clear();
		this->path.push_back(Point(this->OUT_X, this->OUT_Y));
		this->path.push_back(findNotExploredNerbor(this->path.back(), flag));
		flag[this->OUT_X][this->OUT_Y] = true;
		flag[path.back().x][path.back().y] = true;
		bool back = false;
		while (!this->path.empty()) {
			if (!back) {
				Point current = this->path.back();
				Point nerbor = findNotExploredNerbor(current, flag);
				if (nerbor.x == 0xFFFFFFFF) {
					back = true;
				} else {
					this->path.push_back(nerbor);
					flag[nerbor.x][nerbor.y] = true;
				}
			} else {
				this->path.pop_back();
				back = false;
			}
			if (this->path.back().x == this->player.x && this->path.back().y == this->player.y) {
				break;
			}
		}
	}
	void drawPath(HANDLE handle_out) {
		DWORD bytes = 0;
		for (int i = 0; i < path.size() - 1; ++i) {
			COORD coord = { (short)(2 * (this->path[i].y - this->BORDER_LEFT)), (short)(this->path[i].x - this->BORDER_TOP) };
			FillConsoleOutputAttribute(handle_out, FOREGROUND_GREEN | FOREGROUND_INTENSITY, 2, coord, &bytes);
			WriteConsoleOutputCharacterA(handle_out, "◆", 2, coord, &bytes);
		}
	}
	bool getControl() {
		int ch = _getch();
		if ((ch | 0x20) == 'h') {
			help();
		} else if ((ch | 0x20) == 'r') {
			return false;
		} else if (ch != 0x1B) {
			switch (ch) {
				case 0xE0:
					switch (ch = _getch()) {
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
			exit(0);
		}
		return true;
	}
	void drawPlayer(HANDLE handle_out) {
		this->_draw(handle_out, player.x, player.y, "●");
	}
	void doubleBuffer() {
		handle_buf0 = CreateConsoleScreenBuffer(
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			CONSOLE_TEXTMODE_BUFFER,
			NULL
		);
		handle_buf1 = CreateConsoleScreenBuffer(
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			CONSOLE_TEXTMODE_BUFFER,
			NULL
		);
	}
	void show() {
		HANDLE handle_buf;
		if (this->bufferIndex == 0) {
			handle_buf = handle_buf0;
		} else {
			handle_buf = handle_buf1;
		}
		COORD coord = { 0,0 };
		DWORD bytes = 0;
		string buffer;
		this->setBorder();
		FillConsoleOutputAttribute(handle_buf, FOREGROUND_BLUE | FOREGROUND_GREEN, 5400, { 0,0 }, &bytes);
		for (int i = max(0, this->BORDER_TOP); i <= min(this->SIZE_X - 1, this->BORDER_BOTTOM); ++i) {
			buffer.clear();
			for (int j = max(0, this->BORDER_LEFT); j <= min(this->SIZE_Y - 1, this->BORDER_RIGHT); ++j) {
				buffer += (this->board[i][j] ? "　" : "■");
			}
			while (buffer.size() < 120) {
				buffer += "　";
			}
			WriteConsoleOutputCharacterA(handle_buf, buffer.c_str(), 120, { 0,(short)(i - max(0, this->BORDER_TOP)) }, &bytes);
		}
		this->drawPlayer(handle_buf);
		if (!this->path.empty()) {
			this->drawPath(handle_buf);
			this->path.clear();
		}
		SetConsoleActiveScreenBuffer(handle_buf);
		this->bufferIndex ^= 0x01;
		if (this->player.x == this->OUT_X && this->player.y == this->OUT_Y) {
			for (short i = 18; i < 25; ++i) {
				FillConsoleOutputAttribute(handle_buf, FOREGROUND_RED | FOREGROUND_INTENSITY, 60, { 30,i }, &bytes);
			}
			WriteConsoleOutputCharacterA(handle_buf, "                                                            ", 60, { 30,18 }, &bytes);
			WriteConsoleOutputCharacterA(handle_buf, "   m     m  mmmm  m    m       m     m mmmmm  mm   m   m    ", 60, { 30,19 }, &bytes);
			WriteConsoleOutputCharacterA(handle_buf, "    'm m'  m'  'm #    #       #  #  #   #    #'m  #   #    ", 60, { 30,20 }, &bytes);
			WriteConsoleOutputCharacterA(handle_buf, "     '#'   #    # #    #       ' #'# #   #    # #m #   #    ", 60, { 30,21 }, &bytes);
			WriteConsoleOutputCharacterA(handle_buf, "      #    #    # #    #        ## ##'   #    #  # #   '    ", 60, { 30,22 }, &bytes);
			WriteConsoleOutputCharacterA(handle_buf, "      #     #mm#  'mmmm'        #   #  mm#mm  #   ##   #    ", 60, { 30,23 }, &bytes);
			WriteConsoleOutputCharacterA(handle_buf, "                      按 r 键重新开始                       ", 60, { 30,24 }, &bytes);
		}
	}
	void generate() {
		while (!leaves.empty()) {
			this->growth();
		}
		int k = this->SIZE_X *this->SIZE_Y / 100;
		while (--k > 0) {
			int x = rand() % (this->SIZE_X - 2) + 1, y = rand() % (this->SIZE_Y - 2) + 1;
			this->board[x][y] = true;
		}
	}
public:
	bool initial() {
		return this->initial(51, 51);
	}
	bool initial(int SIZE_X, int SIZE_Y) {
		SIZE_X = SIZE_X | 0x01;
		SIZE_Y = SIZE_Y | 0x01;
		return this->initial(SIZE_X, SIZE_Y, 0, 1, SIZE_X - 1, SIZE_Y - 2);
	}
	bool initial(int SIZE_X, int SIZE_Y, int IN_X, int IN_Y, int OUT_X, int OUT_Y) {
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
		for (int i = 0; i < SIZE_X; ++i) {
			this->board[i].assign(SIZE_Y, false);
		}
		if (!(this->set_door(OUT_X, OUT_Y, false) && this->set_door(IN_X, IN_Y, true))) {
			return false;
		}
		this->player.x = IN_X;
		this->player.y = IN_Y;
		srand((int)time(NULL));
		this->generate();
		return true;
	}
	void show(string filename) {
		ofstream file(filename.c_str(), std::ios::out);
		for (int i = 0; i < this->SIZE_X; ++i) {
			for (int j = 0; j < this->SIZE_Y; ++j) {
				file << (this->board[i][j] ? "　" : "■");
			}
			file << endl;
		}
		file.close();
	}
	void play() {
		bool bufIndex = false;
		bool win = false;
		do {
			this->show();
		} while (this->getControl());
	}
	void welcome() {
		system("mode con:cols=120 lines=45");
		CONSOLE_CURSOR_INFO cci;
		cci.bVisible = 1;
		cci.dwSize = 0;
		this->doubleBuffer();
		HANDLE handle_buf;
		if (this->bufferIndex == 0) {
			handle_buf = handle_buf0;
		} else {
			handle_buf = handle_buf1;
		}
		SetConsoleCursorInfo(handle_buf, &cci);
		DWORD bytes;
		for (short i = 4; i < 11; ++i) {
			FillConsoleOutputAttribute(handle_buf, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, 36, { 42,i }, &bytes);
		}
		WriteConsoleOutputCharacterA(handle_buf, " m    m   mm   mmmmmm mmmmmm ", 29, { 46,5 }, &bytes);
		WriteConsoleOutputCharacterA(handle_buf, " ##  ##   ##       #' #      ", 29, { 46,6 }, &bytes);
		WriteConsoleOutputCharacterA(handle_buf, " # ## #  #  #    m#   #mmmmm ", 29, { 46,7 }, &bytes);
		WriteConsoleOutputCharacterA(handle_buf, " # '' #  #mm#   m'    #      ", 29, { 46,8 }, &bytes);
		WriteConsoleOutputCharacterA(handle_buf, " #    # #    # ##mmmm #mmmmm ", 29, { 46,9 }, &bytes);

		for (short i = 14; i < 22; ++i) {
			FillConsoleOutputAttribute(handle_buf, BACKGROUND_INTENSITY, 60, { 31,i }, &bytes);
		}
		for (short i = 13; i < 21; ++i) {
			FillConsoleOutputAttribute(handle_buf, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY | BACKGROUND_BLUE, 60, { 30,i }, &bytes);
		}
		WriteConsoleOutputCharacterA(handle_buf, "******************************", 30, { 45,14 }, &bytes);
		WriteConsoleOutputCharacterA(handle_buf, "          方向键控制          ", 30, { 45,15 }, &bytes);
		WriteConsoleOutputCharacterA(handle_buf, "          ESC 键退出           ", 30, { 45,16 }, &bytes);
		WriteConsoleOutputCharacterA(handle_buf, "          按 h 键提示         ", 30, { 45,17 }, &bytes);
		WriteConsoleOutputCharacterA(handle_buf, "        按 r 键重开一局       ", 30, { 45,18 }, &bytes);
		WriteConsoleOutputCharacterA(handle_buf, "******************************", 30, { 45,19 }, &bytes);

		for (short i = 23; i < 27; ++i) {
			FillConsoleOutputAttribute(handle_buf, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY | BACKGROUND_GREEN, 40, { 40, i }, &bytes);
		}
		WriteConsoleOutputCharacterA(handle_buf, "输入迷宫的高度和宽度，如：99 99", 31, { 45,24 }, &bytes);
		SetConsoleCursorPosition(handle_buf, { 55, 25 });
		SetConsoleActiveScreenBuffer(handle_buf);
		this->bufferIndex ^= 0x01;

		int size_x, size_y;
		cin >> size_x >> size_y;
		this->initial(size_x, size_y);
		for (short i = 0; i < 45; ++i) {
			FillConsoleOutputAttribute(handle_buf, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, 120, { 0,i }, &bytes);
			WriteConsoleOutputCharacterA(handle_buf, "                                                                                                                       ", 120, { 0,i }, &bytes);
		}
		cci.bVisible = 0;
		cci.dwSize = 1;
		SetConsoleCursorInfo(handle_buf0, &cci);
		SetConsoleCursorInfo(handle_buf1, &cci);
	}
};
int main(int argc, char** argv) {
	Maze game;
	while (true) {
		game.welcome();
		game.play();
	}
	//game.show("board.txt");
	return 0;
}
