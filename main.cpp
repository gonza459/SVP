/* Instructions:
      -Sudoko validator Program (SVP): validate a 9x9 completed puzzle
      completed
          *check for correctness
      -SVP accept as input an ASCII.txt file with elements seperated
      by commans and structured as a 9x9 table
      -SVP specify via output to user where occured
          *eg. row 3 column 5
      -SVP detects error then specify what the correct solution is for
      the particular cell
      -SVP should provide user feedback to the GUI or CLI to explain error
      and final answer

      --> Must have at least 3 threads, max 10 threads not-including main
      --> Final answer must be provided by main thread
      --> Explicit threads must be used

    Program run:
      -The program first runs through the main thread where the helper threads
      are initiated and run
      -A grid is created from the input txt file which will be used to provide
      the data to pass as parameters to check through each of the threads
      -A POSIX thread is initiated to define the total number of threads (9)
      after the grid is created.
      -The parameters for rows and columns are passed into data sets and the
      helper threads are run one at a time - subgrid, rowcheck, and columncheck
      -9 threads are run at a time to check all rows and columns at once to
      compare values and whether they are correct
      -Processes are completed at this time to compare all values and return
      any duplicates recognized and returns the row and column in which the
      duplicates were found
      -Once all tasks in each thread is complete, a message of whether there is
      an error or not and where is printed on the User interface
*/

#include <iostream>
#include <fstream>
#include <thread>
#include <pthread.h>
#include <array>
#define T_NUM 9

using namespace std;

int grid[9][9];
int validArray[9];
int num;
bool success = false;

//stores information of row num and column num to be passed
typedef struct{
  int row;
  int col;

}rowXcol;

//creates 9x9 grid from input file
void sudokuGrid(int grid[9][9]){

  ifstream genFile;
  string line;
  string filename;

//Prompts and opens file
  do{
    cout << "Enter a file to input: " << endl;
    cin>>filename;
    genFile.open(filename.c_str());
    if(!genFile){
      cerr<<"Unable to open file\n";
    }
  }
  while(!genFile);

  int rowCount = 0;
  int colCount = 0;

//Once open, the file is read through to input the nums into the grid
if(genFile.is_open()){
  while(getline (genFile, line)){
    for(int i=0; i<= line.size(); i++){
      if (line[i] != ','){
        //cout << line [i] << endl;
        grid[rowCount][colCount] = line[i];
        //cout << grid[rowCount][colCount];
        colCount++;
      }
    }
    rowCount++;
    colCount =0;
    }
  }
}

//checks if a number is duplicated in a given row or column when called
/*int Duplicate (int y[9]) {
  int num;
  for (int i=0; i<9; i++) {
    num = y[i];
    for (int j=i+1; j<9; j++) {
      if (y[j] ==  num)
	      return 0;
    }
  }
  return 1;
}
*/


void *RowCheck(void* row_col){
  //passes parameters of row and column set into variables
  rowXcol *params = (rowXcol*) row_col;
	int r = params->row;
	int c = params->col;

//Counts through all rows
  int rowCounter = 0;

//Array initiates all values to 0
/*The array changes each 0 value in its set to 1 as it
  encounters each number.
*/
  int validArray[9] = {0};
    for(int i=0; i<9; i++){
      num = grid[r][i];
      rowCounter++; //increases as each row is checked

        //if num is already 1 in validArray, then the num is repeated in the row
      if ( validArray[num] == 1){
        cout << "Row check: There is a duplicate found at row " << r+1 << " and column " << i+1<< endl;
        success = false;
        pthread_exit(NULL);
      }else{
        validArray[num] = 1; // sets num to 1 when present in the row
        success = true;
      }
    }
  // if reached this point, all rows have been checked and are valid
  rowCounter = 9;
  success = true;
  pthread_exit(NULL);
}

void *ColCheck(void* row_col){
  //passes parameters of row and column set into variables
  rowXcol *params = (rowXcol*) row_col;
	int r = params->row;
	int c = params->col;

//Counts through all coulumns
  int colCounter = 0;

//Array initiates all values at 0
  int validArray[9] = {0};
  for(int i=0; i<9; i++){
    num = grid[i][c];
    colCounter++; //increases as each column is checked

      //if num is already 1 in validArray, then the num is repeated in the column
      if ( validArray[num] == 1 ){
        cout << "Column check: There is a duplicate found at row " << i+1 << " and column " << c+1 << endl;
        success = false;
        pthread_exit(NULL);
      }else{
        validArray[num] = 1; // sets num to 1 when present in the column
        success = true;
      }
    }
  // if reached this point, all columns have been checked and are valid
  colCounter = 9;
  success = true;
  pthread_exit(NULL);
}


//Checks the 3x3 subgrids for any repetition of numbers
void *subGridCheck(void* row_col){
  //passes parameters of row and column set into variables
  rowXcol *params = (rowXcol*) row_col;
	int r = params->row;
	int c = params->col;

  int subGridCounter = 0;
  int validArray[9] = {0};

  for(int i= r; i < r+3; i++){ //check in segments of multiples of 3
    for(int j= c; j < c+3; j++){
      num = grid[i][j];
      subGridCounter++;
            if (validArray[num] == 1){
              cout << "SubGrid check: There is a duplicate found at row " << i+1 << " and column " << j+1 << endl;
              success = false;
              pthread_exit(NULL);
            }else{
              validArray[num] = 1;
              success = true;
          }
        }
      }
  // if reached this point, all subgrids have been checked and are valid
  subGridCounter = 9;
  success = true;
  pthread_exit(NULL);
  }

string errorMessage(int row, int col){
  string message;
  message = cout << "There is a duplicate found at " << row << " and " << col<< endl;
  return message;
}

int main(int argc, char* argv[]){

  sudokuGrid(grid);
/*
    for(int k = 0; k < 9; k++){
      for (int l=0; l < 9; l++){
        cout << grid[k][l];
      }
      cout << endl;
    }
*/

  pthread_t threads[T_NUM];

  int threadCount = 0;
  for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
      //subgrid threads created
			if (i%3 == 0 && j%3 == 0) { //checks rows and columns by multiples of 3
				rowXcol *data = (rowXcol *) malloc(sizeof(rowXcol));
				data->row = i;
				data->col = j;
				pthread_create(&threads[threadCount++], NULL, subGridCheck, data);
			}
      //column threads created
			if (i == 0) {
				rowXcol *columnData = (rowXcol *) malloc(sizeof(rowXcol));
				columnData->row = i;
				columnData->col = j;
				pthread_create(&threads[threadCount++], NULL, ColCheck, columnData);
    	}
       //Row threads creted
			if (j == 0) {
				rowXcol *rowData = (rowXcol *) malloc(sizeof(rowXcol));
				rowData->row = i;
				rowData->col = j;
				pthread_create(&threads[threadCount++], NULL, RowCheck, rowData);
			}
	}
}
for (int i = 0; i < threadCount; i++) {
      pthread_join(threads[i], NULL);
  }

//Prints out message after all threads have finished running
	for (int i = 0; i < 9; i++) {
		if (success == false || validArray[i] == 0) {
			cout << "Oh No! You're not quite right yet :( \n";
			return 0;

      }
    if (success == true){
      cout << "Good Job! The solution is correct! \n";
      return 0;
    }
  }

}
