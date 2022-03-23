#pragma once

#include "framework.h"

// Class, computing lifecycle
class Life
{
private:
	size_t m_width, m_height;
	unsigned int m_gener{ 0 };
	std::vector<int> m_vector, m_matrix, m_rule;

	// Create transition matrix
	void create_matrix_();

public:
	// Constructor from given size
	Life(size_t, size_t);
	// Constructor from file
	Life(const char[]);
	
	void Update();
	void PrintMatrix();
	void PrintVector();
};
