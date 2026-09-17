#include <iostream>
#include <fstream>
using namespace std;

bool read(const char* filename, int*& mass, int& size) {
    size = 0;
    int capacity = 5;
    mass = new int[capacity];
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Can't find \"" << filename <<'\"' << endl;
        return false;
    }
    while (file.peek() != EOF) {
        if (size == capacity) {
            int* t = new int[capacity * 2];
            for (int i = 0; i < capacity; i++) {
                t[i] = mass[i];
            }
            delete[]mass;
            mass = t;
            capacity *= 2;
        }
        file >> mass[size++]; 
    }
    file.close();
    return true;
}

int partition(int* arr, int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);  
    return i + 1;
}

void quickSort(int* arr, int low, int high) {
  
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}


int main() {
    int* mass = nullptr;
    int size;
    if (!read("file.txt", mass, size)) {
        return ENOENT;
    }
    quickSort(mass, 0, size - 1);
  
    for (int i = 0; i < size; i++) {
        cout << mass[i] << " ";
    }
    return 0;
    
}
