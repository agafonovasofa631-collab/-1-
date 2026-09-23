#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;


// Класс Student (Часть) — существует независимо от University

class Student {
private:
    string name;          // имя студента
    string group;         // группа
    int    id;            // номер студенческого билета

    // Статическое поле — счётчик всех созданных студентов
    static int totalCount;

public:
    // Конструктор
    Student(const string& n, const string& g, int i)
        : name(n), group(g), id(i)
    {
        totalCount++;
        cout << "  [+] Создан студент: " << name
            << " (группа " << group << ", ID " << id << ")" << endl;
    }

    // Деструктор
    ~Student() {
        totalCount--;
        cout << "  [-] Удалён студент: " << name << endl;
    }

    // Геттеры
    string getName()  const { return name; }
    string getGroup() const { return group; }
    int    getId()    const { return id; }

    // Статический метод для получения количества студентов
    static int getTotalCount() { return totalCount; }

    // Вывод информации
    void print() const {
        cout << "    • " << name
            << " | группа: " << group
            << " | ID: " << id << endl;
    }
};

// Определение статического поля вне класса (обязательно!)
int Student::totalCount = 0;


// Класс University (Целое) — реализован как Singleton

class University {
private:
    string name;                       // название университета
    vector<Student*> students;         // агрегация: храним указатели

    // --- Singleton ---
    static University* instance;       // единственный экземпляр

    // Приватный конструктор — нельзя создать объект извне
    University(const string& n) : name(n) {
        cout << "Университет \"" << name << "\" открыт." << endl;
    }

    // Приватный деструктор
    ~University() {
        cout << "Университет \"" << name << "\" закрыт." << endl;
        // ВАЖНО: студентов НЕ удаляем — это агрегация!
        students.clear();
    }

public:
    // Запрещаем копирование и присваивание
    University(const University&) = delete;
    University& operator=(const University&) = delete;

    // Точка доступа к единственному экземпляру
    static University* getInstance(const string& n = "") {
        if (instance == nullptr) {
            if (n.empty()) {
                cerr << "Ошибка: первый вызов getInstance() "
                    "требует имени университета!" << endl;
                return nullptr;
            }
            instance = new University(n);
        }
        return instance;
    }

    // Освобождение ресурса Singleton (можно вызвать вручную)
    static void destroyInstance() {
        delete instance;
        instance = nullptr;
    }

    // Агрегация 

    // Добавить студента (в агрегации целое НЕ владеет частью)
    void addStudent(Student* s) {
        if (s == nullptr) {                       // проверка на nullptr
            cerr << "Ошибка: попытка добавить nullptr-студента!" << endl;
            return;
        }
        students.push_back(s);
        cout << "  → Студент \"" << s->getName()
            << "\" зачислен в \"" << name << "\"" << endl;
    }

    // Отчислить студента (просто убираем из списка, объект жив)
    void removeStudent(Student* s) {
        if (s == nullptr) return;
        auto it = find(students.begin(), students.end(), s);
        if (it != students.end()) {
            students.erase(it);
            cout << "  ← Студент \"" << s->getName()
                << "\" отчислен из \"" << name << "\"" << endl;
        }
    }

    // Вывести список всех студентов
    void printStudents() const {
        cout << "\nУниверситет \"" << name << "\" (всего: "
            << students.size() << "):" << endl;
        if (students.empty()) {
            cout << "    (список пуст)" << endl;
        }
        else {
            for (const auto* s : students) {
                s->print();
            }
        }
        cout << endl;
    }

    string getName() const { return name; }
};

// Определение статического поля Singleton
University* University::instance = nullptr;



// main — демонстрация работы

int main() {
    setlocale(LC_ALL, "ru");

    cout << "1.Создание студентов" << endl;
    // Студенты создаются НЕЗАВИСИМО от университета (агрегация)
    Student s1("Иванов Иван", "ИУ7-21Б", 101);
    Student s2("Петров Пётр", "ИУ7-22Б", 102);
    Student s3("Сидорова Мария", "ИУ7-23Б", 103);
    Student s4("Кузнецов Алексей", "ИУ7-24Б", 104);

    cout << "\nВсего создано студентов (статический счётчик): "
        << Student::getTotalCount() << endl;

    cout << "\n2.Получение Singleton университета" << endl;
    University* uni1 = University::getInstance("МГТУ им. Н.Э. Баумана");
    University* uni2 = University::getInstance();  // второй вызов — тот же объект

    cout << "uni1 == uni2 ? " << (uni1 == uni2 ? "ДА (один и тот же объект)" : "НЕТ")
        << endl;

    cout << "\n3.Зачисление студентов (агрегация)" << endl;
    uni1->addStudent(&s1);
    uni1->addStudent(&s2);
    uni1->addStudent(&s3);
    uni1->addStudent(nullptr);          // проверка на nullptr
    uni1->printStudents();

    cout << "4.Отчисление студента" << endl;
    uni1->removeStudent(&s2);
    uni1->printStudents();

    cout << "5.Демонстрация агрегации" << endl;
    // Закрываем университет 
    University::destroyInstance();
    uni1 = nullptr;
    uni2 = nullptr;

    cout << "\nПосле закрытия университета студентов всё ещё: "
        << Student::getTotalCount() << endl;

    cout << "Студенты продолжают существовать независимо:" << endl;
    s1.print();
    s3.print();
    s4.print();

    cout << "\n6.Конец main — удаление студентов" << endl;
    // s1..s4 удаляются автоматически (стек), счётчик уменьшается
    return 0;
}
