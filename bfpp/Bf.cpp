/*
7 * Bf.cpp
 *
 *  Created on: 13 нояб. 2016 г.
 *      Author: radiolok
 */

#include "Bf.h"

Bf::Bf() {
	// TODO Auto-generated constructor stub

}

Bf::~Bf() {
	// TODO Auto-generated destructor stub
}

bool Bf::IsASymbol(uint8_t c){
	bool result = false;
	switch (c){
	case '>':
	case '<':
	case '+':
	case '-':
	case '.':
	case ',':
	case '[':
	case ']':
	case '~': //is a debug symbol
		result = true;
		break;
	default:

		break;
	}

	return result;
}

bool Bf::IsAIncrementer(uint8_t c){
	bool result = false;
	switch (c){
	case '<':
	case '>':
	case '+':
	case '-':
		result = true;
		break;
	default:
		break;
	}
	return result;
}

bool Bf::IsMemInc(uint8_t c){
	bool result = false;
	switch (c){
	case '>':
	case '<':
		result = true;
		break;
	default:

		break;
	}

	return result;
}

uint8_t Bf::Translate(const uint8_t *SourceBuffer, size_t length, std::vector<Cmd> &Output){
	uint8_t status = SUCCESS;

	uint8_t CurrentCmd = 0;
	size_t CurrentBias = 0;
	size_t LoopMarkIterator;
	//Need to clear out unused symbols:
	size_t new_length = 0;
	uint8_t *NewBuffer = new uint8_t[length];
	for (size_t i = 0 ; i < length; ++i){
		if (IsASymbol(SourceBuffer[i])){
			NewBuffer[new_length] = SourceBuffer[i];
			++new_length;
		}
	}
	//Start translating
	for (size_t i = 0 ; i < new_length; ++i)
	 {
		 if (IsASymbol(NewBuffer[i]))
		 {
			 CurrentCmd = NewBuffer[i];
			 switch (CurrentCmd){
			 	 //For this Cmd's we need to calc shift:
			 	case '>':
				case '<':
				case '+':
				case '-':
					CurrentBias++;
					if (i < new_length -1){
						if (CurrentCmd != NewBuffer[i+1]){
							Output.push_back(Cmd(CurrentCmd, CurrentBias));
							CurrentBias = 0;
						}
					}
					else{
						Output.push_back(Cmd(CurrentCmd, CurrentBias));
						CurrentBias = 0;
					}
					break;
				case '.':
				case ',':
					Output.push_back(Cmd(CurrentCmd));
					CurrentBias = 0;
					break;
				case '[':
				case ']':
					std::string LoopMark = "Mark" + LoopMarkIterator++;
					Output.push_back(Cmd(CurrentCmd, LoopMark));
					CurrentBias = 0;
					break;
 			 }
		 }
	 }
	return status;
}

size_t Bf::FindLoopEnd(std::vector<Cmd> &Output, size_t CurrentIp){
	size_t i = 1;
	size_t NewIp = CurrentIp;
	size_t MaxIp = Output.size();
	while (i){
		NewIp++;
		if (NewIp >=MaxIp){
			return 0;
		}
		switch(Output[NewIp].GetCmdChar()){
		case '[':
			i++;
			break;
		case ']':
			i--;
			break;
		}
	}
	//We found loop ending:
	Output[NewIp].SetBias(-(NewIp-CurrentIp));
	return NewIp;
}


uint8_t Bf::Linking(std::vector<Cmd> &Output){
	uint8_t status = SUCCESS;
	//Let's Get Pointers for loops:
	//[ should get shift > 0
	//] should get shift < 0
	size_t CurrentIp = 0;
	size_t MaxIp = Output.size();
	if (MaxIp == CurrentIp){
		return LINKING_ERROR;
	}

	for (CurrentIp = 0; CurrentIp < MaxIp; CurrentIp++){
		if (Output[CurrentIp].GetCmdChar() == '['){
			//Let's find Loop Closing:
			size_t CloseIp = FindLoopEnd(Output, CurrentIp);
			if (CloseIp){
				Output[CurrentIp].SetBias(CloseIp-CurrentIp);
			}
			else{
				return LINKING_ERROR;
			}
		}
	}

	return status;
}

uint8_t Bf::Compile(const uint8_t *SourceBuffer, size_t length, std::vector<Cmd> &Output){

	uint8_t status = SUCCESS;

	status = Translate(SourceBuffer, length, Output);
	if (status){
		return TRANSLATION_ERROR;
	}

	status = Linking(Output);
	if (status){
		return LINKING_ERROR;
	}
	return status;
}
