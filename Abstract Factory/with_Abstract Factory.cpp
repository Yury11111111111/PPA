#include <iostream>
#include <string>

using namespace std;

// 4 абстрактных продукта
class Button {
public:
    virtual ~Button() {}
    virtual void render() const = 0;
};

class Checkbox {
public:
    virtual ~Checkbox() {}
    virtual void render() const = 0;
};

class Scrollbar {
public:
    virtual ~Scrollbar() {}
    virtual void render() const = 0;
};

class TextField {
public:
    virtual ~TextField() {}
    virtual void render() const = 0;
};

// Windows — 4 продукта
class WindowsButton    : public Button    { public: void render() const override { cout << "Windows button" << endl; } };
class WindowsCheckbox  : public Checkbox  { public: void render() const override { cout << "Windows checkbox" << endl; } };
class WindowsScrollbar : public Scrollbar { public: void render() const override { cout << "Windows scrollbar" << endl; } };
class WindowsTextField : public TextField { public: void render() const override { cout << "Windows textfield" << endl; } };

// Mac — 4 продукта
class MacButton    : public Button    { public: void render() const override { cout << "Mac button" << endl; } };
class MacCheckbox  : public Checkbox  { public: void render() const override { cout << "Mac checkbox" << endl; } };
class MacScrollbar : public Scrollbar { public: void render() const override { cout << "Mac scrollbar" << endl; } };
class MacTextField : public TextField { public: void render() const override { cout << "Mac textfield" << endl; } };

// Linux — 4 продукта
class LinuxButton    : public Button    { public: void render() const override { cout << "Linux button" << endl; } };
class LinuxCheckbox  : public Checkbox  { public: void render() const override { cout << "Linux checkbox" << endl; } };
class LinuxScrollbar : public Scrollbar { public: void render() const override { cout << "Linux scrollbar" << endl; } };
class LinuxTextField : public TextField { public: void render() const override { cout << "Linux textfield" << endl; } };

// Android — 4 продукта
class AndroidButton    : public Button    { public: void render() const override { cout << "Android button" << endl; } };
class AndroidCheckbox  : public Checkbox  { public: void render() const override { cout << "Android checkbox" << endl; } };
class AndroidScrollbar : public Scrollbar { public: void render() const override { cout << "Android scrollbar" << endl; } };
class AndroidTextField : public TextField { public: void render() const override { cout << "Android textfield" << endl; } };

// Абстрактная фабрика.
// Слабость паттерна: чтобы добавить новый продукт (например Menu),
// придётся править сам этот интерфейс и все конкретные фабрики ниже.
// Зато добавить новую ОС дёшево: интерфейс не трогается.
class GUIFactory {
public:
    virtual ~GUIFactory() {}
    virtual Button*    createButton()    const = 0;
    virtual Checkbox*  createCheckbox()  const = 0;
    virtual Scrollbar* createScrollbar() const = 0;
    virtual TextField* createTextField() const = 0;
};

// Конкретные фабрики.
// Добавить новую ОС — создать ещё одну фабрику, старые не трогаются.
// Добавить новый продукт — придётся дописать метод в каждой из этих фабрик.
class WindowsFactory : public GUIFactory {
public:
    Button*    createButton()    const override { return new WindowsButton(); }
    Checkbox*  createCheckbox()  const override { return new WindowsCheckbox(); }
    Scrollbar* createScrollbar() const override { return new WindowsScrollbar(); }
    TextField* createTextField() const override { return new WindowsTextField(); }
};

class MacFactory : public GUIFactory {
public:
    Button*    createButton()    const override { return new MacButton(); }
    Checkbox*  createCheckbox()  const override { return new MacCheckbox(); }
    Scrollbar* createScrollbar() const override { return new MacScrollbar(); }
    TextField* createTextField() const override { return new MacTextField(); }
};

class LinuxFactory : public GUIFactory {
public:
    Button*    createButton()    const override { return new LinuxButton(); }
    Checkbox*  createCheckbox()  const override { return new LinuxCheckbox(); }
    Scrollbar* createScrollbar() const override { return new LinuxScrollbar(); }
    TextField* createTextField() const override { return new LinuxTextField(); }
};

class AndroidFactory : public GUIFactory {
public:
    Button*    createButton()    const override { return new AndroidButton(); }
    Checkbox*  createCheckbox()  const override { return new AndroidCheckbox(); }
    Scrollbar* createScrollbar() const override { return new AndroidScrollbar(); }
    TextField* createTextField() const override { return new AndroidTextField(); }
};

// Клиент не знает ни про одну ОС
// Здесь нет ни if/else, ни упоминаний Windows, Mac, Linux, Android
// Новая ОС: этот класс не меняется вообще, только создаётся новая фабрика
// Новый продукт: сюда добавляется одна строка, но придётся править интерфейс GUIFactory и все конкретные фабрики
class Application {
public:
    Application(const GUIFactory& factory) : factory(factory) {}

    void render() const {
        Button*    b = factory.createButton();
        Checkbox*  c = factory.createCheckbox();
        Scrollbar* s = factory.createScrollbar();
        TextField* t = factory.createTextField();

        b->render();
        c->render();
        s->render();
        t->render();

        delete b; delete c; delete s; delete t;
    }

private:
    const GUIFactory& factory;
};

int main() {
    WindowsFactory wf; Application app1(wf); app1.render();
    MacFactory     mf; Application app2(mf); app2.render();
    LinuxFactory   lf; Application app3(lf); app3.render();
    AndroidFactory af; Application app4(af); app4.render();
}