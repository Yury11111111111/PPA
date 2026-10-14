#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <memory>

// Абстракции (DIP, ISP)
class IDataSource {
public:
    virtual std::vector<int> readAll() = 0;
    virtual ~IDataSource() = default;
};

class ISorter {
public:
    virtual void sort(std::vector<int>& data) = 0;
    virtual ~ISorter() = default;
};

class IOutput {
public:
    virtual void write(const std::vector<int>& data) = 0;
    virtual ~IOutput() = default;
};

// SRP
class FileSource : public IDataSource {
    std::string filename_;
public:
    explicit FileSource(std::string filename) : filename_(std::move(filename)) {}

    std::vector<int> readAll() override {
        std::ifstream file(filename_);
        if (!file) throw std::runtime_error("Can't open " + filename_);

        std::vector<int> data;
        int x;
        while (file >> x) data.push_back(x);
        return data;
    }
};

class QuickSorter : public ISorter {
    static int partition(std::vector<int>& a, int low, int high) {
        int pivot = a[high];
        int i = low - 1;
        for (int j = low; j < high; ++j)
            if (a[j] < pivot) std::swap(a[++i], a[j]);
        std::swap(a[i + 1], a[high]);
        return i + 1;
    }
    static void rec(std::vector<int>& a, int low, int high) {
        if (low < high) {
            int p = partition(a, low, high);
            rec(a, low, p - 1);
            rec(a, p + 1, high);
        }
    }
public:
    void sort(std::vector<int>& data) override {
        if (!data.empty()) rec(data, 0, (int)data.size() - 1);
    }
};

class ConsoleOutput : public IOutput {
public:
    void write(const std::vector<int>& data) override {
        for (int x : data) std::cout << x << ' ';
        std::cout << '\n';
    }
};

// SRP, DIP
int main() {
    try {
        std::unique_ptr<IDataSource> source = std::make_unique<FileSource>("file.txt");
        std::unique_ptr<ISorter> sorter = std::make_unique<QuickSorter>();
        std::unique_ptr<IOutput> output = std::make_unique<ConsoleOutput>();

        auto data = source->readAll();
        sorter->sort(data);
        output->write(data);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}
