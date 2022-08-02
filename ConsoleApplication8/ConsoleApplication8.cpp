#define color(x) SetConsoleTextAttribute(hConsole, (WORD) ((x << 4) | 0))//скрипт для замены цвета заднего фона
#include <iostream>
#include <time.h>
#include <windows.h>
#include <conio.h>
using namespace std;
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
struct block {
	int block_size;
	int** figure;
};
enum ConsoleColor {
	Black = 0,
	Blue = 1,
	Green = 2,
	Cyan = 3,
	Red = 4,
	Magenta = 5,
	Brown = 6,
	LightGray = 7,
	DarkGray = 8,
	LightBlue = 9,
	LightGreen = 10,
	LightCyan = 11,
	LightRed = 12,
	LightMagenta = 13,
	Yellow = 14,
	White = 15,
	Up = 72,
	Left = 75,
	Right = 77,
	Down = 80,
};
void printStuff() {//функция для перевода курсора в нулевую позицию быстрее, чем clear("cls"), нашел на просторах инета
	HANDLE hd = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD cd;
	cd.X = 0;
	cd.Y = 0;
	SetConsoleCursorPosition(hd, cd);
}
void init_field(int**& field, int vertical, int horizontal);
void screen(int** field, int vertical, int horizontal, block current, int x, int y);
block create();
void merge(int** field, int vertical, int horizontal, block current, int x, int y);
void figure_print(block x) {
	color(LightMagenta);
	for (int i = 0; i < x.block_size + 2; i++) {
		cout << "  ";
	}
	color(Black);
	cout << endl;
	for (int i = 0; i < x.block_size; i++) {
		color(LightMagenta);
		cout << "  ";
		for (int j = 0; j < x.block_size; j++) {
			color(x.figure[i][j]);
			cout << "  ";
		}
		color(LightMagenta);
		cout << "  " << endl;
	}
	for (int i = 0; i < x.block_size + 2; i++) {
		color(LightMagenta);
		cout << "  ";
		color(Black);
	}
}
bool collision(int** field, int vertical, int horizontal, block current, int x, int y);
bool rotate(int** field, int vertical, int horizontal, block& current, int x, int y);
void full_line(int** field, int vertical, int horizontal);
void delete_line(int** field, int vertical, int horizontal, int line);
int main() {
	setlocale(LC_ALL, "rus");
	srand(time(NULL));
	int vertical = 24;//количество столбцов
	int horizontal = 14;//количество строк
	int** field;//игровое поле
	int difficulty = 0;
	init_field(field, vertical, horizontal);//инициализируем игровое поле
	int x = 0;
	int y = horizontal - 1;
	int key;
	int timer = 1000;//таймер игры
	block current = create();//создаем текущую фигурку
	block next = create();//создаем следующую фигурку
	while (true) {
		while (timer < (1000 - 10 * difficulty)) {
			if (_kbhit()) {//если была нажата клавиша
				_getch();
				key = _getch();//забираем ее код в переменную
				switch (key) {
				case Left:
					if (collision(field, vertical, horizontal, current, x, y - 1)) {
						y--;
						printStuff();
						screen(field, vertical, horizontal, current, x, y);
						break;
					}
					else break;
				case Right:
					if (collision(field, vertical, horizontal, current, x, y + 1)) {
						y++;
						printStuff();
						screen(field, vertical, horizontal, current, x, y);
						break;
					}
					else break;
				case Down:
					if (collision(field, vertical, horizontal, current, x + 1, y)) {
						x++;
						printStuff();
						screen(field, vertical, horizontal, current, x, y);
						break;
					}
					else break;
				case Up:
					if (rotate(field, vertical, horizontal, current, x, y)) {
						printStuff();
						screen(field, vertical, horizontal, current, x, y);
						break;
					}
					else break;
				}
			}
			timer += 40;//1/25 секунды
			Sleep(40);
		}
		timer = 0;
		if (collision(field, vertical, horizontal, current, x + 1, y)) {//проверка столкновения фигурки с дном
			x++;
			printStuff();
			screen(field, vertical, horizontal, current, x, y);
		}
		else {
			merge(field, vertical, horizontal, current, x, y);
			full_line(field, vertical, horizontal);
			for (int i = 0; i < current.block_size; i++) {
				free(*(current.figure + i));
			}
			free(current.figure);
			current = next;
			next = create();
			x = 0;//начальная позиция фигурки по x
			y = horizontal / 2 - 2;//начальная позиция фигурки по у
			if (!collision(field, vertical, horizontal, current, x + 1, y)) {
				system("cls");
				SetConsoleTextAttribute(hConsole, (WORD)((Black << 4) | White));
				cout << "Game Over";
				exit(0);
			}
			timer = 1000;
			if (current.figure[1][0] == Brown || current.figure[1][0] == White || current.figure[1][0] == Red) {//костыль для "гэшек" и палки
				x -= 1;
			}
		}
		printStuff();
	}
}
void init_field(int**& field, int vertical, int horizontal) {//выделяем место под игровое поле и заполняем его черным цветом
	field = (int**)malloc(sizeof(int) * vertical);
	for (int i = 0; i < vertical - 1; i++) {
		*(field + i) = (int*)calloc(horizontal, sizeof(int));
		for (int j = 0; j < horizontal; j++) {
			if (j < 2 || j > horizontal - 3) {
				*(*(field + i) + j) = Cyan;
			}
			else *(*(field + i) + j) = Black;
		}
	}
	for (int i = 0; i < horizontal; i++) {
		*(*(field + vertical - 2) + i) = Cyan;
	}
}
void screen(int** field, int vertical, int horizontal, block current, int x, int y) {//вывод игры на экран
	cout << "\n\n\t";
	color(Cyan);
	for (int i = 0; i < horizontal - 2; i++) {
		cout << "  ";
	}
	color(Black);
	cout << endl;
	for (int i = 1; i < vertical - 1; i++) {
		cout << "\t";
		for (int j = 1; j < horizontal - 1; j++) {
			if (i >= x && i < x + current.block_size && j >= y && j < y + current.block_size & current.figure[i - x][j - y] != Black) {
				color(current.figure[i - x][j - y]);
			}
			else {
				color(*(*(field + i) + j));//берем текущий цвет ячейки поля
			}
			cout << "  ";
		}
		cout << endl;
	}
	color(Black);
}
block create() {
	block temp;
	int choose = rand() % 7;
	switch (choose) {
	case 0://красная палка
		temp.block_size = 4;
		temp.figure = (int**)malloc(sizeof(int*) * temp.block_size);
		for (int i = 0; i < temp.block_size; i++) {
			*(temp.figure + i) = (int*)malloc(sizeof(int) * temp.block_size);
			for (int j = 0; j < temp.block_size; j++) {
				i == 1 ? (*(*(temp.figure + i) + j) = Red) : (*(*(temp.figure + i) + j) = Black);//заполняем массив четырьмя красными блоками
			}
		}
		break;
	case 1://пирамидка
		temp.block_size = 3;
		temp.figure = (int**)malloc(sizeof(int*) * temp.block_size);
		for (int i = 0; i < temp.block_size; i++) {
			*(temp.figure + i) = (int*)malloc(sizeof(int) * temp.block_size);
			for (int j = 0; j < temp.block_size; j++) {
				if ((i == 0 && j == 1) || (i == 1))
					*(*(temp.figure + i) + j) = Yellow;
				else *(*(temp.figure + i) + j) = Black;
			}
		}
		break;
	case 2://квадрат
		temp.block_size = 2;
		temp.figure = (int**)malloc(sizeof(int*) * temp.block_size);
		for (int i = 0; i < temp.block_size; i++) {
			*(temp.figure + i) = (int*)malloc(sizeof(int) * temp.block_size);
			for (int j = 0; j < temp.block_size; j++) {
				*(*(temp.figure + i) + j) = Blue;
			}
		}
		break;
	case 3://Г1
		temp.block_size = 3;
		temp.figure = (int**)malloc(sizeof(int*) * temp.block_size);
		for (int i = 0; i < temp.block_size; i++) {
			*(temp.figure + i) = (int*)malloc(sizeof(int) * temp.block_size);
			for (int j = 0; j < temp.block_size; j++) {
				if (i == 1 || (i == 2 && j == 0)) {
					*(*(temp.figure + i) + j) = Brown;
				}
				else *(*(temp.figure + i) + j) = Black;
			}
		}
		break;
	case 4://Г1 инверсия
		temp.block_size = 3;
		temp.figure = (int**)malloc(sizeof(int*) * temp.block_size);
		for (int i = 0; i < temp.block_size; i++) {
			*(temp.figure + i) = (int*)malloc(sizeof(int) * temp.block_size);
			for (int j = 0; j < temp.block_size; j++) {
				if (i == 1 || (i == 2 && j == 2)) {
					*(*(temp.figure + i) + j) = White;
				}
				else *(*(temp.figure + i) + j) = Black;
			}
		}
		break;
	case 5://Эска
		temp.block_size = 3;
		temp.figure = (int**)malloc(sizeof(int*) * temp.block_size);
		for (int i = 0; i < temp.block_size; i++) {
			*(temp.figure + i) = (int*)malloc(sizeof(int) * temp.block_size);
			for (int j = 0; j < temp.block_size; j++) {
				if ((i == 0 && j == 2) || (i == 1 && (j == 1 || j == 2)) || (i == 2 && j == 1)) {
					*(*(temp.figure + i) + j) = Magenta;
				}
				else *(*(temp.figure + i) + j) = Black;
			}
		}
		break;
	case 6://Эска инверсия
		temp.block_size = 3;
		temp.figure = (int**)malloc(sizeof(int*) * temp.block_size);
		for (int i = 0; i < temp.block_size; i++) {
			*(temp.figure + i) = (int*)malloc(sizeof(int) * temp.block_size);
			for (int j = 0; j < temp.block_size; j++) {
				if ((i == 0 && j == 1) || (i == 1 && (j == 1 || j == 2)) || (i == 2 && j == 2)) {
					*(*(temp.figure + i) + j) = LightGreen;
				}
				else *(*(temp.figure + i) + j) = Black;
			}
		}
		break;
	}
	return temp;
}
bool collision(int** field, int vertical, int horizontal, block current, int x, int y) {
	for (int i = 0; i < current.block_size; i++) {
		for (int j = 0; j < current.block_size; j++) {
			if (current.figure[i][j] && *(*(field + x + i) + y + j)) {
				return false;
			}
		}
	}
	return true;
}
void merge(int** field, int vertical, int horizontal, block current, int x, int y) {
	for (int i = 0; i < current.block_size; i++) {
		for (int j = 0; j < current.block_size; j++) {
			if (current.figure[i][j] != Black) {
				*(*(field + x + i) + y + j) = current.figure[i][j];
			}
		}
	}
}
bool rotate(int** field, int vertical, int horizontal, block& current, int x, int y) {
	block tmp = current;
	tmp.figure = (int**)malloc(sizeof(int*) * tmp.block_size);
	for (int i = 0; i < tmp.block_size; i++) {
		*(tmp.figure + i) = (int*)malloc(sizeof(int) * tmp.block_size);
		for (int j = 0; j < tmp.block_size; j++) {
			tmp.figure[i][j] = current.figure[current.block_size - 1 - j][i];//поворачиваем матрицу фигурки
		}
	}
	if (collision(field, vertical, horizontal, tmp, x, y)) {//если повернутая фигура не пересекается с другими блоками
		for (int i = 0; i < tmp.block_size; i++) {//освобождаем нынешний массив фигуры
			free(*(current.figure + i));
		}
		free(current.figure);
		current = tmp;//присваиваем повернутую фигуру
		return true;
	}
	else {
		for (int i = 0; i < tmp.block_size; i++) {//массив повернутой фигуры нам не нужен
			free(*(tmp.figure + i));
		}
		free(tmp.figure);
		return false;
	}
}
void full_line(int** field, int vertical, int horizontal) {
	for (int i = 2; i < vertical - 2; i++) {
		int count = 0;
		for (int j = 2; j < horizontal - 2; j++) {
			if (*(*(field + i) + j)) {
				count++;//считаем блоки в ряду
			}
		}
		if (count == 10) {//если все блоки заполнены
			delete_line(field, vertical, horizontal, i);
		}
	}
}
void delete_line(int** field, int vertical, int horizontal, int line) {
	for (int i = line; i > 2; i--) {
		for (int j = 2; j < horizontal - 2; j++) {
			*(*(field + i) + j) = *(*(field + i - 1) + j);
		}
	}
}