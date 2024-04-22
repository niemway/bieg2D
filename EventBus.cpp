//
//  EventBus.cpp
//  game_engine
//
//  Created by Nicholas Way on 4/1/24.
//

#include "EventBus.hpp"

void EventBus::Publish(std::string event_name, luabridge::LuaRef event_object){
	if(subscriptions.find(event_name) != subscriptions.end()){
		for(auto& subscription : subscriptions[event_name]){
			try{
				subscription.second(subscription.first, event_object);
			}
			catch (luabridge::LuaException e){
				// Handle exception
				EngineUtils::ReportError("EventBus", e);
			}
		}
	}
}

void EventBus::Publish_NoRef(std::string event_name){
    if(subscriptions.find(event_name) != subscriptions.end()){
        for(auto& subscription : subscriptions[event_name]){
            try{
                subscription.second(subscription.first);
            }
            catch (luabridge::LuaException e){
                // Handle exception
                EngineUtils::ReportError("EventBus", e);
            }
        }
    }
}

void EventBus::Subscribe(std::string event_name, luabridge::LuaRef component, luabridge::LuaRef function){
	pending_subscriptions.push_back(std::make_pair(event_name, std::make_pair(component, function)));
}

void EventBus::Unsubscribe(std::string event_name, luabridge::LuaRef component, luabridge::LuaRef function){
	pending_unsubscriptions.push_back(std::make_pair(event_name, std::make_pair(component, function)));
}

void EventBus::ProcessPending(){
	ProcessPendingUnsubscriptions();
	ProcessPendingSubscriptions();
}

void EventBus::ProcessPendingSubscriptions(){
	for(auto& subscription : pending_subscriptions){
		if(subscriptions.find(subscription.first) == subscriptions.end()){
			subscriptions[subscription.first] = std::vector<std::pair<luabridge::LuaRef, luabridge::LuaRef>>();
		}
		subscriptions[subscription.first].push_back(subscription.second);
	}
	pending_subscriptions.clear();
}

void EventBus::ProcessPendingUnsubscriptions(){
	for(auto& unsubscription : pending_unsubscriptions){
		if(subscriptions.find(unsubscription.first) != subscriptions.end()){
			auto& subscription_list = subscriptions[unsubscription.first];
			for(auto iter = subscription_list.begin(); iter != subscription_list.end(); iter++){
				if(iter->first == unsubscription.second.first && iter->second == unsubscription.second.second){
					subscription_list.erase(iter);
					break;
				}
			}
		}
	}
	pending_unsubscriptions.clear();
}
