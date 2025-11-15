#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <random>

// Проверка на корректный ввод
void checkingForCorrectInput(int& input);

// Генерация случайного числа
int deneratingRandomNumber(int min, int max);

void draw(int operationNumber, std::mutex& coutMutex, int totalLength);

int main()
{
	// Подключение Русского языка
	setlocale(LC_ALL, "rus");

	// Мьютекс для cout
	std::mutex coutMutex;

	// Запрос количества потоков
	int numberThreads{};
	std::cout << "Введите количество потоков: ";
	checkingForCorrectInput(numberThreads);

	// Отчищаем консоль
	std::cout << "\033[2J\033[3J\033[H";
	std::cout.flush();

	// Вывод шапки
	std::cout << "#\tid\tProgress Bar\tTime" << std::endl;

	// Длина прогресс-бара
	const int progressBarLength = 11;

	// Запуск потоков
	std::vector<std::thread> streams;
	for (size_t index{}; index < numberThreads; index++)
	{
		streams.push_back(std::thread(draw, index, std::ref(coutMutex),
			std::ref(progressBarLength)));
	}
	
	// Ожидаем завершение всех потоков
	for (auto& thread : streams)
	{
		thread.join();
	}

	// Выводим сообщение о завершении работы с мьютексом
	std::lock_guard<std::mutex> lock(coutMutex);
	std::cout << "\nВсе потоки завершили работу!" << std::endl;


	return EXIT_SUCCESS;
}

void checkingForCorrectInput(int& input)
{
	while ((!(std::cin >> input)) || input < 0)
	{
		// Сброс некорректного ввода
		std::cin.clear();
		std::cin.ignore();

		std::cout << "Вы ввели некорректное значение!" << std::endl;
		std::cout << "Попробуйте ещё раз: ";
	}
}

int deneratingRandomNumber(int min, int max)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(min, max);

	return dist(gen);
}

void draw(int operationNumber, std::mutex& coutMutex, int totalLength)
{
	// Определяем позицию строки в консоли
	int rowPosition{ operationNumber + 2 };

	// Начало отсчета времени
	auto startTime{ std::chrono::steady_clock::now() };

	for (int progress{ 1 }; progress <= totalLength; progress++)
	{
		// Засыпает на случайное время
		// Минимум - 70 мс
		// Максимум - 250 мс
		std::this_thread::sleep_for(std::chrono::milliseconds(deneratingRandomNumber(70, 250)));

		// Захватываем мьютекс
		std::lock_guard<std::mutex> lock(coutMutex);

		// Перемещаем курсор на свою строку
		std::cout << "\033[" << rowPosition << ";1H";

		// Выводим всю строку заново
		std::cout << operationNumber << "\t" << std::this_thread::get_id() << "\t[";

		// Прогресс-бар
		for (int i{}; i < progress; i++) { std::cout << "="; }
		for (int i{ progress }; i < totalLength; i++) std::cout << " ";
		std::cout << "]" << std::flush;
	}

	// Конец отсчета времени
	auto endTime{ std::chrono::steady_clock::now() };
	// Подсчёт сколько всего времени было потрачено на операцию
	auto duration{ std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime) };

	// Вывод строки с временем
	{
		std::lock_guard<std::mutex> lock(coutMutex);
		std::cout << "\033[" << rowPosition << ";1H";
		std::cout << operationNumber << "\t" << std::this_thread::get_id() << "\t[";
		for (int i = 0; i < totalLength; i++) std::cout << "=";
		std::cout << "]\t" << duration.count() << "ms" << std::endl;
	}
}