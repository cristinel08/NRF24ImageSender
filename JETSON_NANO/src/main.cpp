#include "NRF24.h"
#include "DataReceived.h"
#include<iostream>
#include<time.h>
#include<string>
#include <cstring>
#include<ctime>

int main()
{
	char a{};
	DataReceived receiveData{};
	receiveData.StartReceiving();
	std::cin >> a;
}

