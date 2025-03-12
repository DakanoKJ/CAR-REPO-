#include<Windows.h>
#include<iostream>
#include<conio.h>
#include<thread>
using namespace std::literals::chrono_literals;
using std::cin;
using std::cout;
using std::endl;

#define Enter		13
#define	Escape		27
#define UpArrow		72
#define DownArrow	80

#define MIN_TANK_CAPACITY	 20
#define MAX_TANK_CAPACITY	120
//define (определить).
//Директива #define создает макроопределение, которое показывает компилятору ЧТО_ЗАМЕНИТЬ и ЧЕМ_ЗАМЕНИТЬ


class Tank
{
	const int CAPACITY;
	double fuel_level;
public:
	int get_CAPACITY()const
	{
		return CAPACITY;
	}
	double get_fuel_level()const
	{
		return fuel_level;
	}
	double fill(double amount)
	{
		if (amount < 0)return fuel_level;
		fuel_level += amount;
		if (fuel_level > CAPACITY)fuel_level = CAPACITY;
		return fuel_level;
	}
	double give_fuel(double amount)
	{
		fuel_level -= amount;
		if (fuel_level < 0)fuel_level = 0;
		return fuel_level;
	}

	//				Constructors:
	Tank(int capacity) :CAPACITY
	(
		capacity < MIN_TANK_CAPACITY ? MIN_TANK_CAPACITY :
		capacity > MAX_TANK_CAPACITY ? MAX_TANK_CAPACITY :
		capacity
	), fuel_level(0)	//Инициализация в заголовке.
		//Если в классе есть константа, то ее можно проинициализировать только в заголовке.
		//Инициализация в заголовке отрабатывает еще до того, как отрабатывает тело конструктора
	{
		//this->CAPACITY = capacity;
		//this->fuel_level = 0;
		cout << "Tank is ready:\t" << this << endl;
	}
	~Tank()
	{
		cout << "Tank is over:\t" << this << endl;
	}

	void info()const
	{
		cout << "Capacity:\t" << get_CAPACITY() << " liters\n";
		cout << "Fuel level:\t" << get_fuel_level() << " liters\n";
	}
};

#define MIN_ENGINE_CONSUMPTION	 4
#define MAX_ENGINE_CONSUMPTION	30

class Engine
{
	const double CONSUMPTION;
	double consumption_per_second;
	bool is_started;
public:
	double get_CONSUMPTION()const
	{
		return CONSUMPTION;
	}
	double get_consumption_per_second()const
	{
		return consumption_per_second;
	}
	void set_consumption_per_second()
	{
		this->consumption_per_second = CONSUMPTION * 3e-5;
	}

	//					Constructors:
	Engine(double consumption) :CONSUMPTION
	(
		consumption < MIN_ENGINE_CONSUMPTION ? MIN_ENGINE_CONSUMPTION :
		consumption > MAX_ENGINE_CONSUMPTION ? MAX_ENGINE_CONSUMPTION :
		consumption
	)
	{
		set_consumption_per_second();
		is_started = false;
		cout << "Engine is ready:\t" << this << endl;
	}
	~Engine()
	{
		cout << "Engine is over:\t" << this << endl;
	}

	void start()
	{
		is_started = true;
	}
	void stop()
	{
		is_started = false;
	}
	bool started()const
	{
		return is_started;
	}

	void info()const
	{
		cout << "Consumption:\t" << get_CONSUMPTION() << " liters\n";
		cout << "Consumption:\t" << get_consumption_per_second() << " liters/s\n";
	}
};

/*
	std::thread thread_name(&MemberFunction, &object);
	member-variable - переменная-член класса
	member-function - функиця-член класса
*/

#define MIN_SPEED_LIMIT	 30
#define MAX_SPEED_LIMIT	408

class Car
{
	Engine engine;
	Tank tank;
	bool driver_inside;
	int speed;
	const int MAX_SPEED;
	int acceleration;
	struct
	{
		std::thread panel_thread;
		std::thread engine_idle_thread;
		std::thread free_wheeling_thread;
	}control;
public:
	Car(double consumption, int capacity, int max_speed = 250) :
		engine(consumption),
		tank(capacity),
		MAX_SPEED
		(
			max_speed < MIN_SPEED_LIMIT ? MIN_SPEED_LIMIT :
			max_speed > MAX_SPEED_LIMIT ? MAX_SPEED_LIMIT :
			max_speed
		)
	{
		acceleration = 20;
		driver_inside = false;
		speed = 0;
		cout << "Your car is ready to go, press Enter to get in ;-)" << endl;
	}
	~Car()
	{
		cout << "Your car is over" << endl;
	}

	void get_in()
	{
		driver_inside = true;
		control.panel_thread = std::thread(&Car::panel, this);
	}
	void get_out()
	{
		driver_inside = false;
		if (control.panel_thread.joinable())
		{
			control.panel_thread.join();
			system("CLS");
			cout << "Вы вышли мз машины" << endl;
		}
	}

