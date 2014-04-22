#ifndef _Sudoku_Solver_Sudoku_Solver_h_
#define _Sudoku_Solver_Sudoku_Solver_h_
#include <iostream>
using namespace std;
#include <Windows.h>
#include<stdio.h>
#pragma warning(disable : 4244 4101 4996 4305 4258)

#define time 10

// Setup Functions
void Reset_History();
void Undo_Move();
void Save_Move();
void Known_Reset();
void Possibilities_Reset();
void Input();
void Auto_Input();
void Handle_Input(int Input_Row, int Input_Column, int Input_Value);
void Output();

// Display Functions
void Display_Known();
void Display_History();
void Display_Possible();

// Diagnostic Functions
int Find_Num_Known();
int Find_Num_Possible(int Column, int Row);
bool Check_Contradictory_Values(int Column, int Row, int Value);
bool Check_Only(int Column, int Row, int Value);
bool ComparePossibilities(int Column1, int Row1, int Column2, int Row2);

// Solving functions
void Eliminate_Possible_Using_Known();
void Elimination_A();
void Elimination_B();
void Elimination_C();
void Elimination_D();
void Elimination_E();

// Data
int Known [9][9];
int Undo[64][9][9];
bool Possibilities [9][9][9];
int Known_Store [9][9];
bool Possibilities_Store [9][9][9];

char Opening_Message[] ="\nWelcome to the Sudoku Solver!\n\nEnter row, column, and number for each known square.\nThere must be at least 17 known squares.\n\n-1 as a value resets the board.\n-2 undoes you last move.\n10 displays possibiliies for each square.\n11 displays history up to 64 moves.\n12 outputs the values by simulating keystrokes.\n13 automatically inputs numbers.\n";

namespace blah
{
	bool Auto = false;
	bool Display_Contradictory = false;
	int prev = 0;
}
#endif
