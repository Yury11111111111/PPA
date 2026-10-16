#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>


// ч1. библиотека (менять нельзя)

namespace shelterlib {

class Pibble {
protected:
    std::string name;
    int happiness = 0;
    int days = 0;       

public:
    explicit Pibble(std::string n) : name(std::move(n)) {}
    virtual ~Pibble() = default;

    virtual std::string greet() const = 0;                 
    virtual std::string play(const std::string& toy) = 0;  


    virtual std::string liveDay() {
        ++days;
        return name + " смотрит на дверь и ждет свою семью";
    }

    std::string treat() {
        happiness += 2;
        return name + " берет печеньку и виляет всем телом";
    }

    int getHappiness() const { return happiness; }
    int getDays() const { return days; }
    const std::string& getName() const { return name; }
};

struct Visit {
    std::string family;
    std::vector<std::string> toys;
};

struct Day {
    std::vector<std::string> arrivals;   
    std::vector<Visit> visits;          
};


class Shelter {
    std::vector<std::unique_ptr<Pibble>> kennel;                      
    std::vector<std::pair<std::string, std::unique_ptr<Pibble>>> homes;  

public:
    virtual ~Shelter() = default;

    void run(const std::vector<Day>& schedule) {
        std::cout << "\n############ " << title() << " ##########\n";
        for (std::size_t d = 0; d < schedule.size(); ++d) {
            std::cout << "\n--- День " << d + 1 << " ---\n";
            receive(schedule[d].arrivals);
            for (const auto& p : kennel)
                std::cout << "  " << p->liveDay() << '\n';
            for (const Visit& v : schedule[d].visits)
                meet(v);
        }
        report();
    }

protected:
    virtual std::unique_ptr<Pibble> createPibble(const std::string& name) = 0;

    virtual std::string title() const = 0;
    virtual int adoptionThreshold() const { return 5; }  

private:
    void receive(const std::vector<std::string>& arrivals) {
        for (const std::string& name : arrivals) {
            std::unique_ptr<Pibble> p = createPibble(name);
            if (!p) {
                std::cout << "  " << name << ": мест нет, отвезли в соседний приют\n";
                continue;
            }
            std::cout << "  + Поступил(а) " << p->getName() << ". " << p->greet() << '\n';
            kennel.push_back(std::move(p));  
        }
    }

    void meet(const Visit& v) {
        if (kennel.empty()) {
            std::cout << "  Семья " << v.family << " пришла, но все пибблы уже дома!\n";
            return;
        }

        Pibble& p = *kennel.front(); 
        std::cout << "  Семья " << v.family << " знакомится с пибблом по имени "
                  << p.getName() << '\n';
        for (const std::string& toy : v.toys)
            std::cout << "    " << p.play(toy) << '\n';
        std::cout << "    " << p.treat() << '\n';

        if (p.getHappiness() >= adoptionThreshold()) {
            std::cout << "    ♡ " << p.getName() << " уезжает домой! (счастье "
                      << p.getHappiness() << ", ждал(а) дней: " << p.getDays() << ")\n";

            homes.emplace_back(v.family, std::move(kennel.front()));
            kennel.erase(kennel.begin());
        } else {
            std::cout << "    " << p.getName() << " пока стесняется (счастье "
                      << p.getHappiness() << " из " << adoptionThreshold()
                      << "), встретятся еще раз\n";
        }
    }

    void report() const {
        std::cout << "\n  Итог. Нашли дом:\n";
        if (homes.empty()) std::cout << "    пока никто\n";
        for (const auto& [family, p] : homes)
            std::cout << "    " << p->getName() << " — у семьи " << family << '\n';

        std::cout << "  Еще ждут в приюте:\n";
        if (kennel.empty()) std::cout << "    никто, приют пуст\n";
        for (const auto& p : kennel)
            std::cout << "    " << p->getName() << " (счастье " << p->getHappiness()
                      << ", дней в приюте: " << p->getDays() << ")\n";
    }
};

}

// ч2

using shelterlib::Pibble;

class CouchPibble : public Pibble {
    bool hasCouch;
public:
    CouchPibble(std::string n, bool couch) : Pibble(std::move(n)), hasCouch(couch) {}

