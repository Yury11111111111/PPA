#include <iostream>
#include <string>
#include <windows.h>

using namespace std;

// 12 конкретных классов. Клиент знает про каждый.
class WindowsButton    { public: void paint() { cout << "Windows button" << endl; } };
class WindowsCheckbox  { public: void paint() { cout << "Windows checkbox" << endl; } };
class WindowsScrollbar { public: void paint() { cout << "Windows scrollbar" << endl; } };
class WindowsTextField { public: void paint() { cout << "Windows text field" << endl; } };

class MacButton    { public: void paint() { cout << "Mac button" << endl; } };
class MacCheckbox  { public: void paint() { cout << "Mac checkbox" << endl; } };
class MacScrollbar { public: void paint() { cout << "Mac scrollbar" << endl; } };
class MacTextField { public: void paint() { cout << "Mac text field" << endl; } };

class LinuxButton    { public: void paint() { cout << "Linux button" << endl; } };
class LinuxCheckbox  { public: void paint() { cout << "Linux checkbox" << endl; } };
class LinuxScrollbar { public: void paint() { cout << "Linux scrollbar" << endl; } };
class LinuxTextField { public: void paint() { cout << "Linux text field" << endl; } };

// Клиент. Четыре метода, в каждом — цепочка if/else по ОС.
class Application {
public:
    void renderButton(const string& os) {
        if (os == "windows")      { WindowsButton b;    b.paint(); }
        else if (os == "mac")     { MacButton b;        b.paint(); }
        else if (os == "linux")   { LinuxButton b;      b.paint(); }
    }

    void renderCheckbox(const string& os) {
        if (os == "windows")      { WindowsCheckbox c;  c.paint(); }
        else if (os == "mac")     { MacCheckbox c;      c.paint(); }
        else if (os == "linux")   { LinuxCheckbox c;    c.paint(); }
    }

    void renderScrollbar(const string& os) {
        if (os == "windows")      { WindowsScrollbar s; s.paint(); }
        else if (os == "mac")     { MacScrollbar s;     s.paint(); }
        else if (os == "linux")   { LinuxScrollbar s;   s.paint(); }
    }

    void renderTextField(const string& os) {
        if (os == "windows")      { WindowsTextField t; t.paint(); }
        else if (os == "mac")     { MacTextField t;     t.paint(); }
        else if (os == "linux")   { LinuxTextField t;   t.paint(); }
    }
};

int main() {
    SetConsoleOutputCP(CP_UTF8);
    system("chcp 65001 > nul");

    Application app;
    app.renderButton("windows");
    app.renderCheckbox("windows");
    app.renderScrollbar("mac");
    app.renderTextField("linux");
}