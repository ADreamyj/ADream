#include "Dream.hpp"

int main()
{
//	Robot r;
//	std::string str;
//	volatile bool quit = false;
//	while(!quit)
//	{
//		std::cout << "我# ";
//		std::cin >> str;
//
//		std::string s = r.Talk(str);
//		std::cout << "ADream$ " << s << std::endl;
//	
//	}
	Dream * dr = new Dream();
	if(!dr->LoadEtc())
	{
		return 1;
	}
		
	dr->Run();
	return 0; 
}
