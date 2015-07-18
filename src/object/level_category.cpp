/*
 * This file is part of the Colobot: Gold Edition source code
 * Copyright (C) 2001-2015, Daniel Roux, EPSITEC SA & TerranovaTeam
 * http://epsiteс.ch; http://colobot.info; http://github.com/colobot
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://gnu.org/licenses
 */
#include "object/level_category.h"

#include <map>

const std::map<LevelCategory, std::string> category_dir_map = {
    { LevelCategory::Missions,     "missions"     },
    { LevelCategory::FreeGame,     "freemissions" },
    { LevelCategory::Exercises,    "exercises"    },
    { LevelCategory::Challenges,   "challenges"   },
    { LevelCategory::CustomLevels, "custom"       },

    { LevelCategory::Win,          "win"          },
    { LevelCategory::Lost,         "lost"         },
    { LevelCategory::Perso,        "perso"        },
};

std::string GetLevelCategoryDir(LevelCategory category)
{
    return category_dir_map.at(category);
}

LevelCategory GetLevelCategoryFromDir(std::string dir)
{
    for(auto it = category_dir_map.begin(); it != category_dir_map.end(); ++it)
    {
        if(it->second == dir)
        {
            return it->first;
        }
    }
    return LevelCategory::Max;
}
