#include "NRF24.h"
#include "DataReceived.h"
#include<iostream>
#include<time.h>
#include<string>
#include <cstring>
#include<ctime>

int main()
{
	DataReceived receiveData{};
	receiveData.StartReceiving();
}

