//
// Created by Masayuki IZUMI on 11/4/16.
//

#ifndef BODYPARTSSEGMENTOR_SKELETON_H
#define BODYPARTSSEGMENTOR_SKELETON_H

#include "cereal/cereal.hpp"
#include "cereal/types/map.hpp"
#include "cereal/types/vector.hpp"

struct Skeleton {
  int user_id;
  std::map<int, std::vector<float>> joints;
  std::vector<std::vector<int>> pixels;

  template<class Archive>
  void serialize(Archive &ar) {
    ar(CEREAL_NVP(user_id), CEREAL_NVP(joints), CEREAL_NVP(pixels));
  }
};

#endif //BODYPARTSSEGMENTOR_SKELETON_H