	void panel()const
	{
		while (driver_inside)
		{
			system("CLS");
			for (int i = 0; i < speed / 2.5; i++)
			{
				cout << "|";
			}
			cout << endl;
			cout << "Speed:	   " << speed << " km/h\n";
			cout << "Fuel level: " << tank.get_fuel_level() << " liters\t";
			if (tank.get_fuel_level() < 5)
			{
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);	//Получаем окно консоли, и сохраняем его в HANDLE
				SetConsoleTextAttribute(hConsole, 0xCF);
				cout << "LOW FUEL";
				SetConsoleTextAttribute(hConsole, 0x07);
				//Функция SetConsoleTextAttribute(...) задает цвет фона и шрифта в заданном окне консоли
				//0xCF и 0x07 - это шестнадцатеричный код цвета.
				//Коды цветов всегда можно посмотреть в командной строке при помощи команды 'color';
				//0x - этот префикс показывает, что дальше идет Шестнадцатеричное число.
				//Старшая цифра - это цвет фона
				//Младшая цифра - цвет текста
			}
			cout << endl;
			cout << "Engine is " << (engine.started() ? "started" : "stopped") << endl;
			cout << "DEBUG:\n";
			cout << "Friction enabled: " << control.free_wheeling_thread.joinable() << endl;
			Sleep(100);	//100 - это задержка в миллисекундах.
		}
	}
	void engine_idle()
	{
		while (tank.give_fuel(engine.get_consumption_per_second()) && engine.started())
			std::this_thread::sleep_for(1s);
	}
	void start()
	{
		if (tank.get_fuel_level())
		{
			engine.start();
			control.engine_idle_thread = std::thread(&Car::engine_idle, this);
		}
		else
		{
			cout << "Проверьте уровент топлива" << endl;
		}
	}
	void stop()
	{
		engine.stop();
		if (control.engine_idle_thread.joinable())
			control.engine_idle_thread.join();
	}
	void free_wheeling()
	{
		while (speed > 0)
		{
			speed -= 3;
			if (speed < 0)speed = 0;
			std::this_thread::sleep_for(1s);
		}
	}
	void accelerate()
	{
		if (driver_inside && engine.started())
		{
			speed += acceleration;
			if (speed > MAX_SPEED)speed = MAX_SPEED;
			if (speed > 0 && !control.free_wheeling_thread.joinable())
				control.free_wheeling_thread = std::thread(&Car::free_wheeling, this);
			std::this_thread::sleep_for(1s);
		}
	}
	void slow_down()
	{
		if (driver_inside && speed > 0)
		{
			speed -= acceleration;
			if (speed < 0)speed = 0;
			if (speed == 0 && control.engine_idle_thread.joinable())
				control.free_wheeling_thread.join();
			std::this_thread::sleep_for(1s);
		}
	}

	void control_car()
	{
		char key = 0;
		do
		{
			//Функция _kbhit() в фоновом режиме отслеживает нажатие клавиши . 
			//Если клавиша была нажата _kbhit() возвращает 'true', в противном слечае - 'false'.
			key = 0;
			if (_kbhit())
			{
				key = _getch();	//Функция _getch() ожидает нажатие клавиши, и возвращает ASCII-код нажатой клавиши.
				//Т.е., _kbhit() ослеживает сам факт нажатия на клавишу (не важно на какую), а _getch() определяет,
				//какая клавиша была нажата.
			//cout << (int)key << endl;

				switch (key)
				{
				case Enter:
					driver_inside ? get_out() : get_in();
					break;
				case 'F':	//Fill - заправить
				case 'f':
					if (engine.started() || driver_inside)
					{
						cout << "Для начала заглушите мотор и выйдите из машины" << endl;
					}
					else
					{
						double fuel_level;
						cout << "Введите объем топлива: "; cin >> fuel_level;
						tank.fill(fuel_level);
					}
					break;
				case 'I':	//Ignition - зажигание
				case 'i':
					if (driver_inside)engine.started() ? stop() : start();
					break;
				case 'W':case 'w':case UpArrow:		accelerate();	break;
				case 'S':case 's':case DownArrow:	slow_down();	break;
				case Escape:
					stop();
					get_out();
				}
			}
			//cout << "+";
			if (speed == 0 && control.free_wheeling_thread.joinable())
				control.free_wheeling_thread.join();
		} while (key != Escape);


		//thread - это последовательность команд Центральному процессору
	}

	void info()const
	{
		engine.info();
		tank.info();
	}
};

//#define TANK_CHECK	//Определяем TANK_CHECK
//#define ENGINE_CHECK

void main()
{
	setlocale(LC_ALL, "");

#ifdef TANK_CHECK	//Если определено TASK_CHECK, то нижеследующий код будет виден компилятору, до директивы #endif
	Tank tank(60);
	tank.info();
	int fuel;
	do
	{
		cout << "Введите объем топлива: "; cin >> fuel;
		tank.fill(fuel);
		tank.info();
	} while (fuel > 0);
#endif // TANK_CHECK

#ifdef ENGINE_CHECK
	Engine engine(10);
	engine.info();
#endif

	Car bmw(10, 80);
	bmw.control_car();
	bmw.info();

}