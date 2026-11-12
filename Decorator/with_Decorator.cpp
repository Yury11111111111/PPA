#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <stdexcept>

namespace utils {
    std::string escapeJson(const std::string& s) {
        std::string res;
        res.reserve(s.size());
        for (char c : s) {
            switch (c) {
                case '"':  res += "\\\""; break;
                case '\\': res += "\\\\"; break;
                default:   res += c;
            }
        }
        return res;
    }

    std::string rleCompress(const std::string& data) {
        std::string comp;
        for (size_t i = 0; i < data.size(); ++i) {
            int count = 1;
            while (i + 1 < data.size() && data[i] == data[i + 1]) { count++; i++; }
            comp += std::to_string(count);
            comp += data[i];
        }
        return comp;
    }

    std::string toHex(const std::string& data) {
        static const char* digits = "0123456789abcdef";
        std::string out;
        out.reserve(data.size() * 2);
        for (unsigned char c : data) {
            out += digits[c >> 4];
            out += digits[c & 0x0F];
        }
        return out;
    }

}

class ITelemetrySender {
public:
    virtual ~ITelemetrySender() = default;
    virtual void send(const std::string& data) = 0;
};

class NetworkSender : public ITelemetrySender {
public:
    void send(const std::string& data) override {
        std::cout << "[NETWORK] Отправлено (" << data.size()
                  << " байт): " << data << "\n";
    }
};

class SenderDecorator : public ITelemetrySender {
protected:
    std::unique_ptr<ITelemetrySender> wrappee_;
public:
    explicit SenderDecorator(std::unique_ptr<ITelemetrySender> sender)
        : wrappee_(std::move(sender)) {}

    void send(const std::string& data) override { wrappee_->send(data); }
};

class ValidationDecorator : public SenderDecorator {
public:
    using SenderDecorator::SenderDecorator;
    void send(const std::string& data) override {
        if (data.empty() || data.find("ERROR") != std::string::npos) {
            throw std::invalid_argument("Validation failed: log is empty or contains ERROR");
        }
        SenderDecorator::send(data);
    }
};

class RateLimitDecorator : public SenderDecorator {
    int count_ = 0;
    int limit_;
public:
    explicit RateLimitDecorator(std::unique_ptr<ITelemetrySender> sender, int limit = 3)
        : SenderDecorator(std::move(sender)), limit_(limit) {}

    void send(const std::string& data) override {
        if (++count_ > limit_) throw std::runtime_error("Rate limit exceeded!");
        SenderDecorator::send(data);
    }
};

class JsonDecorator : public SenderDecorator {
public:
    using SenderDecorator::SenderDecorator;
    void send(const std::string& data) override {
        SenderDecorator::send("{ \"log\": \"" + utils::escapeJson(data) + "\" }");
    }
};

class CompressionDecorator : public SenderDecorator {
public:
    using SenderDecorator::SenderDecorator;
    void send(const std::string& data) override {
        SenderDecorator::send(utils::rleCompress(data));
    }
};

class EncryptionDecorator : public SenderDecorator {
public:
    using SenderDecorator::SenderDecorator;
    void send(const std::string& data) override {
        std::string encrypted = data;
        for (char& c : encrypted) c = static_cast<char>(c ^ 0x5A);
        SenderDecorator::send(encrypted);
    }
};

class AuthTokenDecorator : public SenderDecorator {
public:
    using SenderDecorator::SenderDecorator;
    void send(const std::string& data) override {
        SenderDecorator::send("header.payload." + utils::toHex(data));
    }
};

class LoggingDecorator : public SenderDecorator {
public:
    using SenderDecorator::SenderDecorator;
    void send(const std::string& data) override {
        std::cout << "[LOG] Размер до отправки: " << data.size() << " байт\n";
        SenderDecorator::send(data);
    }
};

