#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses/ncurses.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>

//#define MIN_Y  2
#define CONTROLS  2 // Количество схем управления
enum {LEFT=1, UP, RIGHT, DOWN, STOP_GAME=KEY_F(10)};
enum {MAX_TAIL_SIZE=100, START_TAIL_SIZE=3/*, MAX_FOOD_SIZE=20, FOOD_EXPIRE_SECONDS=10*/};

/*
 Функция для перевода заглавной буквы в строчную. Все остальные символы возвращает как есть.
 */
int toLower(int key)
{
    if (key >= 'A' && key <= 'Z') return key + 32;
    return key;
}

// Здесь храним коды управления змейкой
struct control_buttons
{
    int down;
    int up;
    int left;
    int right;
}control_buttons;

struct control_buttons default_controls[CONTROLS] = {
    {KEY_DOWN, KEY_UP, KEY_LEFT, KEY_RIGHT},
    {'s', 'w', 'a', 'd'}
};

/*
 Голова змейки содержит в себе
 x,y - координаты текущей позиции
 direction - направление движения
 tsize - размер хвоста
 *tail -  ссылка на хвост
 */
typedef struct snake_t
{
    int x;
    int y;
    int direction;
    size_t tsize;
    struct tail_t *tail;
    struct control_buttons controls[CONTROLS];
} snake_t;

/*
 Хвост это массив состоящий из координат x,y
 */
typedef struct tail_t
{
    int x;
    int y;
} tail_t;

void initTail(struct tail_t t[], size_t size)
{
    struct tail_t init_t={0,0};
    for(size_t i=0; i<size; i++)
    {
        t[i]=init_t;
    }
}
void initHead(struct snake_t *head, int x, int y)
{
    head->x = x;
    head->y = y;
    head->direction = RIGHT;
}

void initSnake(snake_t *head, size_t size, int x, int y)
{
    tail_t*  tail  = (tail_t*) malloc(MAX_TAIL_SIZE*sizeof(tail_t));
    initTail(tail, MAX_TAIL_SIZE);
    initHead(head, x, y);
    head->tail = tail; // прикрепляем к голове хвост
    head->tsize = size+1;
    for (int i = 0; i < CONTROLS; i++) head->controls[i] = default_controls[i];
}

/*
 Проверка на столкновение головы змейки с туловищем. Функция возвращает 0, если столкновения нет, и 1 - если оно есть. 
 */
int checkCrash(snake_t *snake)
{
    for (size_t i = 0; i < snake->tsize; i++)
    {
		if (snake->tail[i].x == 0 && snake->tail[i].y == 0) continue;
		if (snake->x == snake->tail[i].x && snake->y == snake->tail[i].y) return 1;
	}
    return 0;
}

/*
 Движение головы с учетом текущего направления движения
 */
void go(struct snake_t *head)
{
    char ch = '@';
    int max_x=0, max_y=0;
    getmaxyx(stdscr, max_y, max_x); // macro - размер терминала
    mvprintw(head->y, head->x, " "); // очищаем один символ
    switch (head->direction)
    {
        case LEFT:
            if (head->x <= 0) head->x = max_x; // телепортация вправо
            mvprintw(head->y, --(head->x), "%c", ch);
        break;
        case RIGHT:
		    if (head->x >= max_x - 1) head->x = -1; // телепортация влево
            mvprintw(head->y, ++(head->x), "%c", ch);
        break;
        case UP:
            if (head->y <= 0) head->y = max_y; // телепортация вниз
            mvprintw(--(head->y), head->x, "%c", ch);
        break;
        case DOWN:
            if (head->y >= max_y - 1) head->y = -1; // телепортация вверх
            mvprintw(++(head->y), head->x, "%c", ch);
        break;
        default:
        break;
    }
    refresh();
}

/*
 Проверка выбранного направления. Функция возвращает 1 если смена направления допустима, 0 если змейка пытается развернуться на 180 градусов.
 */
int checkDirection(snake_t* snake, int32_t key)
{
    int next_direction = -1; 
    int key_lower = toLower(key);
    for (int i = 0; i < CONTROLS; i++)
    {
        // Определяем запрашиваемое направление по текущей схеме управления
        if (key_lower == snake->controls[i].down)
        {
            next_direction = DOWN;
            break;
        }
        else if (key_lower == snake->controls[i].up)
        {
            next_direction = UP;
            break;
        }
        else if (key_lower == snake->controls[i].right)
        {
            next_direction = RIGHT;
            break;
        }
        else if (key_lower == snake->controls[i].left)
        {
            next_direction = LEFT;
            break;
        }
        else
            continue;
    } 
    // Запрет разворота на 180 градусов
    if ((snake->direction == RIGHT && next_direction == LEFT) ||
        (snake->direction == LEFT  && next_direction == RIGHT) ||
        (snake->direction == UP    && next_direction == DOWN) ||
        (snake->direction == DOWN  && next_direction == UP))
    {
        return 0;
    }
    return 1;
}

void changeDirection(struct snake_t* snake, const int32_t key)
{
	if (!checkDirection(snake, key)) return; // игнорируем нажатие
	int key_lower = toLower(key);
	for (int i = 0; i < CONTROLS; i++)
    {
        if (key_lower == snake->controls[i].down)
        {
            snake->direction = DOWN;
            break;
        }
        else if (key_lower == snake->controls[i].up)
        {
            snake->direction = UP;
            break;
        }
        else if (key_lower == snake->controls[i].right)
        {
            snake->direction = RIGHT;
            break;
        }
        else if (key_lower == snake->controls[i].left)
        {
            snake->direction = LEFT;
            break;
        }
    }
}

/*
 Движение хвоста с учетом движения головы
 */
void goTail(struct snake_t *head)
{
    char ch = '*';
    mvprintw(head->tail[head->tsize-1].y, head->tail[head->tsize-1].x, " ");
    for(size_t i = head->tsize-1; i>0; i--)
    {
        head->tail[i] = head->tail[i-1];
        if( head->tail[i].y || head->tail[i].x)
            mvprintw(head->tail[i].y, head->tail[i].x, "%c", ch);
    }
    head->tail[0].x = head->x;
    head->tail[0].y = head->y;
}

int main()
{
snake_t* snake = (snake_t*)malloc(sizeof(snake_t));
    initSnake(snake,START_TAIL_SIZE,10,10);
    initscr();
    keypad(stdscr, TRUE); // Включаем F1, F2, стрелки и т.д.
    raw();                // Откдючаем line buffering
    noecho();            // Отключаем echo() режим при вызове getch
    curs_set(FALSE);    //Отключаем курсор
    mvprintw(0, 0, "Use arrows or WASD for control. Press 'F10' for EXIT");
    timeout(0);    //Отключаем таймаут после нажатия клавиши в цикле
    int key_pressed=0;
    while(key_pressed != STOP_GAME)
    {
        key_pressed = getch(); // Считываем клавишу
        go(snake);
        if (checkCrash(snake)) // Проверка на столкновение
        {
		    key_pressed = STOP_GAME;
		    break;
        }
        goTail(snake);
        timeout(100); // Задержка при отрисовке
        changeDirection(snake, key_pressed);
    }
    free(snake->tail);
    free(snake);
    endwin(); // Завершаем режим curses mod
    return 0;
}
