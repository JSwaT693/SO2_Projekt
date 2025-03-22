#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <random>
#include "Philosopher.h"

int PHILOSOPHERS_AMOUNT = 0;
std::vector<std::mutex> forks(PHILOSOPHERS_AMOUNT);

void philosophersLife(Philosopher &philosopher) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 5000);
    while (philosopher.getState() != States::DEAD) {
        philosopher.think();
        int thinkingTime = dis(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(thinkingTime));

        philosopher.increaseHunger();
        auto lastHungerIncrease = std::chrono::steady_clock::now();

        int leftFork = philosopher.getId();
        int rightFork = (philosopher.getId() + 1) % PHILOSOPHERS_AMOUNT;
        int firstFork = (philosopher.getId() % 2 == 0) ? leftFork : rightFork;
        int secondFork = (philosopher.getId() % 2 == 0) ? rightFork : leftFork;

        while (true) {
            if (forks[firstFork].try_lock()) {
                if (forks[secondFork].try_lock()) {
                    while (philosopher.getHunger() > 0) {
                        philosopher.eat();
                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                        philosopher.decreaseHunger();
                    }
                    forks[firstFork].unlock();
                    forks[secondFork].unlock();
                    break;
                } else {
                    forks[firstFork].unlock();
                }
            }

            if (std::chrono::steady_clock::now() - lastHungerIncrease >= std::chrono::milliseconds(3000)) {
                philosopher.increaseHunger();
                lastHungerIncrease = std::chrono::steady_clock::now();
            }
        }
    }
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

