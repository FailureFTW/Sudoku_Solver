#include "Sudoku_Solver.h"

/*
* Setup Functions
*/

	// Reads user input
void Input()
{
	int Input_Column, Input_Row, Input_Value = 0;

	// Displays opening message, shows current board, saves move to history
	cout << Opening_Message;
	Display_Known();
	Save_Move();
	
	cout << "Time: " << duration << endl;

	// User inputs column, row, and input value
	cout << "Column: ";
	cin >> Input_Column;
	cout << "Row: ";
	cin >> Input_Row;
	cout << "Value: ";
	cin >> Input_Value;
	cout << endl;
	Input_Row--;
	Input_Column--;

	// Interprets and acts on the input
	Display_Contradictory =	true;
	Auto = false;
	Handle_Input(Input_Row, Input_Column, Input_Value);
}

	// Interprets input, acts on it, and calls appropriate functions for that input
void Handle_Input(int Input_Row, int Input_Column, int Input_Value)
{
	int Num_Known_Values = 0;

	if ((Input_Value > 0 && Input_Value < 10) || Input_Value == 0) // Inputs 1-9 set a square on the board, 0 resets a square.
	{
		if (!Check_Contradictory_Values(Input_Column, Input_Row, Input_Value)) // Makes sure the value doesn't conflict with known squares
		{
			// Sets a square, and resets possible values to avoid possible false negatives
			Known [Input_Column][Input_Row] = Input_Value;
			Possibilities_Reset();
			Eliminate_Possible_Using_Known();
		}
		Display_Contradictory = false;
	}
	else if (Input_Value == -1) // Resets the board
	{
		system ("CLS");
		Known_Reset();
		Possibilities_Reset();
		Reset_History();
	}
	else if (Input_Value == -2) // Goes back a move
	{
		system ("CLS");
		Undo_Move();
	}
	else if (Input_Value == 10) // Displays the possible values of each square
	{
		system("CLS");
		Display_Possible();
	}
	else if (Input_Value == 11) // Displays previous moves
	{
		system("CLS");
		Display_History();
	}
	else if (Input_Value == 12 || Input_Value == 13) // Automatically inputs numbers
	{
		Display_Contradictory =	false;
		if (Input_Value == 12) // If input is 12, automatically outputs too
			Auto = true;
		else
			Auto = false;

		// Resets board before inputting
		Known_Reset();
		Possibilities_Reset();
		Reset_History();

		// 5 second delay so user can place cursor on board
		for (float a=50; a>0; a-=1)
		{
			system("CLS");
			cout << "Auto Inputting in: " << a;
			Sleep(100);
		}

		// Calls Auto_Input funcion
		Auto_Input();
		start = clock();
	}
	else if (Input_Value == 14) // Auto outputs the board
	{
		// 5 second delay so user can place cursor on board
		for (int a = 50; a > 0; a -= 1)
		{
			system("CLS");
			cout << "Outputting in: " << a;
			Sleep(100);
		}

		// Calls Auto_Output function
		Output();
	}
	else if (Input_Value == 15) // Shows instructions
	{
		system("CLS");
		cout << endl << Instructions;
		system("pause");
	}
}

	// This function automatically copies and records the board.
void Auto_Input()
{
	cout << endl;
	for (int row = 0; row < 9; row++)
	{
		for (int col = 0; col < 9; col++)
		{
			// Cretes and initializes variables
			int val = 0; // Value of the square being recorded
			HGLOBAL hText; // Handle for the clipboard data
			char *pText; // Text form of the square data
			hText = GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE, 100);
			pText = (char*)GlobalLock(hText);
			strcpy(pText, "0");
			GlobalUnlock(hText);

			// Empties clipboard
			OpenClipboard(NULL);
			EmptyClipboard();
			SetClipboardData(CF_TEXT, hText);
			CloseClipboard();
			
			// Copy function (Ctrl + C)
			keybd_event(0x11, 0, 0, 0);
			Sleep(time);
			keybd_event(0x43, 0, 0, 0);
			Sleep(time);
			keybd_event(0x11, 0, KEYEVENTF_KEYUP, 0);
			Sleep(time);
			keybd_event(0x43, 0, KEYEVENTF_KEYUP, 0);
			Sleep(time);

			// Retrieves square data from source
			OpenClipboard(NULL);
			HANDLE clip0 = GetClipboardData(CF_TEXT); // Gets data from clipboard
			GlobalLock(clip0);
			char* c = (char*) clip0; // Converts clipboard data to char
			sscanf(c, "%d" , &val); // Converts char to int
			GlobalUnlock(clip0);
			CloseClipboard();

			// Displays value and sets appropriate board location to that value
			cout << c << ", ";
			Handle_Input(row, col, val);
			keybd_event(0x27, 0, 0, 0);
			Sleep(time);
			keybd_event(0x27, 0, KEYEVENTF_KEYUP, 0);
			Sleep(time);
		}

		cout << endl;
		keybd_event(0x28, 0, 0, 0);
		Sleep(time);
		keybd_event(0x28, 0, KEYEVENTF_KEYUP, 0);
		Sleep(time);
	}

	cout << endl;
}
	// Sets history of all moves to zero
