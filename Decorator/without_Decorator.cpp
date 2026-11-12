#include <iostream>
#include <string>
#include <chrono>
#include <stdexcept>

struct TelemetryConfig {
    bool validate  = false;
    bool toJson    = false;
    bool compress  = false;
    bool encrypt   = false;
    bool addJwt    = false;
    bool log       = false;
    bool metrics   = false;
    bool rateLimit = false;
};

// God Class: знает и умеет всё сразу.
class MonolithicTelemetrySender {
    int requestCount = 0;

    static std::string escapeJson(const std::string& s) {
        std::string res;
        for (char c : s) {
            switch (c) {
                case '"':  res += "\\\""; break;
                case '\\': res += "\\\\"; break;
                default:   res += c;
            }
        }
        return res;
    }

    static std::string rleCompress(const std::string& data) {
        std::string comp;
        for (size_t i = 0; i < data.size(); ++i) {
            int count = 1;
            while (i + 1 < data.size() && data[i] == data[i + 1]) { count++; i++; }
            comp += std::to_string(count);
            comp += data[i];
        }
        return comp;
    }

    static std::string toHex(const std::string& data) {
        static const char* digits = "0123456789abcdef";
        std::string out;
        out.reserve(data.size() * 2);
        for (unsigned char c : data) {
            out += digits[c >> 4];
            out += digits[c & 0x0F];
        }
        return out;
    }

public:
    void Send(const std::string& rawLog, const TelemetryConfig& cfg) {
        auto start = std::chrono::high_resolution_clock::now();
        std::string data = rawLog;

        // Порядок зашит в код.
        if (cfg.rateLimit) {
            if (++requestCount > 3) throw std::runtime_error("Rate limit exceeded!");
        }

        if (cfg.validate) {
            if (data.empty() || data.find("ERROR") != std::string::npos) {
                throw std::invalid_argument("Validation failed: log contains ERROR!");
            }
        }

        if (cfg.toJson)   data = "{ \"log\": \"" + escapeJson(data) + "\" }";
        if (cfg.compress) data = rleCompress(data);

        if (cfg.encrypt) {
            for (char& c : data) c = static_cast<char>(c ^ 0x5A);
        }

        if (cfg.addJwt) data = "header.payload." + toHex(data);

        if (cfg.log) std::cout << "[LOG] Размер: " << data.size() << " байт\n";

        std::cout << "[NETWORK] Отправлено: " << data << "\n";

        if (cfg.metrics) {
            auto end = std::chrono::high_resolution_clock::now();
            auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            std::cout << "[METRICS] Время: " << us << " мкс\n";
        }
    }

    void SendForDebug(const std::string& rawLog) {
        std::string data = rawLog;
        if (data.empty()) throw std::invalid_argument("empty log");

        data = "{ \"log\": \"" + escapeJson(data) + "\" }";

        std::cout << "[LOG] Размер: " << data.size() << " байт\n";
        std::cout << "[NETWORK] Отправлено: " << data << "\n";
    }
};

int main() {
    MonolithicTelemetrySender sender;

    std::cout << "=== Монолит: полный пайплайн ===\n";
    TelemetryConfig cfg;
    cfg.validate  = true;
    cfg.toJson    = true;
    cfg.compress  = true;
    cfg.encrypt   = true;
    cfg.addJwt    = true;
    cfg.log       = true;
    cfg.metrics   = true;
    cfg.rateLimit = true;

    try {
        sender.Send("User clicked button AAABBB", cfg);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
    }

    std::cout << "\n=== Монолит: отдельный debug-канал ===\n";
    try {
        sender.SendForDebug("cache hit");
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
    }

    return 0;
}