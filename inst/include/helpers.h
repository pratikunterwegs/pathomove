
#pragma once

// TODO: move to different files or include headers as needed

template <typename T> const inline T square(T value) { return value * value; }

/// @brief Is agent i handling food?
/// @param i Agent i.
/// @return A boolean.
inline const bool Population::is_handling(const size_t i) {
  return counter[i] > 0;
}

/// @brief Count agents in a Population near the location (x,y)
/// @param x X coordinate.
/// @param y Y coordinate.
/// @param range Distance to compare against.
/// @return Count of agents within range.
inline const std::pair<int, int> Population::count_agents(const double &x,
                                                          const double &y) {
  int handlers = 0;
  int nonhandlers = 0;
  const double range_square = square(range_agents);
  const auto distsquare = square(coordX - x) + square(coordY - y);

  for (size_t i = 0; i < nAgents; i++) {
    if (distsquare[i] < range_square) {
      if (is_handling(i)) {
        handlers++;
      } else {
        nonhandlers++;
      }
    }
  }

  return std::pair<int, int>{handlers, nonhandlers};
}

/// @brief
/// @param x
/// @param y
/// @return
inline const std::vector<size_t> Population::get_neighbour_id(const double &x,
                                                              const double &y) {
  const double range_square = square(range_agents);
  const auto distsquare = square(coordX - x) + square(coordY - y);

  std::vector<size_t> agent_id;

  for (size_t i = 0; i < nAgents; i++) {
    if (distsquare[i] < range_square) {
      agent_id.push_back(i);
    }
  }

  return agent_id;
}

/// @brief
/// @param food
/// @param x
/// @param y
/// @return
inline const int Population::count_food(const Resources &food, const double &x,
                                        const double &y) {

  int nfood = 0;
  if (food.nAvailable > 0) {
    const double range_square = square(range_food);
    const auto distsquare = square(food.coordX - x) + square(food.coordY - y);

    int nfood = 0;

    for (size_t i = 0; i < food.nFood; i++) {
      if (distsquare[i] < range_square && food.available[i])
        nfood++;
    }
  }

  return nfood;
}

/// @brief 
/// @param food 
/// @param x 
/// @param y 
/// @return 
inline const std::vector<size_t> Population::get_food_id(const Resources &food,
                                                         const double &x,
                                                         const double &y) {
  std::vector<size_t> food_id;
  const double range_square = square(range_food);
  const auto distsquare = square(food.coordX - x) + square(food.coordY - y);

  for (size_t i = 0; i < food.nFood; i++) {
    if (distsquare[i] < range_square && food.available[i]) {
      food_id.push_back(i);
    }
  }

  return food_id;
}
