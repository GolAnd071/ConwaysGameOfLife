#include "Life.h"

// Constructor from given size
Life::Life(size_t width, size_t height) : m_width(width), m_height(height)
{
	// Create field vector randomly
	m_vector.resize(m_width * m_height);
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, 1);
	std::generate(m_vector.begin(), m_vector.end(), [&gen, &dist]() { return dist(gen); });

	// Default rule is B3/S23
	m_rule.push_back(5);
	m_rule.push_back(6);
	m_rule.push_back(7);

	create_matrix_();
}

// Constructor from file
Life::Life(const char filename[])
{
	// Open file
	FILE* file;
	fopen_s(&file, filename, "r");

	// Read size and rule
	unsigned int x{}, y{}, b{}, s{};
	if (file) fscanf_s(file, "x = %d, y = %d, rule = B%d/S%d\n", &x, &y, &b, &s);
	m_width = 64, m_height = 64;
	int offset_x = (m_width - x) / 2,
		offset_y = (m_height - y) / 2;
	std::string bs = std::to_string(b), ss = std::to_string(s);
	for (auto i : bs) m_rule.push_back(2 * (i - '0'));
	for (auto i : ss) m_rule.push_back(2 * (i - '0') + 1);

	// Read field vector
	m_vector.resize(m_width * m_height);
	std::fill(m_vector.begin(), m_vector.end(), 0);
	char c = ' ';
	size_t i = 0, j = 0, n = 0;
	do {
		if (file) c = fgetc(file);
		if (c == 'o') {
			if (!n) n = 1;
			while (n--) { m_vector.at((offset_y + i) * m_width + offset_x + j) = 1; ++j; }
			n = 0;
		}
		else if (c == 'b') {
			if (!n) n = 1;
			while (n--) { m_vector.at((offset_y + i) * m_width + offset_x + j) = 0; ++j; }
			n = 0;
		}
		else if (c == '$') {
			j = 0;
			i += (n ? n : 1);
			n = 0;
		}
		else if (c >= '0' && c <= '9') {
			n *= 10;
			n += c - '0';
		}
	} while (c != '!');

	if (file) fclose(file);

	create_matrix_();
}

void Life::Update()
{
	// New generation
	++m_gener;

	// Multiply matrices
	std::vector<int> new_vector(m_width * m_height);
	for (size_t i = 0; i < m_width * m_height; ++i) {
		int sum = m_matrix.at(i * (m_width * m_height + 1)) * m_vector.at(i);
		if (i > 0) sum += m_matrix.at(i * (m_width * m_height + 1) - 1) * m_vector.at(i - 1);
		if (i < m_width * m_height - 1) sum += m_matrix.at(i * (m_width * m_height + 1) + 1) * m_vector.at(i + 1);
		if (i > m_width) sum += m_matrix.at(i * (m_width * m_height + 1) - m_width - 1) * m_vector.at(i - m_width - 1);
		if (i > m_width - 1) sum += m_matrix.at(i * (m_width * m_height + 1) - m_width) * m_vector.at(i - m_width);
		if (i > m_width - 2) sum += m_matrix.at(i * (m_width * m_height + 1) - m_width + 1) * m_vector.at(i - m_width + 1);
		if (i < m_width * (m_height - 1) + 1) sum += m_matrix.at(i * (m_width * m_height + 1) + m_width - 1) * m_vector.at(i + m_width - 1);
		if (i < m_width * (m_height - 1)) sum += m_matrix.at(i * (m_width * m_height + 1) + m_width) * m_vector.at(i + m_width);
		if (i < m_width * (m_height - 1) - 1) sum += m_matrix.at(i * (m_width * m_height + 1) + m_width + 1) * m_vector.at(i + m_width + 1);
		int mul = 1;
		for (size_t j = 0; j < m_rule.size(); ++j)
			mul *= (sum - m_rule.at(j));
		new_vector.at(i) = mul;
	}

	// Update field vector
	for (size_t i = 0; i < m_width * m_height; ++i)
		m_vector.at(i) = (new_vector.at(i) == 0 ? 1 : 0);
}

void Life::PrintMatrix()
{
	for (size_t i = 0; i < m_height * m_width; ++i) {
		for (size_t j = 0; j < m_width * m_height; ++j)
			std::cout << m_matrix.at(i * m_width * m_height + j) << ' ';
		std::cout << '\n';
	}
}

void Life::PrintVector()
{
	std::cout << m_gener << '\n';
	for (size_t i = 0; i < m_height - 1; i += 2) {
		for (size_t j = 0; j < m_width; ++j)
			std::cout << (m_vector.at(i * m_width + j) ? (m_vector.at((i + 1) * m_width + j) ? "\xDB" : "\xDF") : (m_vector.at((i + 1) * m_width + j) ? "\xDC" : " "));
		std::cout << '\n';
	}
	if (m_height % 2) {
		for (size_t j = 0; j < m_width; ++j)
			std::cout << (m_vector.at((m_height - 1) * m_width + j) ? "\xDF" : " ");
		std::cout << '\n';
	}
}

// Create transition matrix
void Life::create_matrix_()
{
	m_matrix.resize(m_width * m_width * m_height * m_height);
	for (size_t i = 0; i < m_height; ++i)
		for (size_t j = 0; j < m_width; ++j) {
			m_matrix.at((i * m_width + j) * (m_width * m_height + 1)) = 1;
			if (i > 0) m_matrix.at((i * m_width + j) * (m_width * m_height + 1) - m_width) = 2;
			if (i < m_height - 1) m_matrix.at((i * m_width + j) * (m_width * m_height + 1) + m_width) = 2;
			if (j > 0) m_matrix.at((i * m_width + j) * (m_width * m_height + 1) - 1) = 2;
			if (j < m_width - 1) m_matrix.at((i * m_width + j) * (m_width * m_height + 1) + 1) = 2;
			if (i > 0 && j > 0) m_matrix.at((i * m_width + j) * (m_width * m_height + 1) - m_width - 1) = 2;
			if (i > 0 && j < m_width - 1) m_matrix.at((i * m_width + j) * (m_width * m_height + 1) - m_width + 1) = 2;
			if (i < m_height - 1 && j > 0) m_matrix.at((i * m_width + j) * (m_width * m_height + 1) + m_width - 1) = 2;
			if (i < m_height - 1 && j < m_width - 1) m_matrix.at((i * m_width + j) * (m_width * m_height + 1) + m_width + 1) = 2;
		}
}