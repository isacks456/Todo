#include <iostream>
#include <vector>
#include <string>
#include <Windows.h>
#include <fstream>

class Task
{
	int id;
	std::string title;
	std::string description;
	bool completed;

public:
	Task(int p_id, std::string p_title, std::string p_description, bool p_completed) : id{ p_id }, title{ std::move(p_title) }, description{std::move(p_description) }, completed{ p_completed }
	{

	}

	int GetId() const
	{
		return id;
	}

	const std::string& GetTitle() const
	{
		return title;
	}

	const std::string& GetDescription() const
	{
		return description;
	}

	bool GetCompleted() const
	{
		return completed;
	}

	void SetTitle(const std::string& newTitle)
	{
		title = newTitle;
	}

	void SetDescription(const std::string& newDescription)
	{
		description = newDescription;
	}

	void SetCompeted(const bool& newCompleted)
	{
		completed = newCompleted;
	}

};

class Todo
{
	std::vector<Task> tasks;
	int newId = 1;

public:

	void addTask()
	{

		std::cin.ignore(32767, '\n');

		std::string newTitle;
		std::string newDescription;

		std::cout << "Введите название задачи: ";
		std::getline(std::cin, newTitle);


		std::cout << "Введите описание задачи: ";
		std::getline(std::cin, newDescription);


		tasks.push_back(Task(newId, newTitle, newDescription, false));

		newId++;


		std::cout << "Задача успешно добавлена!";
		std::cout << std::endl;

	}

	void showAllTasks()
	{
		std::cout << "Список всех задач: " << std::endl;

		for (size_t i = 0; i < tasks.size(); ++i)
		{
			std::cout << "ID: " << tasks[i].GetId() << "\n"
				<< "Название: " << tasks[i].GetTitle() << "\n"
				<< "Описание: " << tasks[i].GetDescription() << "\n"
				<< "Статус: " << (tasks[i].GetCompleted() ? " Выполнено " : " В процессе... ")
				<< std::endl;
			
			std::cout << std::endl;
		}
	}

	void showCompletedTasks()
	{

		std::cout << "Выполненные задачи: " << "\n";
		bool hasCompleted = false;

		for (size_t i = 0; i < tasks.size(); ++i)
		{
			if (tasks[i].GetCompleted())
			{

				hasCompleted = true;

				std::cout << "ID: " << tasks[i].GetId() << "\n"
					<< "Название: " << tasks[i].GetTitle() << "\n"
					<< "Описание: " << tasks[i].GetDescription() << "\n"
					<< std::endl;
			}

			if (!hasCompleted)
			{
				std::cout << "У вас пока нет выполненных задач" << std::endl;
			}
		}
	}

	void showIsNotCompletedTasks()
	{
		std::cout << "Не выполненные задачи: " << std::endl;
		bool isNotHasCompleted = false;

		for (size_t i = 0; i < tasks.size(); ++i)
		{
			if (tasks[i].GetCompleted() == false)
			{

				isNotHasCompleted = true;

				std::cout << "ID: " << tasks[i].GetId() << "\n"
					<< "Название: " << tasks[i].GetTitle() << "\n"
					<< "Описание: " << tasks[i].GetDescription() << "\n"
					<< std::endl;
			}

			if (!isNotHasCompleted)
			{
				std::cout << "У вас нет невыполненных задач" << std::endl;
			}
		}
	}

	void changeTask()
	{

		int number;

		bool isFound = false;

		std::cout << "Напишите ID задачи, которую хотите изменить: ";
		std::cin >> number;

		for (size_t i = 0; i < tasks.size(); ++i)
		{
			if (tasks[i].GetId() == number)
			{
				isFound = true;

				std::cin.ignore(32767, '\n');

				std::string newTitle;
				std::string newDescription;

				std::cout << "Введите новый заголовок: ";
				std::getline(std::cin, newTitle);

				std::cout << "Введите новое описание: ";
				std::getline(std::cin, newDescription);

				tasks[i].SetTitle(newTitle);
				tasks[i].SetDescription(newDescription);

				std::cout << "Задача успешно изменена!" << std::endl;
				break;
			}
		}

		if (!isFound)
		{
			std::cout << "Вы ввели несуществующее ID!" << std::endl;
		}

	}

