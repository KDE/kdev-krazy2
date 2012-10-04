//krazy:skip
//krazy:excludeall=license

#include <iostream>

#include <QString>

int main (int argc, char *argv[]) {
    std::cout << QString("Hello World!") + "\n";

    std::cout << QString("Goodbye") + " " + QString("World!") + "\n";

    return 0;
}
