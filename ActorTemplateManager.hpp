//
//  ActorTemplateManager.hpp
//  game_engine
//
//  Created by Nicholas Way on 1/31/24.
//

#ifndef ActorTemplateManager_hpp
#define ActorTemplateManager_hpp

#include <stdio.h>
#include "Actor.hpp"
#include "EngineUtils.h"
#include "ComponentManager.hpp"

class ActorTemplateManager{
private:
    // std::vector<Actor> actor_templates;
public:
    // void LoadActorTemplates(const std::string& path);
    // void LoadActorTemplates(const std::string& name);
    static Actor GetActorTemplate(const std::string& name);
    static Actor _GetActorTemplateFromPath(const std::string& path);
};

#endif /* ActorTemplateManager_hpp */
