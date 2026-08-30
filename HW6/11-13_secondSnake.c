#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses/ncurses.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>

//#define MIN_Y  2
double DELAY = 0.1;
#define PLAYERS 2

enum {LEFT=1, UP, RIGHT, DOWN, STOP_GAME=KEY_F(10)};
enum {MAX_TAIL_SIZE=100, START_TAIL_SIZE=3, MAX_FOOD_SIZE=20, FOOD_EXPIRE_SECONDS=10,SEED_NUMBER=3};

/*
 Функция для перевода заглавной буквы в строчную. Все остальные символы возвращает как есть.
 */
int toLower(int key)
{
    if (key >= 'A' && key <= 'Z') return key + 32;
    return key;
}

/*
 Здесь храним коды управления змейкой
*/
struct control_buttons
{
    int down;
    int up;
    int left;
    int right;
} control_buttons;

struct control_buttons default_controls[PLAYERS] = {{KEY_DOWN, KEY_UP, KEY_LEFT, KEY_RIGHT},
                                                    {'s', 'w', 'a', 'd'}};

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
    struct control_buttons controls;
} snake_t;

/*
 Хвост это массив состоящий из координат x,y
 */
typedef struct tail_t
{
    int x;
    int y;
} tail_t;

/*
 Еда — это массив точек, состоящий из координат x,y, времени,
 когда данная точка была установлена, и поля, сигнализирующего,
 была ли данная точка съедена.
 */
struct food
{
    int x;
    int y;
    time_t put_time;
    char point;
    uint8_t enable;
} food[MAX_FOOD_SIZE];

void initFood(struct food f[], size_t size)
{
    struct food init = {0,0,0,0,0};
    for(size_t i=0; i<size; i++)
    {
        f[i] = init;
    }
}

/*
 Обновить/разместить текущее зерно на поле
 */
void putFoodSeed(struct food *fp)
{
    int max_x=0, max_y=0;
    char spoint[2] = {0};
    getmaxyx(stdscr, max_y, max_x);
    mvprintw(fp->y, fp->x, " ");
    fp->x = rand() % (max_x - 1);
    fp->y = rand() % (max_y - 2) + 1; //Не занимаем верхнюю строку
    fp->put_time = time(NULL);
    fp->point = '$';
    fp->enable = 1;
    spoint[0] = fp->point;
    mvprintw(fp->y, fp->x, "%s", spoint);
}

/*
 Разместить еду на поле
 */
void putFood(struct food f[], size_t number_seeds)
{
    for(size_t i=0; i<number_seeds; i++)
    {
        putFoodSeed(&f[i]);
    }
}

void refreshFood(struct food f[], int nfood)
{
    for(size_t i=0; i<nfood; i++)
    {
        if( f[i].put_time )
        {
            if( !f[i].enable || (time(NULL) - f[i].put_time) > FOOD_EXPIRE_SECONDS )
            {
                putFoodSeed(&f[i]);
            }
        }
    }
}

