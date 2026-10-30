#include <iostream>
#include <string>
#include <random>

// Базовый класс
class Enemy {
public:
    std::string name;
    int hp;
    Enemy(std::string n, int h) : name(n), hp(h) {}
    virtual ~Enemy() = default;
    virtual void attack() const = 0;

    // ПАТТЕРН: виртуальный метод клонирования
    virtual Enemy* clone() const = 0; 
};

// 10 конкретных врагов
class Goblin : public Enemy {
public:
    Goblin(std::string n, int h) : Enemy(n, h) {}
    void attack() const override { std::cout << "Goblin " << name << " attacks with a knife!\n"; }
    Enemy* clone() const override { return new Goblin(*this); }
};

class Dragon : public Enemy {
public:
    Dragon(std::string n, int h) : Enemy(n, h) {}
    void attack() const override { std::cout << "Dragon " << name << " breathes fire!\n"; }
    Enemy* clone() const override { return new Dragon(*this); }
};

class Skeleton : public Enemy {
public:
    Skeleton(std::string n, int h) : Enemy(n, h) {}
    void attack() const override { std::cout << "Skeleton " << name << " shoots a bone arrow!\n"; }
    Enemy* clone() const override { return new Skeleton(*this); }
};

class Orc : public Enemy {
public:
    Orc(std::string n, int h) : Enemy(n, h) {}
    void attack() const override { std::cout << "Orc " << name << " swings a heavy axe!\n"; }
    Enemy* clone() const override { return new Orc(*this); }
};

class Troll : public Enemy {
public:
    Troll(std::string n, int h) : Enemy(n, h) {}
    void attack() const override { std::cout << "Troll " << name << " smashes with a club!\n"; }
    Enemy* clone() const override { return new Troll(*this); }
};

class Zombie : public Enemy {
public:
    Zombie(std::string n, int h) : Enemy(n, h) {}
    void attack() const override { std::cout << "Zombie " << name << " bites!\n"; }
    Enemy* clone() const override { return new Zombie(*this); }
};

class Vampire : public Enemy {
public:
    Vampire(std::string n, int h) : Enemy(n, h) {}
    void attack() const override { std::cout << "Vampire " << name << " drains life!\n"; }
    Enemy* clone() const override { return new Vampire(*this); }
};

class Golem : public Enemy {
public:
    Golem(std::string n, int h) : Enemy(n, h) {}
    void attack() const override { std::cout << "Golem " << name << " punches with a stone fist!\n"; }
    Enemy* clone() const override { return new Golem(*this); }
};

class Imp : public Enemy {
public:
    Imp(std::string n, int h) : Enemy(n, h) {}
    void attack() const override { std::cout << "Imp " << name << " throws a fireball!\n"; }
    Enemy* clone() const override { return new Imp(*this); }
};

class Wraith : public Enemy {
public:
    Wraith(std::string n, int h) : Enemy(n, h) {}
    void attack() const override { std::cout << "Wraith " << name << " screams terrifyingly!\n"; }
    Enemy* clone() const override { return new Wraith(*this); }
};

// С ПАТТЕРНОМ: полиморфное клонирование
Enemy* copyEnemyWithPattern(Enemy* original) {
    if (original == nullptr) return nullptr;
    
    // Решение: функция не зависит от конкретных классов.
    // Объект сам знает свой точный тип и создает свою копию.
    return original->clone(); 
}

int main() {
    std::cout << "--- WITH PATTERN (10 ENEMIES) ---\n";
    
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
    Enemy* clone = copyEnemyWithPattern(enemy);
    
    if (clone) {
        std::cout << "Clone appeared: " << clone->name << " with HP: " << clone->hp << "\n";
        clone->attack();
    }

    delete enemy;
    delete clone;
    return 0;
}