void Reset_History()
{
	for (int a = 0; a < 64; a++)
		for (int b = 0; b < 9; b++)
			for (int c = 0; c < 9; c++)
				Undo [a][c][b] = 0;
}

	// Goes back a move, shifts history backwards
void Undo_Move()
{
	for (int b = 0; b < 9; b++)
		for (int c = 0; c < 9; c++)
			Known[c][b] = Undo [1][c][b];
	for (int a = 0; a < 63; a++)
		for (int b = 0; b < 9; b++)
			for (int c = 0; c < 9; c++)
				Undo [a][c][b] = Undo [a+1][c][b];
	Possibilities_Reset();
	Eliminate_Possible_Using_Known();
}

	// Shifts history, adds current board to history
void Save_Move()
{
	for (int a = 63; a > 0; a--)
		for (int b = 0; b < 9; b++)
			for (int c = 0; c < 9; c++)
				Undo [a][c][b] = Undo [a-1][c][b];
	for (int b = 0; b < 9; b++)
		for (int c = 0; c < 9; c++)
			Undo [0][c][b] = Known[c][b];
}

	// Resets known values to zero.
void Known_Reset()
{
	for (int a = 0; a < 9; a++)
		for (int b = 0; b < 9; b++)
			Known [b][a] = 0;
}

	// Sets all possible values to true
void Possibilities_Reset()
{
	for (int a = 0; a < 9; a++)
		for (int b = 0; b < 9; b++)
			for (int c = 0; c < 9; c++)
				Possibilities [b][a][c] = true;
}

/*
* Display Functions
*/

	// Outputs values to source board. Known are displayed. Is a square is unknown, the possibilities are displayed
void Output()
{
	cout << "\nOutputting Numbers...";
	Display_Known();

	for (int a = 0; a < 9; a++)
	{
		for (int b = 0; b < 9; b++)
		{
			if (Known [b][a] > 0 && Known [b][a] < 10)
			{
				// Key of known square is pressed
				keybd_event(0x30 + Known [b][a], 0, 0, 0);
				Sleep(time);
				keybd_event(0x30 + Known [b][a], 0, KEYEVENTF_KEYUP, 0);
				Sleep(time);
			}
			else
			{
				for (int val = 0; val < 9; val++)
				{
					if (Possibilities[b][a][val])
					{
						// Key of posibility is pressed
						keybd_event(0x31 + val, 0, 0, 0);
						Sleep(time);
						keybd_event(0x31 + val, 0, KEYEVENTF_KEYUP, 0);
						Sleep(time);
					}
				}
			}

			keybd_event(0x27, 0, 0, 0);
			Sleep(time);
			keybd_event(0x27, 0, KEYEVENTF_KEYUP, 0);
			Sleep(time);
		}
		keybd_event(0x28, 0, 0, 0);
		Sleep(time);
		keybd_event(0x28, 0, KEYEVENTF_KEYUP, 0);
		Sleep(time);
	}
}

	// Displays the board of known squares

// It takes the following format, where X's are the square values. 1-9 are known, - is unknown.
/*
	[---------|---------|---------]
	[  X X X  |  X X X  |  X X X  ]
	[  X X X  |  X X X  |  X X X  ]
	[  X X X  |  X X X  |  X X X  ]
	[---------|---------|---------]
	[  X X X  |  X X X  |  X X X  ]
	[  X X X  |  X X X  |  X X X  ]
	[  X X X  |  X X X  |  X X X  ]
	[---------|---------|---------]
	[  X X X  |  X X X  |  X X X  ]
	[  X X X  |  X X X  |  X X X  ]
	[  X X X  |  X X X  |  X X X  ]
	[---------|---------|---------]
*/

