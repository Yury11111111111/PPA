#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <vector>
#include <fstream>

using namespace std;

// Интерфейс. Говорит "я умею сохранять строку", но не говорит как
// Всё, что нужно Logger - только этот метод
class IStorage
{
public:
    virtual void save(const string &data) = 0;
};

// Конкретное хранилище - база данных
// Реализует интерфейс, то есть умеет save
class Database : public IStorage
{
private:
    string connectionString;
    vector<string> records;

public:
    Database(const string &connectionString)
        : connectionString(connectionString) {}

    void save(const string &data) override
    {
        records.push_back(data);
        cout << "[DB:" << connectionString << "] INSERT INTO logs VALUES ('"
             << data << "');" << endl;
    }

    // Это деталь самой Database. Logger'у count() не нужен,
    // поэтому в интерфейс его не тащим
    size_t count() const
    {
        return records.size();
    }
};

// Второе хранилище - JSON-файл
// Тоже реализует IStorage, поэтому его можно подставить в Logger
class JsonFile : public IStorage
{
private:
    string filePath;
    vector<string> entries;

    string escape(const string &s) const
    {
        string result;
        result.reserve(s.size());
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

    void save(const string &data) override
    {
        entries.push_back(data);
        flush();

        cout << "[JSON:" << filePath << "] "
             << "{ \"message\": \"" << escape(data) << "\" }" << endl;
    }

    size_t count() const
    {
        return entries.size();
    }
};

// Логгер. Формирует запись и отдаёт её хранилищу
//
// Раньше (без DIP) он знал про Database и JsonFile напрямую
// Теперь он знает только про IStorage. Куда именно писать -
// не его дело. Это и есть DIP
class Logger
{
public:
    enum class Level
    {
        Info,
        Warning,
        Error
    };

private:
    // Тип - абстракция, а не конкретный класс
    // Вот тут происходит инверсия зависимостей
    IStorage &storage;

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
    // Хранилище приходит снаружи. Logger его не создаёт сам
    // Это называется "внедрение зависимости"
    Logger(IStorage &storage) : storage(storage) {}

    void log(const string &message, Level level = Level::Info)
    {
        string entry = "[" + currentTimestamp() + "] [" + levelToString(level) + "] " + message;

        // Во время выполнения тут вызовется либо Database::save,
        // либо JsonFile::save - смотря что подсунули в конструктор
        storage.save(entry);
    }

    void info(const string &message) { log(message, Level::Info); }
    void warn(const string &message) { log(message, Level::Warning); }
    void error(const string &message) { log(message, Level::Error); }
};

int main()
{
    // Только здесь мы знаем про конкретные классы
    // Logger, IStorage и всё остальное про них не знают
    Database db("logs.db");
    JsonFile json("logs.json");

    // Один и тот же Logger, разные хранилища
    // Сам Logger при этом ни капли не изменился
    Logger dbLogger(db);
    Logger jsonLogger(json);

    // Захотим добавить CloudStorage - просто напишем новый класс
    // и добавим одну строчку здесь. Logger не трогаем

    cout << "=== Writing to Database ===" << endl;
    dbLogger.info("Application started");
    dbLogger.warn("Cache is almost full");

    cout << "\n=== Writing to JSON file ===" << endl;
    jsonLogger.info("Application started");
    jsonLogger.warn("Cache is almost full");
    jsonLogger.error("Failed to connect to payment service");

    // count() вызываем через конкретные объекты, потому что в интерфейсе его нет
    // Logger'у он и не нужен
    cout << "\nTotal DB records:   " << db.count() << endl;
    cout << "Total JSON records: " << json.count() << endl;

    return 0;
}