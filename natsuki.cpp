#include "natsuki.h"

#include <iostream>
#include <stdexcept>

int main(int argc, char **argv) try {
    natsuki::Nats nats{argc >= 2 ? argv[1] : "localhost"};
} catch (std::runtime_error const &e) {
    std::cerr << e.what();
    throw;
}
