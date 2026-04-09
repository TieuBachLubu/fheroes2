#pragma once
#include <fstream>
#include <string>
#include <algorithm>
#include "world.h"
#include "settings.h"
#include "game_interface.h"
#include "interface_gamearea.h"
#include "resource.h"
#include "heroes.h"
#include "kingdom.h"
#include "maps.h"

namespace Game {
    inline void DumpTelemetry() {
        std::ofstream f("ai_state.json");
        if (!f.is_open()) return;

        const Settings & conf = Settings::Get();
        const Kingdom & kingdom = world.GetKingdom(conf.CurrentColor());
        
        // Lấy thông tin Hero và Area qua hàm Public
        const Heroes * currentHero = Interface::GetFocusHeroes();
        Interface::AdventureMap & am = Interface::AdventureMap::Get();
        const Interface::GameArea & ga = am.getGameArea(); 
        const fheroes2::Rect roi = ga.GetROI();

        f << "{\n";
        f << "  \"day\": " << (int)world.CountDay() << ",\n";
        f << "  \"gold\": " << (int)kingdom.GetFunds().gold << ",\n";
        
        if (currentHero) {
            f << "  \"hero\": {\n";
            f << "    \"name\": \"" << currentHero->GetName() << "\",\n";
            f << "    \"x\": " << (int)currentHero->GetCenter().x << ",\n";
            f << "    \"y\": " << (int)currentHero->GetCenter().y << ",\n";
            f << "    \"stamina\": " << (int)currentHero->GetMoves() << "\n";
            f << "  },\n";
        }

        f << "  \"objects\": [\n";
        bool first = true;
        
        // Quét các Tile trong tầm nhìn thực tế của màn hình
        for (int32_t i = 0; i < (int32_t)world.getSize(); ++i) {
            const Maps::Tile & tile = world.getTile(i);
            
            // Chỉ lấy vật thể nếu không có sương mù (Chống AI nhìn xuyên thấu gây lỗi)
            if (tile.getMainObjectType() != 0 && !tile.isFog(conf.CurrentColor())) {
                fheroes2::Point p = ga.GetRelativeTilePosition(Maps::GetPoint(i));
                
                // Kiểm tra xem vật thể có đang hiển thị trên màn hình không
                if (p.x >= roi.x && p.y >= roi.y && p.x < roi.x + roi.width && p.y < roi.y + roi.height) {
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
