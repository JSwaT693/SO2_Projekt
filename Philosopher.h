//
// Created by jakub on 17.03.2025.
//

#ifndef PROJEKT_PHILOSOPHER_H
#define PROJEKT_PHILOSOPHER_H


#include "States.h"

class Philosopher {
public:
    explicit Philosopher(int id);

    void increaseHunger();
    void decreaseHunger();
    void eat();
    void think();

    [[nodiscard]] const int getId() const;
    [[nodiscard]] States getState() const;
    [[nodiscard]] int getHunger() const;


private:
    int id;
    States state;
    int hunger = 0;
    const int MAX_HUNGER = 10;

};


#endif //PROJEKT_PHILOSOPHER_H
