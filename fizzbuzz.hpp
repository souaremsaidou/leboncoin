#include <string> // std::string, std::to_string
#include <vector>

std::vector<std::string> fizzbuzz(int int1, int int2, int limit, std::string str1, std::string str2)
{
	std::vector<std::string> values;
	values.reserve(limit);
	for (int i = 1; i < limit + 1; ++i)
	{
		if (i % int1 == 0 && i % int2 == 0)
		{
			values.push_back(str1 + str2);
		}
		else if (i % int1 == 0)
		{
			values.push_back(str1);
		}
		else if (i % int2 == 0)
		{
			values.push_back(str2);
		}
		else
		{
			values.push_back(std::to_string(i));
		}
	}

	return values;
}
