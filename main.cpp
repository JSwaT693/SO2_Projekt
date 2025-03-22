#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include "Philosopher.h"

int PHILOSOPHERS_AMOUNT = 0;
std::vector<std::mutex> forks(PHILOSOPHERS_AMOUNT);

void philosophersLife(Philosopher &philosopher) {

}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <number_of_philosophers>" << std::endl;
        return 1;
    }

    PHILOSOPHERS_AMOUNT = std::stoi(argv[1]);
    if (PHILOSOPHERS_AMOUNT <= 0) {
        std::cout << "The number of philosophers must be greater than 0." << std::endl;
        return 1;
    }
//    std::cout << "Input the amount of philosophers: ";
//    std::cin >> PHILOSOPHERS_AMOUNT;
//    if (PHILOSOPHERS_AMOUNT < 1) {
//        std::cout << "Amount of the philosophers must be greater than 0";
//        return 1;
//    }

    std::vector<Philosopher> philosophers;
    std::vector<std::thread> threads;

    philosophers.reserve(PHILOSOPHERS_AMOUNT);
    for (int i = 0; i < PHILOSOPHERS_AMOUNT; i++) {
            philosophers.emplace_back(i);
    }

    threads.reserve(PHILOSOPHERS_AMOUNT);
    for (int i = 0; i < PHILOSOPHERS_AMOUNT; i++) {
            threads.emplace_back(philosophersLife, std::ref(philosophers[i]));
    }

    for (auto &thread : threads) {
        thread.join();
    }

    return 0;
}

