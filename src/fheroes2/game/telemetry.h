#pragma once
#include <fstream>
#include <string>
#include <vector>
#include "world.h"
#include "settings.h"
#include "game_interface.h"
#include "interface_gamearea.h"
#include "resource.h"
#include "heroes.h"
#include "kingdom.h"
#include "maps.h"
#include "maps_tiles.h"

namespace Game {
    inline void DumpTelemetry() {
        std::ofstream f("ai_state.json");
        if (!f.is_open()) return;

        f << "{\n";
        
        const Settings & conf = Settings::Get();
        const PlayerColor curColor = conf.CurrentColor();
        const Kingdom & kingdom = world.GetKingdom(curColor);
        
        f << "  \"day\": " << (unsigned int)world.CountDay() << ",\n";
        f << "  \"gold\": " << (int)kingdom.GetFunds().gold << ",\n";
        
        Heroes * currentHero = Interface::GetFocusHeroes();
        if (currentHero) {
            f << "  \"hero\": {\n";
            f << "    \"name\": \"hero\",\n";
            f << "    \"x\": " << (int)currentHero->GetCenter().x << ",\n";
            f << "    \"y\": " << (int)currentHero->GetCenter().y << ",\n";
            f << "    \"stamina\": " << (int)currentHero->GetMovePoints() << "\n";
            f << "  },\n";
        }

        f << "  \"objects\": [\n";
        bool first = true;
        
        Interface::AdventureMap & am = Interface::AdventureMap::Get();
        Interface::GameArea & ga = am.getGameArea(); 
        const fheroes2::Rect roi = ga.GetROI();

        const int32_t w = world.w();
        const int32_t totalTiles = (int32_t)world.getSize();
        
        for (int32_t i = 0; i < totalTiles; ++i) {
            const Maps::Tile & tile = world.getTile(i);
            
            if (tile.getMainObjectType() != 0 && !tile.isFog(curColor)) {
                const fheroes2::Point tilePos(i % w, i / w);
                const fheroes2::Point p = ga.GetRelativeTilePosition(tilePos);
                
                if (p.x >= roi.x && p.y >= roi.y && p.x < (roi.x + roi.width) && p.y < (roi.y + roi.height)) {
                    if (!first) f << ",\n";
                    f << "    {\"type\": " << (int)tile.getMainObjectType() 
                      << ", \"x_pixel\": " << (int)p.x << ", \"y_pixel\": " << (int)p.y << "}";
                    first = false;
                }
            }
        }
        f << "\n  ]\n";
        f << "}";
        f.close();
    }
}
