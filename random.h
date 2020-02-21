#ifndef RANDOM_H
#define RANDOM_H
#include <random>

class RandomInt
{
public:
	RandomInt(int min =0, int max =1);
	void changeMinMax(int min, int max);
	int draw();
private:
	std::mt19937 m_rng;
	std::uniform_int_distribution<int> m_dist;
};

class RandomDouble
{
public:
	RandomDouble(double min =0.0, double max =1.0);
	void changeMinMax(double min, double max);
	double draw();
private:
	std::mt19937 m_rng;
	std::uniform_real_distribution<double> m_dist;
};

#endif // RANDOM_H