void Display_Known()
{
	int count1 = 0;
	int count2 = 0;

	cout << endl;
	cout << "    [---------|---------|---------]\n";

	for (int a = 0; a < 9; a++)
	{
		cout << "    [";
		count2 = 0;

		for (int b = 0; b < 9; b++)
		{
			if (count2 > 2)
			{
				cout << "|";
				count2 = 0;
			}

			count2++;

			if (Known [b][a] == 0)
				cout << " " << "-" << " ";
			else
				cout << " " << Known [b][a] << " ";
		}

		count1++;
		cout << "]\n";

		if (count1 > 2)
		{
			cout << "    [---------|---------|---------]\n";
			count1 = 0;
			count2 = 0;
		}
	}

	cout << endl;
}

	// Displays the prevoius moves
void Display_History()
{
	int x = 0;
	cout << endl << "Number of moves: ";
	cin >> x;
	if (!(x>1 && x<64))
		x = 4;

	for (int h=(x-1); h>=0; h--)
	{
		int count1 = 0;
		int count2 = 0;

		cout << "\n";
		cout << "    [---------|---------|---------]\n";

		for (int a = 0; a < 9; a++)
		{
			cout << "    [";
			count2 = 0;
			for (int b = 0; b < 9; b++)
			{
				if (count2 > 2)
				{
					cout << "|";
					count2 = 0;
				}
				count2++;
				if (Undo [h][b][a] == 0)
					cout << " " << "-" << " ";
				else
					cout << " " << Undo [h][b][a] << " ";
			}

			count1++;
			cout << "]\n";
			if (count1 > 2)
			{
				cout << "    [---------|---------|---------]\n";
				count1 = 0;
				count2 = 0;
			}
		}
	}
	cout << endl;
}

	// Displays the possibilities of each square
// Takes the following format.
// There is a set for each square, displaying the possibility of each square
// Each value is boolean

/*
	{Row, Column}: Value_1, Value_2 ... Value_9
*/ 
	
void Display_Possible()
{
	cout << endl;
	for (int a = 0; a < 9; a++)
	{
		for (int b = 0; b < 9; b++)
		{
			cout << "    {" << a + 1 << ", " << b + 1 << "} ";
			for (int c = 0; c < 9; c++)
			{
				if (Possibilities [a][b][c])
					cout << c + 1 << ",";
				else
					cout << ",,";
			}
			cout << endl;
		}
		cout << endl;
	}
}

/*
* Diagnostic Functions
*/

	// Finds the number of values that are known
int Find_Num_Known()
{
	int Num_Known_Values = 0;

	for (int a = 0; a < 9; a++)
		for (int b = 0; b < 9; b++)
			if (Known [b][a] != 0)
				Num_Known_Values++;
	return Num_Known_Values;
}

	// Finds the number of possibilities of a given square
int Find_Num_Possible (int Column, int Row)
{
	int count = 0;

	for (int a = 0; a < 9; a++)
		if (Possibilities [Column][Row][a])
			count++;

	return count;
}

	// Checks a square to make sure that there aren't other known squares that conflict with the chosen value
bool Check_Contradictory_Values (int Column, int Row, int Value)
{
	if (Value == 0) // Values of 0 are disregarded
		return false;

	// Check in same row.
	for (int a = 0; a < 9; a++)
	{
		if (Known [a][Row] == Value && a != Column)
		{
			if (Display_Contradictory)
				cout << "\nContradicting value at:\n - Column " << (a + 1) << "\n - Row " << (Row + 1) << "\n - Value " << Value << "\n\n";
			return true;
		}
	}

	// Check in same column.
	for (int a = 0; a < 9; a++)
	{
		if (Known [Column][a] == Value && a != Row)
		{
			if (Display_Contradictory)
				cout << "\nContradicting value at:\n - Column " << (Column + 1) << "\n - Row " << (a + 1) << "\n - Value " << Value << "\n\n";
			return true;
		}
	}

	// Check same group.
	for (int a = Row - (Row % 3); a < Row - (Row % 3) + 3; a++)
	{
		for (int b = Column - (Column % 3); b < Column - (Column % 3) + 3; b++)
		{
			if (Known [b][a] == Value)
			{
				if (! (a == Row && b == Column))
				{
					if (Display_Contradictory)
						cout << "\nContradicting value at:\n - Column " << (b + 1) << "\n - Row " << (a + 1) << "\n - Value " << Value << "\n\n";
					return true;
				}
			}
		}
	}
	return false;
}

	// Returns true if two squares have the same possibilities
bool ComparePossibilities (int Column1, int Row1, int Column2, int Row2)
{
	for (int a = 0; a < 9; a++)
	{
		if (!Possibilities [Column1][Row1][a] && Possibilities [Column2][Row2][a])
			return false;
		else if (Possibilities [Column1][Row1][a] && !Possibilities [Column2][Row2][a])
			return false;
	}
	return true;
}

