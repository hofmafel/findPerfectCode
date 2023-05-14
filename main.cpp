#include<vector>
#include<iostream>
#include<memory>
#include<numeric>
#include<random>
#include<thread>
#include<mutex>
#include<chrono> 
#include<fstream>
#include<intrin.h>
#include<string>

#define NUMBEROFRESULTS 1

class Stage;

unsigned int codesFound = 0;
std::vector<std::vector<unsigned int>> results;


class Stage
{
public:
	Stage(std::vector<unsigned int> els, unsigned int last) : last_element(last)
	{
		if (codesFound > NUMBEROFRESULTS-1)
			return;
		
		elements = els;

		if (elements.size() == 16)
		{
			if (everythingCovered())
			{
				results.emplace_back(elements);
				codesFound++;
				return;
			}
			else
			{
				discard = true;
			}

		}
		if (discard == false)
			for (auto e : elements)
			{
				if (__popcnt(last_element ^ e) < 3)
					discard = true;
			}

		if (discard == false)
		{

			elements.emplace_back(last_element);

			for (int i = 0; i < 128; i++)
			{
				stages.push_back(std::make_unique<Stage>(elements, i));
			}
		}

		if (discard == true)
		{
			elements.clear();
		}

	}
	// is every bitsting in an e-Ball?
	bool everythingCovered()
	{
		int smallest_distance = 7;
		for (int u = 0; u < 128; u++)
		{
			for (const auto& c : elements)
			{
				if (__popcnt(u ^ c) < smallest_distance)
					smallest_distance = __popcnt(u ^ c);
			}
			if (smallest_distance > 1)
				return false;

		}
		return true;
	}

	bool discard = false;
	std::vector<unsigned int>  elements;
	unsigned int last_element;
	std::vector<std::unique_ptr<Stage>> stages;
};



std::string decToBin(unsigned int value)
{
	std::string result = "0000000";
	unsigned int remainder;

	for (int i = 0; i < 7; i++)
	{
		result[6 - i] = (((1u << i) & value)>>i)+48;

	}
	return result;
}
void findCodes(unsigned int seed0,unsigned int seed1)
{
	std::vector<unsigned int> elements;
	elements.emplace_back(seed0);
	static Stage stage(elements, seed1);
}
std::string c_i(int i)
{
	
	return "$c_{" + std::to_string(i) + "}$";
}
void latexSave(std::vector<unsigned int> & result)
{
	std::ofstream file;

	file.open("output.tex", std::ios_base::app);
	file << "\n\n\n";
	file << "\\subsection{Code ";
	for (auto c : result)
		file << c;
	file << "}\n\n";

	file << "\\subsubsection{code-words:} \\\\\n";

	file << "\\begin{tabular}{c | c}\n";
	file << "# & Code\\\\\n";
	file << "\\hline\n";
	for (int i = 0; i < result.size(); i++)
		file << " $c_" << i<<"$" << "&" << decToBin(result[i]) << "\\\\ \n";

	file << "\\end{tabular}\n";

	file << "\\subsubsection{distances:} \\\\\n";

	file << "\\begin{tabular}{c | c| c| c| c| c| c| c| c| c| c| c| c| c| c| c| c}\n";
	file << "\n \\hline\n $c_i$&";
	for (auto i = 0; i < result.size(); i++)
		file << " " << c_i(i) << "&";
	file << "\n\n";
	for (auto i = 0; i < result.size(); i++)
	{
		file << c_i(i);
		for (auto j = 0; j < result.size(); j++)
		{
			file <<"&" << __popcnt(result[i] ^ result[j]);
		}
		file << "\\\\\n \\hline\n";
	}
	file << "\\end{tabular}\n";


	file << "\\subsubsection{Error-Ball:} \\\\\n";
	file << "(Which code word does word belong to)\n\n\n";

	file << "\\begin{tabular}{c | c | c}\n";
	file << "Word & Code-Word & Code-Word (Binary)\\\\\n";
	file << "\\hline\n";
	for (unsigned int u = 0u; u < 128; u++)
	{
		if (u == 64)
		{
			file << "\n\\end{tabular}\n";
			file << "\\begin{tabular}{c | c | c}\n";
			file << "Word & Code-Word & Code-Word (Binary)\\\\\n";
			file << "\\hline\n";
		}
		for (unsigned int i = 0; i < result.size(); i++)
		{
			if(__popcnt(u ^ result[i]) == 1)
				file << decToBin(u) << "& " << i << "&" << decToBin(i) << "\\\\\n";

		}
	}
	file << "\\end{tabular}\n\n";

}



int main()
{
	unsigned int seed0, seed1;
	std::cout << "Enter seed 0:";
	std::cin >> seed0;

	std::cout << "\nEnter seed 1:";
	std::cin >> seed1;

	if(__popcnt(seed0 ^ seed1) < 3)
	{
		std::cout << "Hamming distance of seeds too small.\n";
		return -1;
	}

	std::thread computer(findCodes, seed0, seed1);

	while(codesFound < NUMBEROFRESULTS)
	{ }
	std::cout << "Codes found!";
	for (auto e : results)
		latexSave(e);

	computer.join();
	return 0;
}
