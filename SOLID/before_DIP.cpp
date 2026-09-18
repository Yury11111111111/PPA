#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <vector>
#include <fstream>

using namespace std;

// Конкретное хранилище - база данных
// Никакого интерфейса, просто класс сам по себе
class Database
{
private:
    string connectionString;
    vector<string> records;

public:
    Database(const string &connectionString)
        : connectionString(connectionString) {}

    void save(const string &data)
    {
        records.push_back(data);
        cout << "[DB:" << connectionString << "] INSERT INTO logs VALUES ('"
             << data << "');" << endl;
    }

    size_t size() const
    {
        return records.size();
    }
};

// Второе хранилище - JSON-файл
// Тоже сам по себе, никак не связан с Database
class JsonFile
{
private:
    string filePath;
    vector<string> entries;

    // Экранирование кавычек и слэшей, чтобы JSON не ломался
    string escape(const string &s) const
    {
        string result;
        for (char c : s)
        {
            switch (c)
            {
            case '"':
                result += "\\\"";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\t':
                result += "\\t";
                break;
            case '\r':
                result += "\\r";
                break;
            default:
                result += c;
                break;
            }
        }
        return result;
    }

    // Перезаписывает файл целиком - получается валидный JSON-массив
    void flush() const
    {
        ofstream out(filePath);
        if (!out.is_open())
        {
            cerr << "[JSON] Cannot open file: " << filePath << endl;
            return;
        }
        out << "[\n";
        for (size_t i = 0; i < entries.size(); ++i)
        {
            out << "  { \"message\": \"" << escape(entries[i]) << "\" }";
            if (i + 1 < entries.size())
                out << ",";
            out << "\n";
        }
        out << "]\n";
    }

public:
    JsonFile(const string &filePath)
        : filePath(filePath) {}

    void save(const string &data)
    {
        entries.push_back(data);
        flush();
        cout << "[JSON:" << filePath << "] { \"message\": \""
             << escape(data) << "\" }" << endl;
    }

    size_t size() const
    {
        return entries.size();
    }
};

// Логгер, который пишет ТОЛЬКО в базу
// Знает про конкретный Database. Про JsonFile не знает
// Логика формирования записи (timestamp, level) тут своя
//
// НАРУШЕНИЕ DIP: высокоуровневая логика зависит от конкретного низкоуровнего класса, а не от абстракции
class DatabaseLogger
{
public:
    enum class Level
    {
        Info,
        Warning,
        Error
    };

private:
    // Конкретный тип, тут DIP нарушен
    Database &db;

    string currentTimestamp() const
    {
        auto now = chrono::system_clock::now();
        time_t t = chrono::system_clock::to_time_t(now);
        char buffer[32];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&t));
        return string(buffer);
    }

    string levelToString(Level level) const
    {
        switch (level)
        {
        case Level::Info:
            return "INFO";
        case Level::Warning:
            return "WARN";
        case Level::Error:
            return "ERROR";
        }
        return "UNKNOWN";
    }

public:
    DatabaseLogger(Database &db) : db(db) {}

    void log(const string &message, Level level = Level::Info)
    {
        string entry = "[" + currentTimestamp() + "] [" + levelToString(level) + "] " + message;
        db.save(entry);
    }

    void info(const string &message) { log(message, Level::Info); }
    void warn(const string &message) { log(message, Level::Warning); }
    void error(const string &message) { log(message, Level::Error); }
};

// Логгер, который пишет ТОЛЬКО в JSON
// Знает про конкретный JsonFile. Про Database не знает
// Логика формирования записи — ТА ЖЕ САМАЯ, скопированная
//
// НАРУШЕНИЕ DIP: опять зависит от конкретного класса
class JsonLogger
{
public:
    enum class Level
    {
        Info,
        Warning,
        Error
    };

private:
    // Конкретный тип - вот тут DIP нарушен
    JsonFile &json;

    string currentTimestamp() const
    {
        auto now = chrono::system_clock::now();
        time_t t = chrono::system_clock::to_time_t(now);
        char buffer[32];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&t));
        return string(buffer);
    }

    // ТА ЖЕ логика, что и в DatabaseLogger
    string levelToString(Level level) const
    {
        switch (level)
        {
        case Level::Info:
            return "INFO";
        case Level::Warning:
            return "WARN";
        case Level::Error:
            return "ERROR";
        }
        return "UNKNOWN";
    }

public:
    JsonLogger(JsonFile &json) : json(json) {}

    void log(const string &message, Level level = Level::Info)
    {
        string entry = "[" + currentTimestamp() + "] [" + levelToString(level) + "] " + message;
        json.save(entry);
    }

    void info(const string &message) { log(message, Level::Info); }
    void warn(const string &message) { log(message, Level::Warning); }
    void error(const string &message) { log(message, Level::Error); }
};

// ПРОБЛЕМА: чтобы добавить новое хранилище, недостаточно написать
// один класс хранилища. Придётся писать ещё и НОВЫЙ ЛОГГЕР,
// потому что каждый логгер жёстко привязан к конкретному типу.
//
// Пример: захотим писать логи в облако (CloudStorage).
// Придётся сделать:
//   1) class CloudStorage { void save(...); };
//   2) class CloudLogger { ... };  // копипаста текущих логгеров
//   3) не забыть поменять main
//
// Это следствие нарушения DIP: высокоуровневая логика
// (форматирование записи) приклеена к низкоуровневой детали
// (конкретному хранилищу).
//
// В after этого нет: там достаточно написать один класс
// CloudStorage : public IStorage, и Logger его уже умеет
// использовать без единой правки.


int main()
{
    // Создаём конкретные хранилища
    Database db("logs.db");
    JsonFile json("logs.json");

    // Каждое хранилище получает СВОЙ логгер
    // Общего логгера нет, потому что логгеры привязаны к конкретике
    DatabaseLogger dbLogger(db);
    JsonLogger jsonLogger(json);

    cout << "=== Writing to Database ===" << endl;
    dbLogger.info("Application started");
    dbLogger.warn("Cache is almost full");

    cout << "\n=== Writing to JSON file ===" << endl;
    jsonLogger.info("Application started");
    jsonLogger.warn("Cache is almost full");
    jsonLogger.error("Failed to connect to payment service");

    // size() вызываем через конкретные объекты
    // В логгерах его нет — он не нужен для записи
    cout << "\nTotal DB records:   " << db.size() << endl;
    cout << "Total JSON records: " << json.size() << endl;

    return 0;
}