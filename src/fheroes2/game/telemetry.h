#pragma once
#include <fstream>
#include <string>
#include "world.h"
#include "settings.h"
#include "game_interface.h"
#include "interface_gamearea.h"

namespace Game {
    inline void DumpTelemetry() {
        std::ofstream f("ai_state.json");
        if (!f.is_open()) return;

        const Settings & conf = Settings::Get();
        const Kingdom & kingdom = world.GetKingdom(conf.CurrentColor());
        const Interface::AdventureMap & am = Interface::AdventureMap::Get();
        const Interface::GameArea & ga = am.GetGameArea();

        f << "{\n";
        f << "  \"day\": " << world.CountDay() << ",\n";
        f << "  \"gold\": " << kingdom.GetFunds().Get(Resource::GOLD) << ",\n";
        
        // Lấy thông tin Hero đang chọn
        const Heroes * hero = am.GetFocusHeroes();
        if (hero) {
            f << "  \"hero\": {\n";
            f << "    \"name\": \"" << hero->GetName() << "\",\n";
            f << "    \"x\": " << hero->GetCenter().x << ",\n";
            f << "    \"y\": " << hero->GetCenter().y << ",\n";
            f << "    \"stamina\": " << hero->GetMoves() << "\n";
            f << "  },\n";
        }

        f << "  \"objects\": [\n";
        bool first = true;
        // Quét các vật thể trong tầm nhìn GameArea (thay cho OpenCV)
        for (int32_t i = 0; i < world.getSize(); ++i) {
            const Maps::Tile & tile = world.getTile(i);
            if (tile.getMainObjectType() != 0 && !tile.isFog(conf.CurrentColor())) {
                fheroes2::Point p = ga.GetPointFromTileId(i);
                if (p.x >= 0 && p.y >= 0 && p.x < ga.width() && p.y < ga.height()) {
                    if (!first) f << ",\n";
                    f << "    {\"type\": " << (int)tile.getMainObjectType() 
                      << ", \"x_pixel\": " << p.x << ", \"y_pixel\": " << p.y << "}";
                    first = false;
                }
            }
        }
        f << "\n  ]\n";
        f << "}";
        f.close();
    }
}
