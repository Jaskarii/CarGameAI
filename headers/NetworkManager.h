#include "NeuralNetwork.h"

class NetworkManager
{
    public:
        NetworkManager();
        void InitNetworks(std::vector<int> layout, int amount);
        void NextGeneration();
        NeuralNetwork* GetNetWork(int index);

    private:
        std::vector<NeuralNetwork>* networks;
};
