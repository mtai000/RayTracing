#pragma once
#include <iostream>
#include <chrono>

class MyTimer {
public:
	MyTimer() {
		start = std::chrono::system_clock::now();
	}
	~MyTimer() { Print(); }

	void Print() {
		auto stop = std::chrono::system_clock::now();
		std::cout << "Render complete: \n";
		std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
		std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() << " minutes\n";
		std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " seconds\n";
	}
private:
	std::chrono::system_clock::time_point start;
	std::chrono::system_clock::time_point stop;
};