void initTail(struct tail_t t[], size_t size)
{
    struct tail_t init_t= {0,0};
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

void initSnake(snake_t *head[], size_t size, int x, int y,int i)
{
    head[i]    = (snake_t*)malloc(sizeof(snake_t));
	tail_t*  tail  = (tail_t*) malloc(MAX_TAIL_SIZE*sizeof(tail_t));
    initTail(tail, MAX_TAIL_SIZE);
    initHead(head[i], x, y);
    head[i]->tail     = tail; // прикрепляем к голове хвост
    head[i]->tsize    = size+1;
	head[i]->controls = default_controls[i];
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
    // Определяем запрашиваемое направление по текущей схеме управления
	if (key_lower == snake->controls.down) next_direction = DOWN;
    else if (key_lower == snake->controls.up) next_direction = UP;
	else if (key_lower == snake->controls.right) next_direction = RIGHT;
	else if (key_lower == snake->controls.left) next_direction = LEFT;
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
	int key_lower = toLower(key);
	if (key_lower == snake->controls.down)
		snake->direction = DOWN;
	else if (key_lower == snake->controls.up)
		snake->direction = UP;
	else if (key_lower == snake->controls.right)
		snake->direction = RIGHT;
	else if (key_lower == snake->controls.left)
		snake->direction = LEFT;
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

//Проверка того, является ли какое-то из зерен съеденным,
_Bool haveEat(struct snake_t *head, struct food f[])
{
	for(size_t i = 0; i < MAX_FOOD_SIZE; i++)
	{
		if(f[i].enable && f[i].x == head->x && f[i].y == head->y)
		{
			f[i].enable = 0; // Помечаем зерно как съеденное
			return 1;
		}
	}
	return 0;
}

/*
 Увеличение хвоста на 1 элемент
 */

void addTail(struct snake_t *head)
{
	if(head->tsize < MAX_TAIL_SIZE) 
	{
		// Копируем последний сегмент, чтобы новый элемент не появился в точке (0,0)
		head->tail[head->tsize] = head->tail[head->tsize - 1];
		head->tsize++;
	}
}

void update(struct snake_t *head, struct food f[], const int32_t key)
{
    go(head);
    goTail(head);
    if (checkDirection(head,key))
    {
        changeDirection(head, key);
    }
    refreshFood(f, SEED_NUMBER);// Обновляем еду
    if (haveEat(head,f))
    {
        addTail(head);
    }
    refresh();//Обновление экрана, вывели кадр анимации
    napms((int)(DELAY * 1000));
}

_Bool isCrush(snake_t * snake)
{
	for(size_t i = 1; i < snake->tsize; i++)
	{
		if(snake->x == snake->tail[i].x && snake->y == snake->tail[i].y)
		{
			return 1; // Произошло столкновение
		}
	}
	return 0;
}

void repairSeed(struct food f[], size_t nfood, struct snake_t *head)
{
	for(size_t i=0; i<head->tsize; i++)
		for(size_t j=0; j<nfood; j++)
		{
			/* Если хвост совпадает с зерном */
			if(f[j].enable && head->tail[i].x == f[j].x && head->tail[i].y == f[j].y)
			{
				putFoodSeed(&f[j]); // Генерируем новую позицию для зерна
			}
		}
	for(size_t i=0; i<nfood; i++)
		for(size_t j=0; j<nfood; j++)
		{
            /* Если два зерна на одной точке */
			if(i != j && f[i].enable && f[j].enable && f[i].x == f[j].x && f[i].y == f[j].y)
			{
				putFoodSeed(&f[j]); // Перемещаем дублирующее зерно
			}
		}
}


int main()
{
//========================================================================   
snake_t* snakes[PLAYERS];
    for (int i = 0; i < PLAYERS; i++)
        initSnake(snakes,START_TAIL_SIZE,10+i*10,10+i*10,i);
//========================================================================

    initscr();
    keypad(stdscr, TRUE); // Включаем F1, F2, стрелки и т.д.
    raw(); // Откдючаем line buffering
    noecho(); // Отключаем echo() режим при вызове getch
    curs_set(FALSE);//Отключаем курсор
    mvprintw(0, 0,"Use arrows for control. Press 'F10' for EXIT");
    timeout(0); // Отключаем таймаут после нажатия клавиши в цикле
    initFood(food, MAX_FOOD_SIZE);
    putFood(food, SEED_NUMBER); // Кладем зерна
    
    int key_pressed = 0;
    int loser = 0; // Проигравший игрок
    int winner = 0; // Победитель
    
    while(key_pressed != STOP_GAME && loser == 0 && winner == 0)
    {
        key_pressed = getch(); // Считываем клавишу
        for (int i = 0; i < PLAYERS; i++)
        {
            update(snakes[i], food, key_pressed);
            
            // Проверка на столкновение
            if(isCrush(snakes[i]))
            {
				loser = i + 1;
                break;
            }
            
            // Проверка на достижение максимальной длины
            if(snakes[i]->tsize >= MAX_TAIL_SIZE)
            {
                winner = i + 1;
                break;
            }

            repairSeed(food, SEED_NUMBER, snakes[i]);
        }
    }
    
    clear(); // Очищаем экран от игрового поля

	if (winner > 0) mvprintw(10, 20, "Player %d won by reaching max length!!!", winner);
    else if (loser > 0) mvprintw(10, 20, "Player %d lost!", loser);
    else mvprintw(10, 20, "Game over.");

    mvprintw(12, 20, "Press F10 to exit.");
    refresh(); // Отрисовываем финальный экран
	// Переключаемся в блокирующий режим, чтобы дождаться нажатия клавиши
	timeout(-1); 
    int exit_key = 0;
    while (exit_key != KEY_F(10)) exit_key = getch();
	
    for (int i = 0; i < PLAYERS; i++)
    {
        //printExit(snakes[i]);
        free(snakes[i]->tail);
        free(snakes[i]);
    }
    endwin(); // Завершаем режим curses mod
    return 0;
}
