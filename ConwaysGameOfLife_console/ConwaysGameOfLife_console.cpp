// ConwaysGameOfLife_console.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

// Class, computing lifecycle
class Life
{
private:
	size_t m_width, m_height;
	unsigned int m_gener{ 0 };
	std::vector<int> m_vector, m_matrix, m_rule;

	// Create transition matrix
	void create_matrix_()
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

public:
	// Constructor from given size
	Life(size_t width, size_t height) : m_width(width), m_height(height)
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
	Life(const char filename[])
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

	void Update()
	{
		// New generation
		++m_gener;

		// Multiply matrices
		// TODO: This matrix can be multiplied faster:
		//       it's symmetric and has at maximum 9 non-zero elements in a row
		std::vector<int> new_vector(m_width * m_height);
		for (size_t i = 0; i < m_width * m_height; ++i) {
			int sum = 0;
			for (size_t j = 0; j < m_width * m_height; ++j)
				sum += m_matrix.at(i * m_width * m_height + j) * m_vector.at(j);
			int mul = 1;
			for (size_t j = 0; j < m_rule.size(); ++j)
				mul *= (sum - m_rule.at(j));
			new_vector.at(i) = mul;
		}

		// Update field vector
		for (size_t i = 0; i < m_width * m_height; ++i)
			m_vector.at(i) = (new_vector.at(i) == 0 ? 1 : 0);
	}

	void PrintMatrix()
	{
		for (size_t i = 0; i < m_height * m_width; ++i) {
			for (size_t j = 0; j < m_width * m_height; ++j)
				std::cout << m_matrix.at(i * m_width * m_height + j) << ' ';
			std::cout << '\n';
		}
	}

	void PrintVector()
	{
		std::cout << m_gener << '\n';
		for (size_t i = 0; i < m_height; i += 2) {
			for (size_t j = 0; j < m_width; ++j)
				std::cout << (m_vector.at(i * m_width + j) ? (m_vector.at((i + 1) * m_width + j) ? "\xDB" : "\xDF") : (m_vector.at((i + 1) * m_width + j) ? "\xDC" : " "));
			std::cout << '\n';
		}
	}
};

int main()
{
	//size_t width, height;
	//std::cin >> width >> height;
	//Life* life = new Life(width, height);
	
	Life* life = new Life("start_pos.txt");

	//life->PrintMatrix();
	//system("pause");

	life->PrintVector();
	while (true) {
		life->Update();
		//system("pause");
		system("cls");
		life->PrintVector();
	}
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
