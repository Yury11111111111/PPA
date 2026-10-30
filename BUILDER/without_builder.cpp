#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class HamsterHome {
    std::string name_;
    int width_, length_, bedding_depth_, wheel_diameter_;
    bool has_water_, has_sand_bath_;
    std::vector<std::string> hideouts_, tunnels_;

public:
    HamsterHome(std::string name, int width, int length, int bedding_depth,
                int wheel_diameter, bool has_water, bool has_sand_bath,
                std::vector<std::string> hideouts, std::vector<std::string> tunnels)
        : name_(std::move(name)), width_(width), length_(length),
          bedding_depth_(bedding_depth), wheel_diameter_(wheel_diameter),
          has_water_(has_water), has_sand_bath_(has_sand_bath),
          hideouts_(std::move(hideouts)), tunnels_(std::move(tunnels)) {
        if (name_.empty() || width_ <= 0 || length_ <= 0 || bedding_depth_ <= 0 ||
            wheel_diameter_ <= 0 || !has_water_ || hideouts_.empty()) {
            throw std::invalid_argument("Нужны имя, размеры, подстилка, колесо, вода и укрытие");
        }
        if (wheel_diameter_ > width_ || wheel_diameter_ > length_) {
            throw std::invalid_argument("Колесо не помещается по правилам модели");
        }
        for (const auto& item : hideouts_) {
            if (item.empty()) throw std::invalid_argument("Укрытие должно иметь название");
        }
        for (const auto& item : tunnels_) {
            if (item.empty()) throw std::invalid_argument("Тоннель должен иметь название");
        }
    }

    void print() const {
        std::cout << name_ << ": основание " << width_ << 'x' << length_
                  << ", подстилка " << bedding_depth_ << ", колесо " << wheel_diameter_
                  << ", вода: " << (has_water_ ? "да" : "нет")
                  << ", песочная ванна: " << (has_sand_bath_ ? "да" : "нет") << '\n';
        for (const auto& item : hideouts_) std::cout << "  Укрытие: " << item << '\n';
        for (const auto& item : tunnels_) std::cout << "  Тоннель: " << item << '\n';
    }
};

int main() {
    const HamsterHome basic("Домик Плюши", 100, 50, 20, 28,
                            true, false, {"Деревянный домик"}, {});
    const HamsterHome tunnels("Домик Бублика", 100, 50, 20, 28,
                              true, true, {"Деревянный домик", "Керамическое укрытие"},
                              {"Пробковый тоннель", "Картонный тоннель"});
    basic.print();
    tunnels.print();
    return 0;
}
