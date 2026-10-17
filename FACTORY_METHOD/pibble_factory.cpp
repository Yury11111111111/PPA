#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Pibble {
protected:
    std::string name;
    int happiness = 0;

public:
    explicit Pibble(std::string n) : name(std::move(n)) {}
    virtual ~Pibble() = default;

    virtual std::string greet() const = 0;

    virtual std::string play(const std::string& toy) = 0;

    std::string treat() {
        happiness += 2;
        return name + " аккуратно берет печеньку и виляет всем телом";
    }

    int getHappiness() const { return happiness; }
    const std::string& getName() const { return name; }
};

class CouchPibble : public Pibble {
public:
    using Pibble::Pibble; 

    std::string greet() const override {
        return name + " медленно подходит и кладет голову вам на колени";
    }

    std::string play(const std::string& toy) override {
        happiness += 1;
        return name + " смотрит на " + toy + ", вздыхает и засыпает рядом";
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
        return name + " хватает " + toy + " и делает круги по двору (всего: "
               + std::to_string(laps) + ")";
    }
};

class PuppyPibble : public Pibble {
    std::vector<std::string> learned;
public:
    using Pibble::Pibble;

    std::string greet() const override {
        return name + " спотыкается о собственные лапы и радостно пищит";
    }

    std::string play(const std::string& toy) override {
        learned.push_back(toy);
        happiness += 2;
        return name + " впервые увидел " + toy + "! Знакомых игрушек: "
               + std::to_string(learned.size());
    }
};


class Shelter {
    int adopted = 0;
    int nextId = 1;

public:
    virtual ~Shelter() = default;

    void adoptionDay(const std::string& family, const std::vector<std::string>& toys) {
        std::cout << "\n=== " << shelterName() << ": в гости пришла семья "
                  << family << " ===\n";

        std::unique_ptr<Pibble> pibble = createPibble(pickName());

        std::cout << "  " << pibble->greet() << '\n';
        for (const std::string& toy : toys)
            std::cout << "  " << pibble->play(toy) << '\n';
        std::cout << "  " << pibble->treat() << '\n';

        if (pibble->getHappiness() >= 5) {
            ++adopted;
            std::cout << "  ♡ " << pibble->getName() << " уезжает домой с семьей "
                      << family << " (счастье: " << pibble->getHappiness() << ")\n";
        } else {
            std::cout << "  " << pibble->getName()
                      << " пока стесняется, встретятся ещё раз (счастье: "
                      << pibble->getHappiness() << ")\n";
        }
    }

    int adoptedCount() const { return adopted; }

protected:
    virtual std::unique_ptr<Pibble> createPibble(const std::string& name) const = 0;
    virtual std::string shelterName() const = 0;
    virtual std::vector<std::string> names() const = 0;

private:
    std::string pickName() {
        std::vector<std::string> list = names();
        return list[(nextId++ - 1) % list.size()];
    }
};

class CozyShelter : public Shelter {
protected:
    std::unique_ptr<Pibble> createPibble(const std::string& name) const override {
        return std::make_unique<CouchPibble>(name);
    }
    std::string shelterName() const override { return "Приют «Теплый плед»"; }
    std::vector<std::string> names() const override { return {"Пельмень", "Булка"}; }
};

class ParkShelter : public Shelter {
protected:
    std::unique_ptr<Pibble> createPibble(const std::string& name) const override {
        return std::make_unique<ZoomiesPibble>(name);
    }
    std::string shelterName() const override { return "Приют «Большая поляна»"; }
    std::vector<std::string> names() const override { return {"Ракета", "Турбо"}; }
};

class PuppyShelter : public Shelter {
protected:
    std::unique_ptr<Pibble> createPibble(const std::string& name) const override {
        return std::make_unique<PuppyPibble>(name);
    }
    std::string shelterName() const override { return "Щенячий приют «Лапки»"; }
    std::vector<std::string> names() const override { return {"Кексик", "Фасолька"}; }
};

int main() {
    CozyShelter cozy;
    cozy.adoptionDay("Пупупу", {"плюшевую уточку"});
    cozy.adoptionDay("Петровых", {"мячик", "канат", "косточку"});

    ParkShelter park;
    park.adoptionDay("Смирновых", {"фрисби", "мячик"});

    PuppyShelter puppies;
    puppies.adoptionDay("Кузнецовых", {"пищалку", "носок"});

    std::cout << "\nИтого домой уехали: "
              << cozy.adoptedCount() + park.adoptedCount() + puppies.adoptedCount()
              << " пиббла\n";
}
