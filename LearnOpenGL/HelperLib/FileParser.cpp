#include "FileParser.h"

std::vector<std::string> split(const std::string &s, char delim);
void split(const std::string &s, char delim, std::vector<std::string> &elems);


FileParser::FileParser()
{
}


FileParser::~FileParser()
{
}

std::vector<glm::vec3> FileParser::extract_points(std::string file_path)
{
	std::vector<glm::vec3> points;
	std::ifstream file_handle(file_path);

	if (!file_handle) {
		std::cerr << "Could not open: " << file_path << std::endl;
		return points;
	}

	std::string line;
	while (std::getline(file_handle, line))
	{
		std::vector<std::string> numbers = split(line, ' ');

		if (numbers[0] != "v") continue;

		double x = std::stod(numbers[1]);
		double y = std::stod(numbers[2]);
		double z = std::stod(numbers[3]);
		glm::vec3 point(x, y, z);
		points.push_back(point);
	}

	return points;
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

void split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
}
