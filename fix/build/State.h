#pragma once

#include <string>
#include <vector>

class State {
 public:
  std::vector<int> rooms;
  int room;
  bool inHospital;
  bool inRoom;
  bool valid;
  bool doctor;
  std::string name;
  State() { room = -1; inHospital = false; inRoom = false; 
    valid = false; doctor = false; name = ""; }
};

class Room {
 public:
  int roomNum;
  std::string name;
  bool doctor;
  Room() { roomNum = -1; }
};

struct RoomCompare {
  bool operator()(const Room& lhs, const Room& rhs) {
    return lhs.roomNum < rhs.roomNum;
  }
};
