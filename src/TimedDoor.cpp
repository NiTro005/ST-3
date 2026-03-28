// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include "TimedDoor.h"
#include <stdexcept>
#include <thread>
#include <chrono>


DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() {
  if (door.isDoorOpened()) {
    door.throwState();
  }
}


void Timer::sleep(int t) {
  std::this_thread::sleep_for(std::chrono::seconds(t));
}

void Timer::tregister(int timeout, TimerClient* cl) {
  client = cl;
}


TimedDoor::TimedDoor(int timeout)
    : iTimeout(timeout), isOpened(false) {
  adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened() {
  return isOpened;
}

void TimedDoor::unlock() {
  isOpened = true;
}

void TimedDoor::lock() {
  isOpened = false;
}

int TimedDoor::getTimeOut() {
  return iTimeout;
}

void TimedDoor::throwState() {
  if (isOpened) {
    throw std::runtime_error("Door is still open!");
  }
}