/*
* Solving and elimination functions.
*/
	// Uses the known value for a square to eliminate all other possibilities.
void Eliminate_Possible_Using_Known()
{
	for (int a = 0; a < 9; a++)
	{
		for (int b = 0; b < 9; b++)
		{
			if (Known [b][a] > 0 && Known [b][a] < 10)
			{
				for (int c = 0; c < 9; c++)
					Possibilities [b][a][c] = false;
				Possibilities [b][a][Known [b][a] - 1] = true;
			}
		}
	}
}

	// Checks known cosquares for any known numbers and eliminates those values accordingly.
void Elimination_A()
{
	for (int a = 0; a < 9; a++)
		for (int b = 0; b < 9; b++)
			for (int c = 0; c < 9; c++)
				if (Check_Contradictory_Values (b, a, c + 1))
					Possibilities [b][a][c] = false;
}

	// Sees if there is only one possibility for a given square.
void Elimination_B()
{
	for (int a = 0; a < 9; a++)
		for (int b = 0; b < 9; b++)
			if (Find_Num_Possible (b, a) == 1)
				for (int c = 0; c < 9; c++)
					if (Possibilities [b][a][c])
						Known [b][a] = c + 1;
}

	// Checks if there is only one square in a row, column, or group that has a certain possible value.
void Elimination_C()
{
	int count;

	// Check row by row.
	for (int a = 0; a < 9; a++)
	{
		for (int b = 0; b < 9; b++)
		{
			count = 0;
			for (int c = 0; c < 9; c++)
				if (Possibilities [c][a][b])
					count++;

			if (count == 1)
				for (int c = 0; c < 9; c++)
					if (Possibilities [c][a][b])
						Known [c][a] = b + 1;
		}
	}

	// Check column by column.
	for (int a = 0; a < 9; a++)
	{
		for (int b = 0; b < 9; b++)
		{
			count = 0;

			for (int c = 0; c < 9; c++)
				if (Possibilities [a][c][b])
					count++;

			if (count == 1)
				for (int c = 0; c < 9; c++)
					if (Possibilities [a][c][b])
						Known [a][c] = b + 1;
		}
	}


	// Check group by group.
	for (int vert = 0; vert < 3; vert++)
	{
		for (int horiz = 0; horiz < 3; horiz++)
		{
			for (int a = 0; a < 3; a++)
			{
				count = 0;

				for (int b = 0; b < 3; b++)
					for (int c = 0; c < 3; c++)
						if (Possibilities [c + 3 * horiz][b + 3 * vert][a])
							count++;
				if (count == 1)
					for (int b = 0; b < 3; b++)
						for (int c = 0; c < 3; c++)
							if (Possibilities [c + 3 * horiz][b + 3 * vert][a] && Known [c + 3 * horiz][b + 3 * vert] == 0)
								Known [c + 3 * horiz][b + 3 * vert] = a + 1;
			}
		}
	}
}

	// Naked twin exclusion
void Elimination_D()
{
	// Check twins in rows. 
	for (int a = 0; a < 9; a ++) // Row of 1st number being checked
		for (int b = 0; b < 9; b++) // Column of 1st number being checked
			if (Find_Num_Possible (b, a) == 2) // Checks if there are only 2 possibilities for that square
				for (int c = b + 1; c < 9; c++) // Column of 2nd number being searched
					if (ComparePossibilities(b, a, c, a)) // If the possibilities of both squares being searched match
						for (int d = 0; d < 9; d++) // Goes down the row to eliminate possibilities of the other squares
							if (d != b && d != c) // Skips the squares being searched
								for (int e = 0; e < 9; e++) // Goes down all possibilities of the other square
									if (Possibilities [b][a][e]) // If the value in either twin is true
										Possibilities [d][a][e] = false; // The corresponding value in the is set to false for the other square

	// Check twins in columns. (Same thing as prevoius, but flips rows and columns for functions.)
	for (int a = 0; a < 9; a ++) // Column of 1st
		for (int b = 0; b < 9; b++) // Row of 1st
			if (Find_Num_Possible (a, b) == 2)
				for (int c = b + 1; c < 9; c++) // Column of 2nd
					if (ComparePossibilities(a, b, a, c))
						for (int d = 0; d < 9; d++) // Rows of other
							if (d != b && d != c)
								for (int e = 0; e < 9; e++)
									if (Possibilities [a][b][e])
										Possibilities [a][d][e] = false;
	
	// Checks twins in groups
	/*
	// Check same group.
	for (h = 0; h < 3; h++) // Columns of groups
		for (i = 0; i < 3; i++) // Rows of groups

			for (a = h * 3; a < h * 3 + 3; a ++) // Comp1 col
				for (b = i * 3; b < i * 3 + 3; b ++) // Comp1 row
					if (Find_Num_Possible (a, b) == 2) // Get num poss

						for (c = h * 3; c < h * 3 + 3; c ++) // Comp2 col
							for (d = i * 3; d < i * 3 + 3; d ++) // Comp2 row
								if ((a != c) && (b != d)) // Avoids comparing a square with itself
									if (ComparePossibilities(a, b, c, d)) // Compare Comp1 and Comp2

										for (e = h * 3; e < h * 3 + 3; e ++) // Goes down the columns in a group
											for (f = i * 3; f < i * 3 + 3; f ++) // Goes down the rows in a group
												if (!((e == a) && (f == b)) && !((e == c) && (f == d))) // Skips the twin squares
													for (g = 0; g < 9; g++)
														if (Possibilities[b][a][g])
															Possibilities [g][f][g] = false;
	*/
}

	// Guess and check
	// This simulates a square
	// If it finds a solution, it accepts that solution.
	// If it concludes a square is incorrect, that square is marked as such

