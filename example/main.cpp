#include <injector/injector.hpp>
#include <iostream>

int main() {
    auto stream = injector::get_resource_stream<injector::injected_resources::cmake>();

    char line[255] = {};
    while (stream.getline(line, sizeof(line))) {
        std::cout << line << std::endl;
    }

    return 0;
}