#include <iostream>
#include <string>
#include <random>

// Базовый класс (без метода clone)
class Enemy {
public:
    std::string name;
    int hp;
    Enemy(std::string n, int h) : name(n), hp(h) {}
    virtual ~Enemy() = default;
    virtual void attack() const = 0;
};

// 10 конкретных врагов
class Goblin : public Enemy {
public:
    Goblin(std::string n, int h) : Enemy(n, h) {}
    void attack() const override { std::cout << "Goblin " << name << " attacks with a knife!\n"; }
};

class Dragon : public Enemy {
public:
    Dragon(std::string n, int h) : Enemy(n, h) {}
    void attack() const override { std::cout << "Dragon " << name << " breathes fire!\n"; }
};

class Skeleton : public Enemy {
public:
    Skeleton(std::string n, int h) : Enemy(n, h) {}
    void attack() const override { std::cout << "Skeleton " << name << " shoots a bone arrow!\n"; }
};

class Orc : public Enemy {
public:
    Orc(std::string n, int h) : Enemy(n, h) {}
    void attack() const override { std::cout << "Orc " << name << " swings a heavy axe!\n"; }
};

class Troll : public Enemy {
public:
    Troll(std::string n, int h) : Enemy(n, h) {}
    void attack() const override { std::cout << "Troll " << name << " smashes with a club!\n"; }
};

class Zombie : public Enemy {
public:
    Zombie(std::string n, int h) : Enemy(n, h) {}
    void attack() const override { std::cout << "Zombie " << name << " bites!\n"; }
};

class Vampire : public Enemy {
public:
    Vampire(std::string n, int h) : Enemy(n, h) {}
    void attack() const override { std::cout << "Vampire " << name << " drains life!\n"; }
};

class Golem : public Enemy {
public:
    Golem(std::string n, int h) : Enemy(n, h) {}
    void attack() const override { std::cout << "Golem " << name << " punches with a stone fist!\n"; }
};

class Imp : public Enemy {
public:
    Imp(std::string n, int h) : Enemy(n, h) {}
    void attack() const override { std::cout << "Imp " << name << " throws a fireball!\n"; }
};

class Wraith : public Enemy {
public:
    Wraith(std::string n, int h) : Enemy(n, h) {}
    void attack() const override { std::cout << "Wraith " << name << " screams terrifyingly!\n"; }
};

// ОТСУТСТВИЕ ПАТТЕРНА: ручная проверка типов
Enemy* copyEnemyWithoutPattern(Enemy* original) {
    if (original == nullptr) return nullptr;

    // Проблема: клиентский код вынужден знать ВСЕ конкретные типы. 
    // При добавлении 11-го врага придется дописывать код здесь.
    if (auto g = dynamic_cast<Goblin*>(original)) return new Goblin(*g);
    else if (auto d = dynamic_cast<Dragon*>(original)) return new Dragon(*d);
    else if (auto s = dynamic_cast<Skeleton*>(original)) return new Skeleton(*s);
    else if (auto o = dynamic_cast<Orc*>(original)) return new Orc(*o);
    else if (auto t = dynamic_cast<Troll*>(original)) return new Troll(*t);
    else if (auto z = dynamic_cast<Zombie*>(original)) return new Zombie(*z);
    else if (auto v = dynamic_cast<Vampire*>(original)) return new Vampire(*v);
    else if (auto g = dynamic_cast<Golem*>(original)) return new Golem(*g);
    else if (auto i = dynamic_cast<Imp*>(original)) return new Imp(*i);
    else if (auto w = dynamic_cast<Wraith*>(original)) return new Wraith(*w);
    
    std::cout << "ERROR: Unknown enemy type!\n";
    return nullptr;
}

int main() {
    std::cout << "--- WITHOUT PATTERN ---\n";
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 9);
    
    Enemy* enemy = nullptr;
    switch (dist(gen)) {
        case 0: enemy = new Goblin("Toothy", 100); break;
        case 1: enemy = new Dragon("Smaug", 1000); break;
        case 2: enemy = new Skeleton("Bones", 50); break;
        case 3: enemy = new Orc("Grunt", 150); break;
        case 4: enemy = new Troll("Smash", 300); break;
        case 5: enemy = new Zombie("Walker", 80); break;
        case 6: enemy = new Vampire("Dracula", 200); break;
        case 7: enemy = new Golem("Rocky", 500); break;
        case 8: enemy = new Imp("Sparky", 40); break;
        case 9: enemy = new Wraith("Ghost", 120); break;
    }

    std::cout << "Spawned an enemy: " << enemy->name << " (HP: " << enemy->hp << ")\n";
    enemy->hp -= 30; 
    std::cout << "Enemy took damage, HP left: " << enemy->hp << "\n";

    std::cout << "5 seconds passed, spawning clone...\n";
    Enemy* clone = copyEnemyWithoutPattern(enemy);
    
    if (clone) {
        std::cout << "Clone appeared: " << clone->name << " with HP: " << clone->hp << "\n";
    }

    delete enemy;
    delete clone;
    return 0;
}