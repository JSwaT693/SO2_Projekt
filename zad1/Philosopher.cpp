//
// Created by jakub on 17.03.2025.
//

#include "Philosopher.h"

Philosopher::Philosopher(int id) { // Parameters at the start
    this->id = id;
    state = States::THINKING;
    hunger = 0;
}

void Philosopher::increaseHunger() {
    hunger++;
    if (hunger < MAX_HUNGER) {
        if (hunger == 1) {
            state = States::HUNGRY;
        }
    } else {
        state = States::DEAD;
    }
}

void Philosopher::decreaseHunger() {
    hunger--;
}

void Philosopher::eat() {
    state = States::EATING;
}

void Philosopher::think() {
    state = States::THINKING;
}


const int Philosopher::getId() const {
    return id;
}

States Philosopher::getState() const {
    return state;
}

int Philosopher::getHunger() const {
    return hunger;
}


