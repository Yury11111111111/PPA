#include <iostream>
#include <string>

using namespace std;

// Абстрактные продукты — 4 интерфейса
class Button {
public:
    virtual ~Button() {}
    virtual void paint() const = 0;
};

class Checkbox {
public:
    virtual ~Checkbox() {}
    virtual void paint() const = 0;
};

class Scrollbar {
public:
    virtual ~Scrollbar() {}
    virtual void paint() const = 0;
};

class TextField {
public:
    virtual ~TextField() {}
    virtual void paint() const = 0;
};

// Семейство Windows — 4 конкретных продукта
class WindowsButton : public Button {
public:
    void paint() const override { cout << "Windows button" << endl; }
};

class WindowsCheckbox : public Checkbox {
public:
    void paint() const override { cout << "Windows checkbox" << endl; }
};

class WindowsScrollbar : public Scrollbar {
public:
    void paint() const override { cout << "Windows scrollbar" << endl; }
};

class WindowsTextField : public TextField {
public:
    void paint() const override { cout << "Windows text field" << endl; }
};

// Семейство Mac — 4 конкретных продукта
class MacButton : public Button {
public:
    void paint() const override { cout << "Mac button" << endl; }
};

class MacCheckbox : public Checkbox {
public:
    void paint() const override { cout << "Mac checkbox" << endl; }
};

class MacScrollbar : public Scrollbar {
public:
    void paint() const override { cout << "Mac scrollbar" << endl; }
};

class MacTextField : public TextField {
public:
    void paint() const override { cout << "Mac text field" << endl; }
};

// Семейство Linux — 4 конкретных продукта
class LinuxButton : public Button {
public:
    void paint() const override { cout << "Linux button" << endl; }
};

class LinuxCheckbox : public Checkbox {
public:
    void paint() const override { cout << "Linux checkbox" << endl; }
};

class LinuxScrollbar : public Scrollbar {
public:
    void paint() const override { cout << "Linux scrollbar" << endl; }
};

class LinuxTextField : public TextField {
public:
    void paint() const override { cout << "Linux text field" << endl; }
};

// Абстрактная фабрика — создаёт все 4 продукта семейства
class GUIFactory {
public:
    virtual ~GUIFactory() {}
    virtual Button*    createButton()    const = 0;
    virtual Checkbox*  createCheckbox()  const = 0;
    virtual Scrollbar* createScrollbar() const = 0;
    virtual TextField* createTextField() const = 0;
};

// Конкретные фабрики
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

// Клиент — знает только про абстракции
class Application {
public:
    Application(const GUIFactory& factory) : factory(factory) {}

    void render() const {
        Button* btn = factory.createButton();
        btn->paint();
        delete btn;

        Checkbox* cb = factory.createCheckbox();
        cb->paint();
        delete cb;

        Scrollbar* sb = factory.createScrollbar();
        sb->paint();
        delete sb;

        TextField* tf = factory.createTextField();
        tf->paint();
        delete tf;
    }

private:
    const GUIFactory& factory;
};

int main() {
    WindowsFactory windowsFactory;
    Application windowsApp(windowsFactory);
    windowsApp.render();

    cout << endl;

    MacFactory macFactory;
    Application macApp(macFactory);
    macApp.render();

    cout << endl;

    LinuxFactory linuxFactory;
    Application linuxApp(linuxFactory);
    linuxApp.render();
}