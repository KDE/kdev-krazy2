//krazy:skip
#include <iostream>

#include <QString>

int main (int argc, char *argv[]) {
    //A spelling error: begining

    std::cout << QString("Hello World!") + "\n";

    //Another spelling error: commiting

    std::cout << QString("Goodbye") + " " + QString("World!") + "\n";

    //Yet another spelling error: labelling

    return 0;
}