class MetricsDecorator : public SenderDecorator {
public:
    using SenderDecorator::SenderDecorator;
    void send(const std::string& data) override {
        auto t0 = std::chrono::high_resolution_clock::now();
        SenderDecorator::send(data);
        auto t1 = std::chrono::high_resolution_clock::now();
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
        std::cout << "[METRICS] Время: " << us << " мкс\n";
    }
};

std::unique_ptr<ITelemetrySender> buildFullPipeline() {
    std::unique_ptr<ITelemetrySender> s = std::make_unique<NetworkSender>();
    s = std::make_unique<LoggingDecorator>(std::move(s));
    s = std::make_unique<AuthTokenDecorator>(std::move(s));
    s = std::make_unique<EncryptionDecorator>(std::move(s));
    s = std::make_unique<CompressionDecorator>(std::move(s));
    s = std::make_unique<JsonDecorator>(std::move(s));
    s = std::make_unique<ValidationDecorator>(std::move(s));
    s = std::make_unique<RateLimitDecorator>(std::move(s));
    s = std::make_unique<MetricsDecorator>(std::move(s));
    return s;
}

std::unique_ptr<ITelemetrySender> buildEncryptThenCompressPipeline() {
    std::unique_ptr<ITelemetrySender> s = std::make_unique<NetworkSender>();
    s = std::make_unique<LoggingDecorator>(std::move(s));
    s = std::make_unique<AuthTokenDecorator>(std::move(s));
    s = std::make_unique<CompressionDecorator>(std::move(s));
    s = std::make_unique<EncryptionDecorator>(std::move(s));
    s = std::make_unique<JsonDecorator>(std::move(s));
    s = std::make_unique<MetricsDecorator>(std::move(s));
    return s;
}

std::unique_ptr<ITelemetrySender> buildCriticalPipeline() {
    std::unique_ptr<ITelemetrySender> s = std::make_unique<NetworkSender>();
    s = std::make_unique<AuthTokenDecorator>(std::move(s));
    s = std::make_unique<EncryptionDecorator>(std::move(s));
    s = std::make_unique<ValidationDecorator>(std::move(s));
    s = std::make_unique<RateLimitDecorator>(std::move(s));
    return s;
}

std::unique_ptr<ITelemetrySender> buildDebugPipeline() {
    std::unique_ptr<ITelemetrySender> s = std::make_unique<NetworkSender>();
    s = std::make_unique<LoggingDecorator>(std::move(s));
    s = std::make_unique<JsonDecorator>(std::move(s));
    return s;
}

int main() {
    std::cout << "=== Тест 1: полный пайплайн ===\n";
    {
        auto sender = buildFullPipeline();
        sender->send("User clicked button AAABBB");
    }

    std::cout << "\n=== Тест 2: смена порядка (encrypt -> compress) ===\n";
    {
        auto sender = buildEncryptThenCompressPipeline();
        sender->send("User clicked button AAABBB");
    }

    std::cout << "\n=== Тест 3: валидация обрывает цепочку ===\n";
    {
        auto sender = buildFullPipeline();
        try {
            sender->send("ERROR: something went wrong");
        } catch (const std::exception& e) {
            std::cerr << "Исключение: " << e.what() << "\n";
        }
    }

    std::cout << "\n=== Тест 4: rate limit ===\n";
    {
        auto sender = std::make_unique<RateLimitDecorator>(
                          std::make_unique<NetworkSender>());
        for (int i = 1; i <= 4; ++i) {
            std::cout << "-- отправка #" << i << "\n";
            try {
                sender->send("ping " + std::to_string(i));
            } catch (const std::exception& e) {
                std::cerr << "Исключение: " << e.what() << "\n";
            }
        }
    }

    std::cout << "\n=== Тест 5: разные каналы без правок декораторов ===\n";
    {
        auto critical = buildCriticalPipeline();
        auto debug    = buildDebugPipeline();
        std::cout << "[critical] "; critical->send("payment approved");
        std::cout << "[debug]    "; debug->send("cache hit");
    }

    return 0;
}