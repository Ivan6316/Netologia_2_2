#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

// Расчёт суммы двух векторов
template <typename T>
void sumVector(const std::vector<T>& vector1, const std::vector<T>& vector2, std::vector<T>& results,
	size_t lastIndex, size_t firstIndex = 0);

// Запуск расчёта суммы векторов в нескольких потоках
long long launchingCalculations(long numberElementsInVector, int numberThreads);

// Вывод таблицы
void tableOutput(const std::vector<std::vector<long long>>& resultsTable, const std::vector<long>& sizes,
	const std::vector<int>& threadsCount);

int main()
{
	// Подключение Русского языка для консоли
	setlocale(LC_ALL, "rus");

	// Вывод количества аппаратных ядер
	std::cout << "Количество аппаратных ядер - " << std::thread::hardware_concurrency() << std::endl;
	std::cout << std::endl;

	std::vector<long> sizes = { 1000, 10000, 100000, 1000000 };
	std::vector<int> threadsCount = { 1, 2, 4, 8, 16 };

	// Создаём таблицу для хранения результатов
	std::vector<std::vector<long long>> resultsTable(sizes.size(), std::vector<long long>(threadsCount.size()));

	// Заполнение таблицы
	for (size_t index{}; index < sizes.size(); index++)
	{
		for (size_t index2{}; index2 < threadsCount.size(); index2++)
		{
			try
			{
				resultsTable[index][index2] = launchingCalculations(sizes[index], threadsCount[index2]);
			}
			catch (const std::exception& e)
			{
				std::cout << "Ошибка при заполнении таблицы: " << e.what() << std::endl;
				// помечаем ошибку
				resultsTable[index][index2] = -1; 
			}
		}
	}
	
	// Вывод таблицы
	tableOutput(resultsTable, sizes, threadsCount);


	return EXIT_SUCCESS;
}

template <typename T>
void sumVector(const std::vector<T>& vector1, const std::vector<T>& vector2, std::vector<T>& results,
	size_t lastIndex, size_t firstIndex)
{
	// Проверка на одинаковый размер векторов
	if (vector1.size() != vector2.size())
	{
		throw std::invalid_argument("Векторы должны быть одинакового размера!");
	}

	// Операция сложения
	for (size_t index{ firstIndex }; index < lastIndex; index++)
	{
		results[index] = vector1[index] + vector2[index];
	}
}

long long launchingCalculations(long numberElementsInVector, int numberThreads)
{
	// Проверка на отрицательные значения
	if (numberElementsInVector < 0 || numberThreads < 0)
	{
		throw std::invalid_argument("Количество элементов вектора или потоков должно быть положительным!");
	}

	// Расчёт количества элементов для каждого потока
	int numberElementsForEachStream{ numberElementsInVector / numberThreads };

	std::vector<int> vector1(numberElementsInVector, 0);
	std::vector<int> vector2(numberElementsInVector, 1);
	std::vector<int> results(numberElementsInVector);
	std::vector<std::thread> TV;

	// Засекаем время начала
	auto startTime{ std::chrono::high_resolution_clock::now() };

	for (size_t i{}; i < numberThreads; i++)
	{
		size_t startIndex{ i * numberElementsForEachStream };
		size_t endIndex = (i == numberThreads - 1) ? numberElementsInVector : (i + 1) * numberElementsForEachStream;

		TV.push_back(std::thread(sumVector<int>, std::ref(vector1), std::ref(vector2), 
                        std::ref(results), startIndex, endIndex));
	}

	// Ждём завершение всех потоков
	for (auto& thread : TV)
	{
		thread.join();
	}

	// Засекаем время окончания
	auto endTime{ std::chrono::high_resolution_clock::now() };


	return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
}

void tableOutput(const std::vector<std::vector<long long>>& resultsTable, const std::vector<long>& sizes,
	const std::vector<int>& threadsCount)
{
	for (size_t index{}; index < sizes.size(); index++)
	{
		std::cout << "\t" << sizes[index];
	}
	std::cout << std::endl;

	for (size_t index{}; index < threadsCount.size(); index++)
	{
		std::cout << threadsCount[index];
		for (size_t index2{}; index2 < sizes.size(); index2++)
		{
			std::cout << "\t" << resultsTable[index2][index] << " мс";
		}
		std::cout << std::endl;
	}
}