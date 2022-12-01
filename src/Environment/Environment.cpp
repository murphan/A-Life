//
// Created by Anna on 11/9/2022.
//

#include "Environment.h"
#include "iostream"

namespace std {

    //Constructs a map with the given number of cells
    Environment::Environment(int startCells) {
        cells = startCells;

        for (int i = 0; i < cells; ++i) {
            //creates a starting vector of vectors
            auto mapRow = std::vector<std::MapCell>();

            for (int j = 0; j < cells; ++j) {
                //creates mapCells and adds them to the row
                auto col = MapCell(10, 10);
                mapRow.emplace_back(col);
            }
            //add the row to the map
            map.emplace_back((mapRow));
        }
    }

    //Preforms actions for each step of the program
    void Environment::EnvironmentStep() {
        //Moves through the map and updates each cell
        for (int i = 0; i < cells; ++i) {
            for (int j = 0; j < cells; ++j) {
                map[i][j].mapCellStep();
            }
        }

    }
} // std