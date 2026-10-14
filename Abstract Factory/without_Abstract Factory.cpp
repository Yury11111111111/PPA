#include <iostream>
#include <string>

using namespace std;

// 16 конкретных классов. Клиент знает про каждый.
class WindowsButton    { public: void render() { cout << "Windows button" << endl; } };
class WindowsCheckbox  { public: void render() { cout << "Windows checkbox" << endl; } };
class WindowsScrollbar { public: void render() { cout << "Windows scrollbar" << endl; } };
class WindowsTextField { public: void render() { cout << "Windows textfield" << endl; } };

class MacButton    { public: void render() { cout << "Mac button" << endl; } };
class MacCheckbox  { public: void render() { cout << "Mac checkbox" << endl; } };
class MacScrollbar { public: void render() { cout << "Mac scrollbar" << endl; } };
class MacTextField { public: void render() { cout << "Mac textfield" << endl; } };

class LinuxButton    { public: void render() { cout << "Linux button" << endl; } };
class LinuxCheckbox  { public: void render() { cout << "Linux checkbox" << endl; } };
class LinuxScrollbar { public: void render() { cout << "Linux scrollbar" << endl; } };
class LinuxTextField { public: void render() { cout << "Linux textfield" << endl; } };

class AndroidButton    { public: void render() { cout << "Android button" << endl; } };
class AndroidCheckbox  { public: void render() { cout << "Android checkbox" << endl; } };
class AndroidScrollbar { public: void render() { cout << "Android scrollbar" << endl; } };
class AndroidTextField { public: void render() { cout << "Android textfield" << endl; } };

class Application {
public:
    void render(const string& os) {
        cout << "=== " << os << " ===" << endl;

        // 4 ветки, в каждой по 4 создания. Итого 16 строк.
        // Добавить 5-ю ОС: писать ещё 4 строки в этом же методе.
        // Добавить 5-й продукт: добавлять по строке в каждую из 4 ветвей.
        if (os == "windows") {
            WindowsButton b;    b.render();
            WindowsCheckbox c;  c.render();
            WindowsScrollbar s; s.render();
            WindowsTextField t; t.render();
        }
        else if (os == "mac") {
            MacButton b;    b.render();
            MacCheckbox c;  c.render();
            MacScrollbar s; s.render();
            MacTextField t; t.render();
        }
        else if (os == "linux") {
            LinuxButton b;    b.render();
            LinuxCheckbox c;  c.render();
            LinuxScrollbar s; s.render();
            LinuxTextField t; t.render();
        }
        else if (os == "android") {
            AndroidButton b;    b.render();
            AndroidCheckbox c;  c.render();
            AndroidScrollbar s; s.render();
            AndroidTextField t; t.render();
        }
    }
};

int main() {
    Application app;
    app.render("windows");
    app.render("mac");
    app.render("linux");
    app.render("android");
}