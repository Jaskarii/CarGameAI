#include "NetworkUpdateEvent.h"


NetworkUpdateEvent::NetworkUpdateEvent()
{}

NetworkUpdateEvent::~NetworkUpdateEvent() 
{}

void NetworkUpdateEvent::Subscribe(const NetworkUpdateHandler& handler) 
{
    handlers.push_back(handler);
}

void NetworkUpdateEvent::Unsubscribe(const NetworkUpdateHandler& handler) 
{
    handlers.remove_if([&handler](const NetworkUpdateHandler& h) {
        return h.target<void(NeuralNetwork*)>() == handler.target<void(NeuralNetwork*)>();
    });
}


void NetworkUpdateEvent::UpdateBestNetwork(NeuralNetwork* newBestNetwork) 
{
    bestNetwork = newBestNetwork;
    NotifyListeners();
}

void NetworkUpdateEvent::NotifyListeners() 
{
    for (const auto& handler : handlers) {
        handler(bestNetwork);
    }
}
