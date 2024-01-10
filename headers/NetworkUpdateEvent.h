#ifndef NUN_H
#define NUN_H
#include <functional>
#include <list>
#include <mutex>
#include "NeuralNetwork.h"

class NetworkUpdateEvent
{
public:
    using NetworkUpdateHandler = std::function<void(NeuralNetwork*)>;

    NetworkUpdateEvent();
    ~NetworkUpdateEvent();

    void Subscribe(const NetworkUpdateHandler& handler);
    void Unsubscribe(const NetworkUpdateHandler& handler);
    void UpdateBestNetwork(NeuralNetwork* newBestNetwork);

private:
    void NotifyListeners();
    NeuralNetwork* bestNetwork;
    std::list<NetworkUpdateHandler> handlers;
};
#endif