	void taskCompleted()
	{
		int number;
		std::cout << "Введите ID задачи: ";
		std::cin >> number;
		bool isFound = false;

		for (size_t i = 0; i < tasks.size(); ++i)
		{
			if (tasks[i].GetId() == number)
			{
				isFound = true;

				tasks[i].SetCompeted(true);

				std::cout << "Ваша задача выполнена!" << std::endl;
				break;
			}
		}

		if (!isFound)
		{
			std::cout << "Вы ввели несуществующее ID!" << std::endl;
		}

	}

	void dropTask()
	{
		int number;
		int newId = 1;
		std::cout << "Введите ID задачи: ";
		std::cin >> number;

		bool isFound = false;

		for (size_t i = 0; i < tasks.size(); ++i)
		{
			if (tasks[i].GetId() == number)
			{
				isFound = true;

				tasks.erase(tasks.begin() + i);

				std::cout << "Ваша задача успешно удалена!" << std::endl;

				break;
			}
		}

		if (!isFound)
		{
			std::cout << "Вы ввели несуществующее ID!" << std::endl;
		}

	}

	void findTask()
	{
		int number;
		std::cout << "ВВедите ID задачи: ";
		std::cin >> number;

		bool isFound = false;

		for (size_t i = 0; i < tasks.size(); ++i)
		{

			if (tasks[i].GetId() == number)
			{
				isFound = true;

				std::cout << "Ваша задача: " << std::endl;
				std::cout << "ID: " << tasks[i].GetId() << "\n"
					<< "Название: " << tasks[i].GetTitle() << "\n"
					<< "Описание: " << tasks[i].GetDescription() << "\n"
					<< "Статус: " << (tasks[i].GetCompleted() ? " Выполнено " : " В процессе... ")
					<< std::endl;
				
				break;

			}
		}

		if (!isFound)
		{
			std::cout << "Вы ввели несуществующее ID!" << std::endl;
		}

	}

	void addToFile()
	{
		std::ofstream out("Todo.txt");

		if (!out.is_open())
		{
			std::cout << "Ошибка открытия файла на запись!" << std::endl;
			return;
		}

		for (size_t i = 0; i < tasks.size(); ++i)
		{
				out << tasks[i].GetId() << "\n"
					<< tasks[i].GetTitle() << "\n"
					<< tasks[i].GetDescription() << "\n"
					<< (tasks[i].GetCompleted() ? " Выполнено " : " В процессе... ") << "\n"
					<< std::endl;
		}

		std::cout << "Файл успешно создан!" << std::endl;

		out.close();

	}



};

int main()
{
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);
	setlocale(LC_ALL, "ru_RU.UTF-8");

	std::cout << "Привет! Это Список дел!";

	std::cout << "Нужно просто написать цифру для выбора" << std::endl;

	int num;

	Todo list;

	while (true)
	{
		std::cout << "1. Добавить задачу \n"
			<< "2. Показать все задачи \n"
			<< "3. Показать выполненные задачи \n"
			<< "4. показать невыполненные задачи \n"
			<< "5. Изменить задачу \n"
			<< "6. Отметить задачу выполненной \n"
			<< "7. Удалить задачу \n"
			<< "8. Найти задачу \n"
			<< "9. Создать файл \n"
			<< "10. Выйти \n"
			<< std::endl;

		std::cin >> num;

		switch (num)
		{
		case 1:
			list.addTask();
			std::cout << std::endl;
			break;
		case 2:
			list.showAllTasks();
			std::cout << std::endl;
			break;
		case 3:
			list.showCompletedTasks();
			std::cout << std::endl;
			break;
		case 4:
			list.showIsNotCompletedTasks();
			std::cout << std::endl;
			break;
		case 5:
			list.changeTask();
			std::cout << std::endl;
			break;
		case 6:
			list.taskCompleted();
			std::cout << std::endl;
			break;
		case 7:
			list.dropTask();
			std::cout << std::endl;
			break;
		case 8:
			list.findTask();
			std::cout << std::endl;
			break;
		case 9:
			list.addToFile();
			std::cout << std::endl;
			break;
		case 10:
			std::cout << "До свидания!" << std::endl;
			std::cout << std::endl;
			return 0;
		default:
			std::cout << "Неверный пункт попробуйте еще раз!" << std::endl;
		}

	}
}