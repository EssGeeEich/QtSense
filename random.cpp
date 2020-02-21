#include "random.h"

RandomInt::RandomInt(int min, int max)
	: m_rng(std::random_device{}()),
	  m_dist(min, max)
{}

int RandomInt::draw()
{
	return m_dist(m_rng);
}

void RandomInt::changeMinMax(int min, int max)
{
	m_dist = std::uniform_int_distribution<int>(min, max);
}

RandomDouble::RandomDouble(double min, double max)
	: m_rng(std::random_device{}()),
	  m_dist(min, max)
{}

double RandomDouble::draw()
{
	return m_dist(m_rng);
}

void RandomDouble::changeMinMax(double min, double max)
{
	m_dist = std::uniform_real_distribution<double>(min, max);
}
