// ControlFlowGenerator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Line.h"
#include "Constant.h"
#include "BasicBlock.h"
#include "Util.h"
#include "ValidationProcessor.h"


#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <stack> 

using namespace std;

int main(int argc, char* argv[])
{
	list<Line> lines = ValidationProcessor::readAndValidateInput(argc, argv);

	//define variables
	list<BasicBlock> basicBlocks;
	Line newLine;
	int lineNumber;
	BasicBlock bk;

	//initialize variables
	lineNumber = 1;
	bk = *new BasicBlock();

	//Create first basic block
	BasicBlock startBasicBlock = Util::createStartBasicBlock();
	basicBlocks.push_back(startBasicBlock);


	//Create a list to store asic blocks
	stack<BasicBlock> basicBlocksEdges;

	for (list<Line>::iterator iterator = lines.begin(); iterator != lines.end(); ++iterator) {
		//Trim line
		newLine = (*iterator);
		string trimmedLine = newLine.getExpression();

		/* if */
		if (trimmedLine.find(Constant::KEYWORD_IF) != string::npos) {
			newLine.setLineType(Constant::LINE_TYPE_IF);
			bk.addLine(newLine);
			basicBlocks.push_back(bk);

			bk = *new BasicBlock();
		}
		/* while */
		else if (trimmedLine.find(Constant::KEYWORD_WHILE) != string::npos) {
			basicBlocks.push_back(bk);

			newLine.setLineType(Constant::LINE_TYPE_WHILE);

			bk = *new BasicBlock();
			bk.addLine(newLine);
			basicBlocks.push_back(bk);

			bk = *new BasicBlock();
		}
		/* else */
		else if (trimmedLine.find(Constant::KEYWORD_ELSE) != string::npos) {
			bk.setLineType(Constant::LINE_TYPE_ELSE);
			bk.addLine(newLine);
			basicBlocks.push_back(bk);
			bk = *new BasicBlock();
		}
		/* close */
		else if (trimmedLine.find(Constant::KEYWORD_CLOSE_BRACKET) != string::npos) {
			newLine.setLineType(Constant::LINE_TYPE_CLOSE);
			bk.addLine(newLine);

			bk.setLineType(Constant::LINE_TYPE_CLOSE);
			basicBlocks.push_back(bk);
			bk = *new BasicBlock();
		}
		/* basic line */
		else {
			newLine.setLineType(Constant::LINE_TYPE_EXPRESSION);
			bk.addLine(newLine);
		}

		//increase the line number
		lineNumber++;
	}

	//Create last basic block
	BasicBlock endBasicBlock = Util::createEndBasicBlock();
	basicBlocks.push_back(endBasicBlock);


	// Print the file path:
	cout << "BASIC BLOCKS" << endl;
	cout << "-------------" << endl;


	//Iterate all basic blocks
	for (list<BasicBlock>::iterator iterator = basicBlocks.begin(); iterator != basicBlocks.end(); ++iterator) {
		BasicBlock currentBasicBlock = (*iterator);
		list<Line> currentLineList = currentBasicBlock.getLines();

		if (currentBasicBlock.getLineType() == Constant::LINE_TYPE_EXPRESSION) {
			Line current = (*currentLineList.begin());
			if (current.getLineNumber() == 0) {
				//START
			}
			else {
				auto nx = std::prev(iterator, 1);
				BasicBlock previous = *(nx);

				previous.addEdge(currentBasicBlock);
			}

		}

		else if (currentBasicBlock.getLineType() == Constant::LINE_TYPE_IF) {
			//if previous is expression assign it
			auto nx = std::prev(iterator, 1);
			BasicBlock previous = *(nx);
			if (previous.getLineType() == Constant::LINE_TYPE_EXPRESSION) {
				previous.addEdge(currentBasicBlock);
			}
			else if (previous.getLineType() == Constant::LINE_TYPE_IF) {
				previous.addEdge(currentBasicBlock);
			}
			else if (previous.getLineType() == Constant::LINE_TYPE_WHILE) {
				previous.addEdge(currentBasicBlock);
			}
			else if (previous.getLineType() == Constant::LINE_TYPE_CLOSE) {
				previous.addEdge(currentBasicBlock);
			}
			else {
				for (list<BasicBlock>::iterator iteratorSecond = iterator; iteratorSecond != basicBlocks.begin(); --iteratorSecond) {
					BasicBlock previousBasicBlock = (*iteratorSecond);
					if (previous.getLineType() == Constant::LINE_TYPE_EXPRESSION) {
						previous.addEdge(currentBasicBlock);
					}
				}
			}

		}/*
		else if (currentBasicBlock.getLineType() == Constant::LINE_TYPE_EXPRESSION) {

		}
		else if (currentBasicBlock.getLineType() == Constant::LINE_TYPE_EXPRESSION) {

		}
		*/

		//Iterate all lines of current basic block
		for (list<Line>::iterator it = currentLineList.begin(); it != currentLineList.end(); ++it) {
			Line current = (*it);
			cout << current.getExpression() << endl;
		}
		cout << "" << "" << "" << endl;
	}

	return 0;
}