    std::string greet() const override {
        return name + (hasCouch ? " сразу забирается на свой диванчик"
                                : " ищет, где бы прилечь, но диванчиков больше нет");
    }

    std::string play(const std::string& toy) override {
        happiness += 1;
        return name + " смотрит на " + toy + ", вздыхает и засыпает рядом";
    }

    std::string liveDay() override {
        ++days;
        if (hasCouch) {
            happiness += 1;
            return name + " весь день дремлет на диванчике (+1 к счастью)";
        }
        return name + " грустит на полу без диванчика";
    }
};


class ZoomiesPibble : public Pibble {
    int laps = 0;
public:
    using Pibble::Pibble;

    std::string greet() const override {
        return name + " влетает в комнату и пытается облизать всех сразу";
    }

    std::string play(const std::string& toy) override {
        laps += 3;
        happiness += 3;
        return name + " хватает " + toy + " и наматывает круги (всего кругов: "
               + std::to_string(laps) + ")";
    }

    std::string liveDay() override {
        ++days;
        laps += 5;
        happiness += 1;
        return name + " с утра пробегает 5 кругов по поляне (+1 к счастью)";
    }
};

class PuppyPibble : public Pibble {
    std::vector<std::string> knownToys;
public:
    PuppyPibble(std::string n, std::string firstToy) : Pibble(std::move(n)) {
        knownToys.push_back(std::move(firstToy));
    }

    std::string greet() const override {
        return name + " спотыкается о лапы и прижимает к себе " + knownToys.front();
    }

    std::string play(const std::string& toy) override {
        for (const std::string& known : knownToys) {
            if (known == toy) {
                happiness += 3;
                return name + " узнает " + toy + " и визжит от восторга";
            }
        }
        knownToys.push_back(toy);
        happiness += 1;
        return name + " осторожно обнюхивает " + toy + " (знакомых игрушек: "
               + std::to_string(knownToys.size()) + ")";
    }

};


class CozyShelter : public shelterlib::Shelter {
    int freeBeds = 3;
    int freeCouches = 1;
protected:
    std::unique_ptr<Pibble> createPibble(const std::string& name) override {
        if (freeBeds == 0)
            return nullptr;                 // принять не можем — библиотека это обработает
        --freeBeds;
        bool couch = freeCouches > 0;
        if (couch) --freeCouches;
        return std::make_unique<CouchPibble>(name, couch);
    }
    std::string title() const override { return "Приют «Теплый плед»"; }
};


class ParkShelter : public shelterlib::Shelter {
protected:
    std::unique_ptr<Pibble> createPibble(const std::string& name) override {
        return std::make_unique<ZoomiesPibble>(name);
    }
    std::string title() const override { return "Приют «Большая поляна»"; }
    int adoptionThreshold() const override { return 10; }
};


class PuppyShelter : public shelterlib::Shelter {
    int nextNumber = 1;
    std::vector<std::string> donatedToys = {"плюшевую морковку", "резинового ежика"};
protected:
    std::unique_ptr<Pibble> createPibble(const std::string& name) override {
        std::string toy = donatedToys[(nextNumber - 1) % donatedToys.size()];
        std::string fullName = name + " №" + std::to_string(nextNumber);
        ++nextNumber;
        return std::make_unique<PuppyPibble>(fullName, toy);
    }
    std::string title() const override { return "Щенячий приют «Лапки»"; }
    int adoptionThreshold() const override { return 4; }  
};


int main() {
    CozyShelter cozy;
    cozy.run({
        {{"Пельмень", "Булка"}, {{"Ивановых", {"плюшевую уточку"}}}},
        {{"Ватрушка", "Сырник"}, {{"Петровых", {"мячик"}}}},
        {{}, {{"Смирновых", {"канат", "косточку"}}}},
    });

    ParkShelter park;
    park.run({
        {{"Ракета"}, {{"Кузнецовых", {"фрисби"}}}},
        {{"Турбо"}, {{"Кузнецовых", {"фрисби", "мячик"}}}},
    });

    PuppyShelter puppies;
    puppies.run({
        {{"Кексик", "Фасолька"}, {{"Волковых", {"пищалку"}}}},
        {{}, {{"Соколовых", {"плюшевую морковку"}}, {"Морозовых", {"резинового ежика"}}}},
    });
}