void Elimination_E()
{
	int prev_store = Previous;

	// Stores current known values as backup
	for (int a = 0; a < 9; a++)
		for (int b = 0; b < 9; b++)
			Known_Store [a][b] = Known [a][b];
	
	// Stores possibilities as backup
	for (int a = 0; a < 9; a++)
		for (int b = 0; b < 9; b++)
			for (int c = 0; c < 9; c++)
				Possibilities_Store [a][b][c] = Possibilities [a][b][c];
	
	// Does a simulation for each possible value of each square
	for (int a = 0; a < 9; a++)
	{
		for (int b = 0; b < 9; b++)
		{
			for (int c = 0; c < 9; c++)
			{
				if (Possibilities [b][a][c])
				{
					Known [b][a] = c + 1; // Sets the square to the value being tested

					while (1)
					{
						int Num_Sweeps = 2;

						for (int i = 0; i < Num_Sweeps; i++) 
						{
							Elimination_A();
							Elimination_B();
							Elimination_C();
							Elimination_D();
						}

						if ((Find_Num_Known() - Previous) == 0)
						{
							break; // If no progress is being made, restore the board and move on to a new value to simulate
						}

						if (Find_Num_Known() >= 81) // All 81 squares have been found
						{	
							bool Fine = true; // True means there are no problems with the board so far
							for (int a = 0; a < 9; a++)
								for (int b = 0; b < 9; b++)
									if (Check_Contradictory_Values (b, a, Known [b][a])) // There is a conflict in the squares
										Fine = false;

							if (Fine) // If all 81 squares are found and there are no conflicts, then the board has been solved
							{
								goto Done;
							}
							else // If all 81 squares are found, but there are conflicts, then that means the simulated value doesn't work
							{
								Possibilities_Store [b][a][c] = false;
								Possibilities [b][a][c] = false;
								break;
							}
						}
						Previous = Find_Num_Known();
					}
					
					// Restore all values
					for (int a = 0; a < 9; a++)
						for (int b = 0; b < 9; b++)
							Known [a][b] = Known_Store [a][b];
					for (int a = 0; a < 9; a++)
						for (int b = 0; b < 9; b++)
							for (int c = 0; c < 9; c++)
								Possibilities [a][b][c] = Possibilities_Store [a][b][c];
				}

				Previous = prev_store;
			}
		}
	}
Done:
	Previous = prev_store;
}

int main()
{
	duration = 0;
	Known_Reset();
	Possibilities_Reset();
	Reset_History();
	Previous = 0;
	Auto = false;
	int Num_Sweeps = 2;

	while(1)
	{
		do
		{
			Previous = Find_Num_Known();
			for (int a = 0; a < Num_Sweeps; a++) // Sweeps through the solving functions twice for good measure before continuing
			{
				Elimination_A();
				Elimination_B();
				Elimination_C();
				Elimination_D();
			}
			if (Find_Num_Known() == Previous && (Find_Num_Known() >= 17)) // At least 17 squares are needed to have a unique solution
				Elimination_E();
		}
		while (Find_Num_Known() != Previous);
		
		duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;

		if (Auto)
			Output();
		Auto = false;
		Input();
	}
}
