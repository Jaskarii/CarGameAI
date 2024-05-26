This is a demo made to demonstrate reinforcement learning neural networks.

Application should open randomly generated track that arrows learn no navigate.
One Game contains hundreds of cars which are controlled by randomly generated neural network.

Application opens multiple games, one that is rendered to the screen while others run virtually in the background.
After game finishes its generation, its best performing NN is passed to global list of best networks.
Neural network engine is selfmade and also has Cuda implementation to run Feed forward more efficiently.

One game loop lasts for configurable amount of frames after which generation is reset.
For next generation, top 20 networks are retained and passed for all the rest with incremental random mutations.

NOTE!
This approach for training is the most unefficient imaginable. Assessing and updating weights should be done frame by frame basis.
