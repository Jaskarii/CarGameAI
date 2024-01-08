#include "CarHandler.h"
void CheckKeyPress(GLFWwindow *window);

float scale = 1;
CarHandler *carHandler;
Road *road;
void UpdateCars(std::vector<Car> *cars, Road *road);
