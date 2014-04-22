#include "Sudoku_Solver.h"
/*
* Setup Functions
*/
void Reset_History()
{
	for (int a = 0; a < 64; a++)
		for (int b = 0; b < 9; b++)
			for (int c = 0; c < 9; c++)
				Undo [a][c][b] = 0;
}

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
void Known_Reset()
{
	for (int a = 0; a < 9; a++)
		for (int b = 0; b < 9; b++)
			Known [b][a] = 0;
}

void Possibilities_Reset()
{
	for (int a = 0; a < 9; a++)
		for (int b = 0; b < 9; b++)
			for (int c = 0; c < 9; c++)
				Possibilities [b][a][c] = true;
}

void Input()
{
	int Input_Column, Input_Row, Input_Value = 0;

	cout << Opening_Message;
	Display_Known();
	Save_Move();
	cout << "Column: ";
	cin >> Input_Column;
	cout << "Row: ";
	cin >> Input_Row;
	cout << "Value: ";
	cin >> Input_Value;
	cout << endl;
	Input_Row--;
	Input_Column--;

	blah::Display_Contradictory = true;
	Handle_Input(Input_Row, Input_Column, Input_Value);
	blah::Display_Contradictory = false;
}

void Auto_Input()
{
	cout << endl;
	for (int row = 0; row < 9; row++)
	{
		for (int col = 0; col < 9; col++)
		{
			int val = 0;

			HGLOBAL hText;
			char *pText;
			hText = GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE, 100);
			pText = (char*)GlobalLock(hText);
			strcpy(pText, "0");
			GlobalUnlock(hText);

			OpenClipboard(NULL);
			EmptyClipboard();
			SetClipboardData(CF_TEXT, hText);
			CloseClipboard();
			
			// Copy function
			keybd_event(0x11, 0, 0, 0);
			Sleep(time);
			keybd_event(0x43, 0, 0, 0);
			Sleep(time);
			keybd_event(0x11, 0, KEYEVENTF_KEYUP, 0);
			Sleep(time);
			keybd_event(0x43, 0, KEYEVENTF_KEYUP, 0);
			Sleep(time);

			OpenClipboard(NULL);
			HANDLE clip0 = GetClipboardData(CF_TEXT);
			GlobalLock(clip0);
			char* c = (char*) clip0;
			sscanf(c, "%d" , &val);
			GlobalUnlock(clip0);
			CloseClipboard();
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

void Handle_Input(int Input_Row, int Input_Column, int Input_Value)
{
	int Num_Known_Values = 0;

	if ((Input_Value > 0 && Input_Value < 10 && !Check_Contradictory_Values(Input_Column, Input_Row, Input_Value)) || Input_Value == 0)
	{
		Known [Input_Column][Input_Row] = Input_Value;
		if (Input_Value == 0)
		{
			Possibilities_Reset();
			Eliminate_Possible_Using_Known();
		}
	}
	else if (Input_Value == -1)
	{
		system ("CLS");
		Known_Reset();
		Possibilities_Reset();
		Reset_History();
	}
	else if (Input_Value == -2)
	{
		system ("CLS");
		Undo_Move();
	}
	else if (Input_Value == 10)
	{
		system("CLS");
		Display_Possible();
	}
	else if (Input_Value == 11)
	{
		system("CLS");
		Display_History();
	}
	else if (Input_Value == 12)
	{
		for (int a = 50; a > 0; a -= 1)
		{
			system("CLS");
			cout << "Outputting in: " << a;
			Sleep(100);
		}
		Output();
	}
	else if (Input_Value == 13)
	{
		blah::Auto = true;
		Known_Reset();
		Possibilities_Reset();
		Reset_History();
		for (float a=50; a>0; a-=1)
		{
			system("CLS");
			cout << "Auto Inputting in: " << a;
			Sleep(100);
		}
		Auto_Input();
	}
}
/*
* Display Functions
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

int Find_Num_Known()
{
	int Num_Known_Values = 0;

	for (int a = 0; a < 9; a++)
		for (int b = 0; b < 9; b++)
			if (Known [b][a] != 0)
				Num_Known_Values++;
	return Num_Known_Values;
}

int Find_Num_Possible (int Column, int Row)
{
	int count1 = 0;

	for (int a = 0; a < 9; a++)
		if (Possibilities [Column][Row][a])
			count1++;

	return count1;
}

bool Check_Contradictory_Values (int Column, int Row, int Value)
{
	// Check in same row.
	for (int a = 0; a < 9; a++)
	{
		if (Known [a][Row] == Value && a != Column)
		{
			if (blah::Display_Contradictory)
				cout << "\nContradicting value at:\n - Column " << (a + 1) << "\n - Row " << (Row + 1) << "\n - Value " << Value << "\n\n";
			return true;
		}
	}

	// Check in same column.
	for (int a = 0; a < 9; a++)
	{
		if (Known [Column][a] == Value && a != Row)
		{
			if (blah::Display_Contradictory)
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
					if (blah::Display_Contradictory)
						cout << "\nContradicting value at:\n - Column " << (b + 1) << "\n - Row " << (a + 1) << "\n - Value " << Value << "\n\n";
					return true;
				}
			}
		}
	}
	return false;
}

/*
* Solving and elimination functions.
*/

void Eliminate_Possible_Using_Known() // Uses the known value for a square to eliminate all other possibilities.
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

void Elimination_A() // Checks known cosquares for any known numbers and eliminates those values accordingly.
{
	int a, b, c;
	Eliminate_Possible_Using_Known();

	for (a = 0; a < 9; a++)
		for (b = 0; b < 9; b++)
			for (c = 0; c < 9; c++)
				if (Check_Contradictory_Values (b, a, c + 1))
					Possibilities [b][a][c] = false;
}

void Elimination_B() // Sees if there is only one possibility for a given square.
{
	int a, b, c;

	for (a = 0; a < 9; a++)
		for (b = 0; b < 9; b++)
			if (Find_Num_Possible (b, a) == 1)
				for (c = 0; c < 9; c++)
					if (Possibilities [b][a][c])
						Known [b][a] = c + 1;
}

void Elimination_C() // Checks if there is only one square in a row, column, or group that has a certain possible value.
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

void Elimination_D() // Naked twin exclusion
{
	// Check twins in rows. 
	for (int a = 0; a < 9; a ++) // Comp1 row
		for (int b = 0; b < 9; b++) // Comp1 col
			if (Find_Num_Possible (b, a) == 2) // Get num poss

				for (int c = b + 1; c < 9; c++) // Comp2 col
					if (ComparePossibilities(b, a, c, a)) // Compare the Comp1 and Comp2

						for (int d = 0; d < 9; d++) // Goes down cols to eliminate poss of each square
							if (d != b && d != c) // Skips Comp1 and Comp2
								for (int e = 0; e < 9; e++) // Goes down all poss 1-9
									if (Possibilities [b][a][e]) // If the val in one twin is true
										Possibilities [d][a][e] = false; // The corresponding val is set to false

	// Check twins in columns. (Same thing as prevoius, but flips rows and columns for functions.)
	for (int a = 0; a < 9; a ++) // Comp1 col
		for (int b = 0; b < 9; b++) // Comp1 row
			if (Find_Num_Possible (a, b) == 2) // Get num poss

				for (int c = b + 1; c < 9; c++) // Comp 2 row
					if (ComparePossibilities(a, b, a, c)) // Compare Comp1 and Comp2

						for (int d = 0; d < 9; d++) // Goes down rows to eliminate poss of each square
							if (d != b && d != c) // Skips Comp1 and Comp2
								for (int e = 0; e < 9; e++) // Goes down all poss 1-9
									if (Possibilities [a][b][e]) // If the value in one twin is true
										Possibilities [a][d][e] = false; // The corresponding value is false
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

bool ComparePossibilities (int Column1, int Row1, int Column2, int Row2)
{
	for (int a = 0; a < 9; a++)
	{
		if (!Possibilities [Column1][Row1][a] && Possibilities [Column2][Row2][a])
		{
			//cout << Column1+1 << ", " << Row1+1 << " and " << Column2+1 << ", " << Row2+1 << " don't match.\n";
			return false;
		}
		else if (Possibilities [Column1][Row1][a] && !Possibilities [Column2][Row2][a])
		{
			//cout << Column1+1 << ", " << Row1+1 << " and " << Column2+1 << ", " << Row2+1 << " don't match.\n";
			return false;
		}
	}
	//cout << Column1+1 << ", " << Row1+1 << " and " << Column2+1 << ", " << Row2+1 << " match.\n";
	return true;
}

void Elimination_E() // Guess and check
{
	int prev_store = blah::prev;

	for (int a = 0; a < 9; a++)
		for (int b = 0; b < 9; b++)
			Known_Store [a][b] = Known [a][b];
	
	for (int a = 0; a < 9; a++)
		for (int b = 0; b < 9; b++)
			for (int c = 0; c < 9; c++)
				Possibilities_Store [a][b][c] = Possibilities [a][b][c];
	
	for (int a = 0; a < 9; a++)
	{
		for (int b = 0; b < 9; b++)
		{
			for (int c = 0; c < 9; c++)
			{
				if (Possibilities [b][a][c])
				{
					Known [b][a] = c + 1;
					//Sleep (100);
					//printf ("Guess and check: %i, %i, %i...", b+1, a+1, c+1);

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
						if ((Find_Num_Known() - blah::prev) == 0)
						{
							//Display_Known();
							//cout << "Nope\n";
							break;
						}
						if (Find_Num_Known() >= 81)
						{	
							//Display_Known();
							bool Fine = true;
							//blah::Display_Contradictory = true;
							for (int a = 0; a < 9; a++)
								for (int b = 0; b < 9; b++)
									if (Check_Contradictory_Values (b, a, Known [b][a]))
										Fine = false;
							//blah::Display_Contradictory = false;
							if (Fine)
							{
								goto Done;
							}
							else
							{
								//cout << "Almost\n";
								Possibilities_Store [b][a][c] = false;
								Possibilities [b][a][c] = false;
								break;
							}
						}
						blah::prev = Find_Num_Known();
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
				blah::prev = prev_store;
			}
		}
	}
Done:
	blah::prev = prev_store;
	//int q;
	//cin >> q;
}

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

int main()
{
	Known_Reset();
	Possibilities_Reset();
	Reset_History();
	blah::prev = 0;
	blah::Auto = false;
	int Num_Sweeps = 2;

	while(1)
	{
		do
		{
			blah::prev = Find_Num_Known();
			for (int a = 0; a < Num_Sweeps; a++) 
			{
				Elimination_A();
				Elimination_B();
				Elimination_C();
				Elimination_D();
			}
			if (Find_Num_Known() == blah::prev && (Find_Num_Known() >= 17))
				Elimination_E();
		}
		while (Find_Num_Known() != blah::prev);
		
		if (blah::Auto)
			Output();
		blah::Auto = false;
		Input();
	}
}
