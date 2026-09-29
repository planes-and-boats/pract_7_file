#include <iostream>
#include <string>
#include <exception>
#include <fstream>

using namespace std;

#define DEBUG_MODE
#define DEBUG_TEST
#define FILE_NAME "expenses.txt"

enum typeExpense
{
	Income, Expense
};

struct ExpenseRecord
{
	string date = "";
	string category = "";
	string description = "";
	double amount = 0;
	typeExpense type = typeExpense::Expense;
};

static void PrintInfo(ExpenseRecord& record)
{
	cout << "Дата: " << record.date << " | Категория: " << record.category << endl;
	cout << "Описание: " << record.description << endl;
	cout << "Сумма: " << record.amount;
	cout << " | Тип операции: " << (record.type == typeExpense::Income ? "Доход" : "Расход") << endl;
}

static ExpenseRecord LineToRecord(string line)
{
	if (line.empty())
		throw runtime_error("строка пуста!");

	string data[5];
	for (int i = 0; i < 5; i++)
	{
		size_t pos = line.find(";");
		if (pos == string::npos && i == 4)
		{
			data[i] = line;
			break;
		}
		else if (pos == string::npos)
		{
			throw logic_error("недостаточно данных в строке");
		}
		data[i] = line.substr(0, pos);
		if (data[i].empty())
			throw invalid_argument("значение в строчке пустует.");
		line.erase(0, pos + 1);
	}

	ExpenseRecord record;
	record.date = data[0];

	if (record.date.length() != 10 || record.date[2] != '.' || record.date[5] != '.')
		throw invalid_argument("Некорректный формат даты. Ожидается ДД.ММ.ГГГГ (например, 12.03.2023)");

	record.category = data[1];
	record.description = data[2];

	try { record.amount = stod(data[3]); }
	catch (exception) { throw (invalid_argument("не удалось преобразовать строку в число.")); }
	if (record.amount < 1)
		throw out_of_range("числовое значение за допустимым пределом, число не может быть меньше 1.");

	while (!data[4].empty() && (data[4].back() == '\r' || data[4].back() == '\n' || data[4].back() == ' ')) {
		data[4].pop_back();
	}

	if (data[4] == "доход") {
		record.type = typeExpense::Income;
	}
	else if (data[4] == "расход") {
		record.type = typeExpense::Expense;
	}
	else {
		throw invalid_argument("некорректный тип операции в файле. Считано: '" + data[4] + "'");
	}
	/*else {
		throw invalid_argument("некорректный тип операции в файле.");
	}*/

	return record;
}


static void PrintHistory(string fileName)
{
	int count = 0;

	ifstream in(fileName);
	if (!in.is_open())
		throw runtime_error("не удалось открыть файл.");

	string line;
	while (getline(in, line))
	{
		if (line.empty())
			continue;
		count++;
		ExpenseRecord record = LineToRecord(line);
		PrintInfo(record);
		cout << endl;
	}
	in.close();

#ifdef DEBUG_MODE
	cout << endl << "[DEBUG] Прочитано строк при выводе истории: " << count << endl << endl;
#endif
}

static void CalculateFinances(string fileName)
{
	double income = 0;
	double outcome = 0;
	int count = 0;

	ifstream in(fileName);
	if (!in.is_open())
		throw runtime_error("не удалось открыть файл.");

	string line;
	while (getline(in, line))
	{
		if (line.empty())
			continue;
		count++;
		ExpenseRecord record = LineToRecord(line);
		if (record.type == typeExpense::Income)
			income += record.amount;
		else
			outcome += record.amount;
	}
	in.close();

#ifdef DEBUG_MODE
	cout << endl << "[DEBUG] Прочитано строк при расчете финансов: " << count << endl << endl;
#endif

	cout << "< Финансы >" << endl;
	cout << "\t| Доход: " << income << endl;
	cout << "\t| Расход: " << outcome << endl;
	cout << "\t| Тек. баланс: " << income - outcome << endl;
}

static void PrintByCategory(string fileName, string targetCategory)
{
	/*while (!targetCategory.empty() && (targetCategory.back() == '\r' || targetCategory.back() == '\n'
		|| targetCategory.back() == ' ')) {
		targetCategory.pop_back();
	}*/

	int count = 0;
	int countByCat = 0;

	ifstream in(fileName);
	if (!in.is_open())
		throw runtime_error("не удалось открыть файл.");

	string line;
	while (getline(in, line))
	{
		if (line.empty())
			continue;
		count++;
		ExpenseRecord record = LineToRecord(line);
		if (record.category == targetCategory)
		{
			PrintInfo(record);
			cout << endl;
			countByCat++;
		}
	}
	in.close();

	if (countByCat == 0)
		cout << "Нет подходящих записей." << endl;

#ifdef DEBUG_MODE
	cout << endl << "[DEBUG] Прочитано строк при фильтрации расходов по типу: " << count << endl << endl;
#endif
}

