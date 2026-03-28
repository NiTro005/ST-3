// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "TimedDoor.h"

using ::testing::Return;


class FakeTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class FakeDoor : public Door {
 public:
  MOCK_METHOD(void, lock, (), (override));
  MOCK_METHOD(void, unlock, (), (override));
  MOCK_METHOD(bool, isDoorOpened, (), (override));
};


class DoorFixture : public ::testing::Test {
 protected:
  TimedDoor* door;

  void SetUp() override {
    door = new TimedDoor(2);
  }

  void TearDown() override {
    delete door;
  }
};


TEST_F(DoorFixture, DoorClosedAtStart) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(DoorFixture, OpenChangesState) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(DoorFixture, CloseChangesState) {
  door->unlock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(DoorFixture, TimeoutValueStored) {
  EXPECT_EQ(door->getTimeOut(), 2);
}

TEST_F(DoorFixture, ExceptionIfStillOpen) {
  door->unlock();
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(DoorFixture, NoExceptionIfClosed) {
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}


TEST(AdapterCheck, CallsThrowStateWhenOpen) {
  TimedDoor d(1);
  d.unlock();

  DoorTimerAdapter adapter(d);
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(AdapterCheck, NoThrowWhenClosed) {
  TimedDoor d(1);
  d.lock();

  DoorTimerAdapter adapter(d);
  EXPECT_NO_THROW(adapter.Timeout());
}


TEST(TimerCheck, CallsClientMethod) {
  Timer timer;
  FakeTimerClient mock;

  EXPECT_CALL(mock, Timeout()).Times(1);
  timer.tregister(0, &mock);
}


TEST(MockUsage, UnlockCalledOnce) {
  FakeDoor d;

  EXPECT_CALL(d, unlock()).Times(1);
  d.unlock();
}

TEST(MockUsage, LockCalledOnce) {
  FakeDoor d;

  EXPECT_CALL(d, lock()).Times(1);
  d.lock();
}

TEST(MockUsage, ReturnValueCheck) {
  FakeDoor d;

  EXPECT_CALL(d, isDoorOpened()).WillOnce(Return(true));
  EXPECT_TRUE(d.isDoorOpened());
}


TEST(Integration, FullFlowThrows) {
  TimedDoor d(0);
  EXPECT_THROW(d.unlock(), std::runtime_error);
}

TEST(Integration, ClosedDoorSafe) {
  TimedDoor d(0);
  d.lock();

  DoorTimerAdapter adapter(d);
  EXPECT_NO_THROW(adapter.Timeout());
}
