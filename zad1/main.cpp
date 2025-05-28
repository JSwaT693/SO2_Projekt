#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <random>
#include "Philosopher.h"

int PHILOSOPHERS_AMOUNT;
std::vector<std::mutex> forks;

void philosophersLife(Philosopher &philosopher) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> thinkingRange(2000, 4000);
    while (philosopher.getState() != States::DEAD) {
        philosopher.think();
        int thinkingTime = thinkingRange(gen);  // Random time of thinking
        //std::cout << "Philosopher " << philosopher.getId() << " is thinking" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(thinkingTime));

        philosopher.increaseHunger();
        auto lastHungerIncrease = std::chrono::steady_clock::now();

        int leftFork = philosopher.getId();
        int rightFork = (philosopher.getId() + 1) % PHILOSOPHERS_AMOUNT;
        int firstFork = (philosopher.getId() % 2 == 0) ? leftFork : rightFork; // Avoiding deadlock through asymetric fork picking
        int secondFork = (philosopher.getId() % 2 == 0) ? rightFork : leftFork;

        while (philosopher.getHunger() < Philosopher::MAX_HUNGER) {
            //std::cout << "Philosopher " << philosopher.getId() << " is waiting for eating" << std::endl;
            if (forks[firstFork].try_lock()) { // Trying to pick up the first fork, if successfull -> trying to pick up 2nd one, else -> trying to pick up the same one again
                if (forks[secondFork].try_lock()) {
                    while (philosopher.getHunger() > 0) { // loop for eating till the philosopher is not hungry
                        philosopher.eat();
                        std::uniform_int_distribution<> eatingRange(500, 2000);
                        int eatingTime = eatingRange(gen);  // Random time of eating
                        std::this_thread::sleep_for(std::chrono::milliseconds(eatingTime));
                        philosopher.decreaseHunger();
                        //std::cout << "Philosopher " << philosopher.getId() << " is eating" << std::endl;
                    }
                    forks[firstFork].unlock();
                    forks[secondFork].unlock();
                    break;
                } else {
                    forks[firstFork].unlock(); // unlocking the first fork in case he couldnt pick up the second one
                }
            }

            if (std::chrono::steady_clock::now() - lastHungerIncrease >= std::chrono::milliseconds(2000)) { // getting more hungry every second of not being able to eat
                philosopher.increaseHunger();
                lastHungerIncrease = std::chrono::steady_clock::now();
            }

            std::uniform_int_distribution<> waitingRange(50, 250);
            int waitTime = waitingRange(gen);  // Random time of waiting - the purpose of this is to lesser the chance of starvation of one of the philosophers
            std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
        }
    }
}

void display(const std::vector<Philosopher>& philosophers) { // Function displaying states of the philosophers
    while (true) {
        //system("cls");
        for (const auto& philosopher : philosophers) {
            std::cout << philosopher.getId() + 1 << " ";
            switch (philosopher.getState()) {
                case States::THINKING:
                    std::cout << "THINKING ";
                    break;
                case States::EATING:
                    std::cout << "EATING ";
                    break;
                case States::HUNGRY:
                    std::cout << "HUNGRY ";
                    break;
                case States::DEAD:
                    std::cout << "DEAD ";
                    break;
            }
            std::cout << philosopher.getHunger() << std::endl;
        }
        std::cout << "-------------------" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Time between each display
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) { // Entry data handling
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

    forks = std::vector<std::mutex>(PHILOSOPHERS_AMOUNT);

    std::vector<Philosopher> philosophers;
    std::vector<std::thread> threads;

    philosophers.reserve(PHILOSOPHERS_AMOUNT);
    for (int i = 0; i < PHILOSOPHERS_AMOUNT; i++) {
            philosophers.emplace_back(i);
    }

    threads.reserve(PHILOSOPHERS_AMOUNT);
    std::thread displayThread(display, std::ref(philosophers)); // The display thread
    for (int i = 0; i < PHILOSOPHERS_AMOUNT; i++) {
            threads.emplace_back(philosophersLife, std::ref(philosophers[i])); // Creating the threads of philosophers
    }

    for (auto &thread : threads) {
        thread.join();
    }

    displayThread.detach();
    return 0;
}