static void SearchByDescription(string fileName, string subString)
{

	int count = 0;
	int countByDes = 0;

	ifstream in(fileName);
	if (!in.is_open())
		throw runtime_error("не удалось открыть файл.");

	string line;
	while (getline(in, line))
	{
		if (line.empty())
			continue;
		count++;
		ExpenseRecord record = LineToRecord(line);
		if (record.description.find(subString) != string::npos)
		{
			PrintInfo(record);
			cout << endl;
			countByDes++;
		}
	}
	in.close();

	if (countByDes == 0)
		cout << "Нет подходящих записей." << endl;

#ifdef DEBUG_MODE
	cout << endl << "[DEBUG] Прочитано строк при фильтрации расходов по типу: " << count << endl << endl;
#endif
}

static void SaveToFile(string fileName, ExpenseRecord& record)
{
	ofstream out(fileName, ios::app);
	if (!out.is_open())
		throw runtime_error("не удалось открыть файл для записи.");

	out << record.date << ";" << record.category << ";" << record.description << ";" << record.amount << ";";
	out << (record.type == typeExpense::Income ? "доход" : "расход") << "\n";
	out.close();

#ifdef DEBUG_MODE
	cout << endl << "[DEBUG] Запись успешно добавлена в файл" << endl << endl;
#endif
}


int main()
{
	try
	{
		setlocale(LC_ALL, "Russian");

		int choice;

		do {
			cout << endl << "--- < Учет расходов > ---" << endl << endl;
			cout << "1. Выыод истории" << endl;
			cout << "2. Расчет доходов, расходов и баланса" << endl;
			cout << "3. Вывод операций определенной категории" << endl;
			cout << "4. Поиск операций по описанию" << endl;
			cout << "5. Добавить новую операцию" << endl;
			cout << "0. Выход из программы" << endl << endl;

			cout << "Выберите действие: ";
			cin >> choice;
			cin.ignore();
			cout << endl;

			switch (choice)
			{
			case 1:
			{
				PrintHistory(FILE_NAME);
				break;
			}
			case 2:
			{
				CalculateFinances(FILE_NAME);
				break;
			}
			case 3:
			{
#ifdef DEBUG_TEST
				cout << "Категория: Продукты" << endl << endl;
				PrintByCategory(FILE_NAME, "Продукты");
#else
				string cat;
				cout << "Введите категорию: ";
				getline(cin, cat);
				PrintByCategory(FILE_NAME, cat);
#endif
				break;
			}
			case 4:
			{
#ifdef DEBUG_TEST
				cout << "Описание: молоко" << endl << endl;
				SearchByDescription(FILE_NAME, "молоко");
#else
				string desc;
				cout << "Введите описание: ";
				getline(cin, desc);
				cout << endl;
				SearchByDescription(FILE_NAME, desc);
#endif
				break;
			}
			case 5:
			{
#ifdef DEBUG_TEST
				cout << "Добавление дефолтной записи: " << endl << endl;
				ExpenseRecord record = { "29.02.2005", "Продукты", "батончик", 50, typeExpense::Expense };
				PrintInfo(record);
				SaveToFile(FILE_NAME, record);
				cout << "Запись добавлена!" << endl;
#else
				ExpenseRecord record;
				cout << "Введите дату (ввида 12.02.2023): ";
				cin >> record.date;
				if (record.date.length() != 10 || record.date[2] != '.' || record.date[5] != '.')
					throw invalid_argument("Некорректный формат даты. Ожидается ДД.ММ.ГГГГ (например, 12.03.2023)");
				cin.ignore();
				do
				{
					cout << "Введите категорию: ";
					cin.ignore();
					getline(cin, record.category);
				} while (record.category.empty());
				do
				{
					cout << "Введите описание: ";
					cin.ignore();
					getline(cin, record.description);
				} while (record.description.empty());
				cout << "Введите сумму (> 0): ";
				try { cin >> record.amount; }
				catch (exception) { throw invalid_argument("не удалось преобразовать строку в число."); }
				if (record.amount < 0)
					throw out_of_range("числовое значение за допустимым пределом, число не может быть меньше 1.");
				cout << "Выберите тип: \n1. доход\n2. расход" << endl;
				cin >> choice;
				if (choice == 1)
					record.type = typeExpense::Income;
				else if (choice == 2)
					record.type = typeExpense::Expense;
				else
					throw invalid_argument("неверный пункт меню!");
				SaveToFile(FILE_NAME, record);
				cout << endl << "Запись добавлена!" << endl;
#endif
				break;
			}
			case 0:
			{
				cout << "Программа завершена." << endl;
				break;
			}
			default:
				cout << "Неверный пункт меню! Попробуйте еще раз." << endl;
				break;
			}
		} while (choice != 0);

	}
	catch (invalid_argument& e)
	{
		cout << "Некорректные данные: " << e.what() << endl;
	}
	catch (out_of_range& e)
	{
		cout << "За пределами: " << e.what() << endl;
	}
	catch (runtime_error& e)
	{
		cout << "Ошибка работы программы: " << e.what() << endl;
	}
	catch (exception& e)
	{
		cout << "Ошибка: " << e.what() << endl;
	}
}