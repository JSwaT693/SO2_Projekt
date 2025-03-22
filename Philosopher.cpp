//
// Created by jakub on 17.03.2025.
//

#include "Philosopher.h"

Philosopher::Philosopher(int id) {
    this->id = id;
    state = States::THINKING;
    hunger = 0;
}

void Philosopher::increaseHunger() {
    if (hunger < MAX_HUNGER - 1) {
        hunger++;
    } else {
        state = States::DEAD;
    }
}

void Philosopher::decreaseHunger() {
    hunger--;
}

void Philosopher::eat() {
    state = States::EATING;
    decreaseHunger();
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


