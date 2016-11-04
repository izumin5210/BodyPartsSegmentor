//
// Created by Masayuki IZUMI on 11/4/16.
//

#ifndef BODYPARTSSEGMENTOR_SKELETON_H
#define BODYPARTSSEGMENTOR_SKELETON_H

#include "cereal/cereal.hpp"
#include "cereal/types/vector.hpp"

struct Joint {
  float x;
  float y;
  int type;

  template<class Archive>
  void serialize(Archive &ar) {
    ar(CEREAL_NVP(x), CEREAL_NVP(y), CEREAL_NVP(type));
  }
};

struct Skeleton {
  int user_id;
  std::vector<Joint> joints;

  template<class Archive>
  void serialize(Archive &ar) {
    ar(CEREAL_NVP(user_id), CEREAL_NVP(joints));
  }
};

#endif //BODYPARTSSEGMENTOR_SKELETON_H
