#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class HamsterHomeBuilder;

class HamsterHome {
    friend class HamsterHomeBuilder;

    HamsterHome(std::string name, int width, int length)
        : name_(std::move(name)), width_(width), length_(length) {}

    std::string name_;
    int width_, length_;
    int bedding_depth_ = 0;
    int wheel_diameter_ = 0;
    bool has_water_ = false;
    bool has_sand_bath_ = false;
    std::vector<std::string> hideouts_, tunnels_;

public:
    void print() const {
        std::cout << name_ << ": основание " << width_ << 'x' << length_
                  << ", подстилка " << bedding_depth_ << ", колесо " << wheel_diameter_
                  << ", вода: " << (has_water_ ? "да" : "нет")
                  << ", песочная ванна: " << (has_sand_bath_ ? "да" : "нет") << '\n';
        for (const auto& item : hideouts_) std::cout << "  Укрытие: " << item << '\n';
        for (const auto& item : tunnels_) std::cout << "  Тоннель: " << item << '\n';
    }
};

class HamsterHomeBuilder {
    HamsterHome home_;

public:
    HamsterHomeBuilder(std::string name, int width, int length)
        : home_(std::move(name), width, length) {}

    HamsterHomeBuilder& add_bedding(int depth) {
        home_.bedding_depth_ = depth;
        return *this;
    }
    HamsterHomeBuilder& add_wheel(int diameter) {
        home_.wheel_diameter_ = diameter;
        return *this;
    }
    HamsterHomeBuilder& add_water_bottle() {
        home_.has_water_ = true;
        return *this;
    }
    HamsterHomeBuilder& add_sand_bath() {
        home_.has_sand_bath_ = true;
        return *this;
    }
    HamsterHomeBuilder& add_hideout(std::string name) {
        if (name.empty()) throw std::invalid_argument("Укрытие должно иметь название");
        home_.hideouts_.push_back(std::move(name));
        return *this;
    }
    HamsterHomeBuilder& add_tunnel(std::string name) {
        if (name.empty()) throw std::invalid_argument("Тоннель должен иметь название");
        home_.tunnels_.push_back(std::move(name));
        return *this;
    }

    HamsterHome build() const {
        if (home_.name_.empty() || home_.width_ <= 0 || home_.length_ <= 0 ||
            home_.bedding_depth_ <= 0 || home_.wheel_diameter_ <= 0 ||
            !home_.has_water_ || home_.hideouts_.empty()) {
            throw std::invalid_argument("Нужны имя, размеры, подстилка, колесо, вода и укрытие");
        }
        if (home_.wheel_diameter_ > home_.width_ || home_.wheel_diameter_ > home_.length_) {
            throw std::invalid_argument("Колесо не помещается по правилам модели");
        }
        return home_;
    }
};

int main() {
    const HamsterHome basic = HamsterHomeBuilder("Домик Плюши", 100, 50)
        .add_bedding(20)
        .add_wheel(28)
        .add_water_bottle()
        .add_hideout("Деревянный домик")
        .build();

    const HamsterHome tunnels = HamsterHomeBuilder("Домик Бублика", 100, 50)
        .add_bedding(20)
        .add_wheel(28)
        .add_water_bottle()
        .add_hideout("Деревянный домик")
        .add_sand_bath()
        .add_hideout("Керамическое укрытие")
        .add_tunnel("Пробковый тоннель")
        .add_tunnel("Картонный тоннель")
        .build();

    basic.print();
    tunnels.print();
    return 0;
}
