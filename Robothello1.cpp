/*
 * File:   Robothello.cpp
 * Author: Muhammed & Dipendra
 *
 * Created on June 9, 2014, 9:40 AM
 */

#include <cstdlib>
#include<iostream>
#include<sstream>
#include <exception>
#include<string>
#include <cctype>
using namespace std;

#include "Robothello.h"

#define INFINITY 100000

/*
 *
 */

int counter;

Robothello::Robothello() {
	for(int i=0; i<8;i++)
		for(int j=0; j<8; j++)
			squares[i][j] = 0;
	squares[3][3]=-1;
	squares[4][4]=-1;
	squares[3][4]=1;
	squares[4][3]=1;
        myPlayer=-1;
}

string Robothello::toString() {
	stringstream s;
	char cforvalplusone[] = {'W', '_', 'B'};
	s << "  1 2 3 4 5 6 7 8" << endl;
	for(int i=0; i<8;i++) {
		s << i+1 << '|';
		for(int j=0; j<8; j++)
			s << cforvalplusone[squares[i][j]+1] << '|';
		s << endl;
	}
	return s.str();
}

//returns if player with val has some valid move in this configuration
bool Robothello::has_valid_move(int val) {
	for(int i=0; i<8;i++)
		for(int j=0; j<8; j++)
			if(move_is_valid(i+1, j+1, val))
				return true;
	return false;
}

//r and c zero indexed here
//checks whether path in direction rinc, cinc results in flips for val
//will actually flip the pieces along path when doFlips is true
bool Robothello::check_or_flip_path(int r, int c, int rinc, int cinc, int val, bool doFlips) {
	int pathr = r + rinc;
	int pathc = c + cinc;
	if(pathr < 0 || pathr > 7 || pathc < 0 || pathc > 7 || squares[pathr][pathc]!=-1*val)
		return false;
	//check for some chip of val's along the path:
	while(true) {
		pathr += rinc;
		pathc += cinc;
		if(pathr < 0 || pathr > 7 || pathc < 0 || pathc > 7 || squares[pathr][pathc]==0)
			return false;
		if(squares[pathr][pathc]==val) {
			if(doFlips) {
				pathr=r+rinc;
				pathc=c+cinc;
				while(squares[pathr][pathc]!=val) {
					squares[pathr][pathc]=val;
					pathr += rinc;
					pathc += cinc;
				}
			}
			return true;
		}
	}
	return false;
}


//returns whether given move is valid in this configuration
bool Robothello::move_is_valid(int row, int col, int val) {
	int r = row-1;
	int c = col-1;
	if(r < 0 || r > 7 || c < 0 || c > 7)
		return false;
	//check whether space is occupied:
	if(squares[r][c]!=0)
		return false;
	//check that there is at least one path resulting in flips:
	for(int rinc = -1; rinc <= 1; rinc++)
		for(int cinc = -1; cinc <= 1; cinc++) {
			if(check_or_flip_path(r, c, rinc, cinc, val, false))
				return true;
		}
	return false;
}

//executes move if it is valid.  Returns false and does not update board otherwise
bool Robothello::play_square(int row, int col, int val) {
	if(!move_is_valid(row, col, val))
		return false;
	squares[row-1][col-1] = val;
	for(int rinc = -1; rinc <= 1; rinc++)
		for(int cinc = -1; cinc <= 1; cinc++) {
			check_or_flip_path(row-1, col-1, rinc, cinc, val, true);
		}
	return true;
}

//executes move if it is valid. Returns false and does not update board otherwise
//Makes computer make its move and returns the computer's move in row, and col
bool Robothello::play_square(int &row, int &col){
	static int count =0;
	count ++;
	if(row == 0 && col==0 && count==1)  myPlayer = 1;
	else play_square(row, col, -myPlayer);

        int **board, *action, flag=0;	//count is used to count the number of available squares

        board = new int*[8];
        for(int i=0; i<8;i++) {
           board[i] = new int[8];
        }

        //Store board board to a board array
        for(int i=0; i<8; i++){
	    for(int j=0; j<8; j++)
	    {
	         board[i][j]=get_square(i+1,j+1);
	         if((get_square(i+1,j+1)==0))
	         {
		    count++;
		     if(move_is_valid(i+1, j+1, myPlayer))
	             {
		         flag=1;  //if there is any valid move flag is set to 1
		     }
	          }
	     }
         }
         //Make move if board is not completely filled
         //flag=1 means board is not full and there is some valid move for computer
         if(flag)
         {
	      if(myPlayer==1){
	         action = alphabetaMaxDecision(board,  count);
              }
	      else {
	        action = alphabetaMinDecision(board, count);
              }

              //play the selected square
              play_square(action[0]+1, action[1]+1, myPlayer);
              row=action[0]+1;
              col=action[1]+1;
              cout<<"Robothello "<<myPlayer<<": "<<row<<" "<<col<<endl;
	          return true;
          }
          else {  //no possible move available, so computer passes
             row = -1;
	         col = -1;
             return false;
          }
}

bool Robothello::full_board() {
	for(int i=0; i<8;i++)
		for(int j=0; j<8; j++)
			if(squares[i][j]==0)
				return false;
	return true;
}

//returns score, positive for X player's advantage
int Robothello::score() {
	int sum =0;
	for(int i=0; i<8;i++)
		for(int j=0; j<8; j++)
			sum+=squares[i][j];
	return sum;
}

int Robothello::get_square(int row, int col) {
	return squares[row-1][col-1];
}

bool make_simple_cpu_move(Robothello * b, int cpuval) {
	for(int i=1; i<9;i++)
		for(int j=1; j<9; j++)
			if(b->get_square(i, j)==0)
				if(b->play_square(i, j, cpuval))
					return true;
	cout << "Computer passes." << endl;
	return false;
}

///*************functions created so that our simlar code for tic tac toe works without any significant modifications **********///////////////////////

//r and c zero indexed here
//checks whether path in direction rinc, cinc results in flips for val
//will actually flip the pieces along path when doFlips is true
bool checkflipPath(int r, int c, int rinc, int cinc, int val, bool doFlips, int **squares) {
	int pathr = r + rinc;
	int pathc = c + cinc;
	if(pathr < 0 || pathr > 7 || pathc < 0 || pathc > 7 || squares[pathr][pathc]!=-1*val)
		return false;
	//check for some chip of val's along the path:
	while(true) {
		pathr += rinc;
		pathc += cinc;
		if(pathr < 0 || pathr > 7 || pathc < 0 || pathc > 7 || squares[pathr][pathc]==0)
			return false;
		if(squares[pathr][pathc]==val) {
			if(doFlips) {
				pathr=r+rinc;
				pathc=c+cinc;
				while(squares[pathr][pathc]!=val) {
					squares[pathr][pathc]=val;
					pathr += rinc;
					pathc += cinc;
				}
			}
			return true;
		}
	}
	return false;
}


//returns whether given move is valid in this configuration
bool isMoveValid(int row, int col, int val, int **squares) {
	int r = row;
	int c = col;
	if(r < 0 || r > 7 || c < 0 || c > 7)
		return false;
	//check whether space is occupied:
	if(squares[r][c]!=0)
		return false;
	//check that there is at least one path resulting in flips:
	for(int rinc = -1; rinc <= 1; rinc++)
		for(int cinc = -1; cinc <= 1; cinc++) {
			if(checkflipPath(r, c, rinc, cinc, val, false, squares))
				return true;
		}
	return false;
}

//executes move if it is valid.  Returns false and does not update board otherwise
void playSquare(int row, int col, int val, int**squares) {
	//if(!isMoveValid(row, col, val, squares))
		//return false;
	squares[row][col] = val;
	for(int rinc = -1; rinc <= 1; rinc++)
		for(int cinc = -1; cinc <= 1; cinc++) {
			checkflipPath(row, col, rinc, cinc, val, true, squares);
		}
}

//******************************functions for evaluation, utility check and terminal checks*****************/////////////////////////

// Calculates the utility value based on the number of squares owned by the each players
int getUtility(int**board, int player)
{
     int playerSquares = 0, opponentSquares=0;
     for(int i=0; i<8; i++){
	for(int j=0; j<8; j++)
	{
	    if(board[i][j]== player){
                playerSquares++;
            }
	    else if(board[i][j]== -player){
                opponentSquares++;
            }
	}
     }

     if (playerSquares==opponentSquares){
         return 0;
     }
     else{
         return 10 * 100.00 * (((float)playerSquares - opponentSquares)/((float)playerSquares+ opponentSquares));
     }
}

// Check for final termination board
//all slots of the board squares taken
bool isTerminal(int** board)
{
    bool flag = true;
    for(int i=0; i<8; i++){
       for(int j=0; j<8; j++)
       {
          if(board[i][j] == 0) {
              flag = false;
              break;
          }
       }
       if(!flag){
           break;
       }
    }
    return flag;
}

//calculates the evaluation value for a particular board state
//the equation for the evaluation function is given in the homework pdf
float evaluation(int** board, int player, int count)
{
    //variables for each heuristic
    int valid = 0, square = 0 , corner = 0, cc = 0, stable=0, opSquare = 0, opValid = 0, opCorner = 0, opCC=0, opStable=0;
    float coinParity=0, playerMobility=0, cornerValues=0, closeCorner = 0, stabilityValue = 0;
    float eval =0;
    int **stability, **temp;
    //computing values
    for(int i=0; i<8; i++) {
        for(int j=0; j<8; j++) {
            if((board[i][j]==0) &&  isMoveValid(i, j, player, board)) valid++; //number of valid moves
            if((board[i][j]==0) &&  isMoveValid(i, j, -player, board))opValid++; //oppobnent's valid moves
            if(board[i][j]== player) square++;  //number of squares/coins
        }
    }

    if(board[0][0] == player) corner++; //computing the number of corners
    if(board[0][7] == player) corner++;
    if(board[7][0] == player) corner++;
    if(board[7][7] == player) corner++;

    if(board[0][0] == -player) opCorner++; //same for opposition
    if(board[0][7] == -player) opCorner++;
    if(board[7][0] == -player) opCorner++;
    if(board[7][7] == -player) opCorner++;

    if(board[0][0] == 0)   {   //this portion computes the number of close to corners coin (which is not always good)
            if(board[0][1] == player) opCC++; //(if player, then oppositions counter is increased)
            else if(board[0][1] == -player) cc++; // vice versa, as this is a negative heuristic
            if(board[1][1] == player) opCC++;
            else if(board[1][1] == -player) cc++;
            if(board[1][0] == player) opCC++;
            else if(board[1][0] == -player) cc++;
    }
    if(board[0][7] == 0)   {
            if(board[0][6] == player) opCC++;
            else if(board[0][6] == -player) cc++;
            if(board[1][6] == player) opCC++;
            else if(board[1][6] == -player) cc++;
            if(board[1][7] == player) opCC++;
            else if(board[1][7] == -player) cc++;
    }
    if(board[7][0] == 0)   {
            if(board[7][1] == player) opCC++;
            else if(board[7][1] == -player) cc++;
            if(board[6][1] == player) opCC++;
            else if(board[6][1] == -player) cc++;
            if(board[6][0] == player) opCC++;
            else if(board[6][0] == -player) cc++;
    }
    if(board[7][7] == 0)   {
            if(board[6][7] == player) opCC++;
            else if(board[6][7] == -player) cc++;
            if(board[6][6] == player) opCC++;
            else if(board[6][6] == -player) cc++;
            if(board[7][6] == player) opCC++;
            else if(board[7][6] == -player) cc++;
    }

    stability = new int*[8]; //this is used to compute stability (number of unstable coins, thats also a negative heuristic)
    for(int i=0; i<8;i++) {
        stability[i] = new int[8];
    }
    //at first, assign all squares of player -1 stability
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
        	if(board[i][j]==player){
        	   stability[i][j] = 0;
        	}
        	else {
        		stability[i][j] = 0;
        	}
        }
    }

    temp = new int*[8];
    for(int i=0; i<8;i++) {
        temp[i] = new int[8];
    }
    //at first, assign all squares of player +1 stability
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
           temp[i][j] = board[i][j];
        }
    }

    //set un-stable squares values
    for(int i=0; i<8; i++) {
        for(int j=0; j<8; j++) {
            if(board[i][j] ==0 && isMoveValid(i, j, -player, board)){
            	playSquare(i, j, -player, board);
            	for(int k=0; k<8; k++) {
                   for(int l=0; l<8; l++) {
                      if(temp[k][l]==player && board[k][l]!=player){
                          stability[k][l]=-1;  //if after making he next move, the coins can be flipped, hen -1 is given, otherwise 0
                      }
                   }
                }
            	for(int k=0; k<8; k++) {
                   for(int l=0; l<8; l++) {
                      board[k][l] = temp[k][l];
                   }
                }
            }
        }
    }

    for(int i=0; i<8; i++) {
        for(int j=0; j<8; j++) {
             stable+=stability[i][j];
        }
    }

    //opponent stabilitiy
    //at first, assign all squares of player -1 stability
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
        	if(board[i][j]==-player){
        	   stability[i][j] = 0;
        	}
        	else {
        		stability[i][j] = 0;
        	}
        }
    }

    //set un-stable squares values
    for(int i=0; i<8; i++) {
        for(int j=0; j<8; j++) {
            if(board[i][j] ==0 && isMoveValid(i, j, player, board)){
            	playSquare(i, j, player, board);
            	for(int k=0; k<8; k++) {
                   for(int l=0; l<8; l++) {
                      if(temp[k][l]==-player && board[k][l]!=-player){
                          stability[k][l]=-1;   /////////same mechanism for opponents
                      }
                   }
                }
            	for(int k=0; k<8; k++) {
                   for(int l=0; l<8; l++) {
                      board[k][l] = temp[k][l];
                   }
                }
            }
        }
    }

    for(int i=0; i<8; i++) {
        for(int j=0; j<8; j++) {
             opStable+=stability[i][j];
        }
    }
    
    opSquare = 64 - (count+square);
    coinParity = 100.00 * (((float)square - opSquare)/((float)square+opSquare)); //we compute the (difference/sum) metric, coin-parity
    
    if((valid+opValid)!=0){
    	playerMobility = 100.00 * (((float)valid - opValid)/((float)valid + opValid)); //computing player mobility heuristic from previous variables
    }

    if((corner+opCorner)!=0){
    	cornerValues = 100.00 * (((float)corner - opCorner)/((float)corner + opCorner));  //corner heuristic
    }

    if((cc+opCC)!=0){
    	closeCorner = 100.00 * (((float)cc - opCC)/((float)cc + opCC));  //corner close heuristic
    }

    if((stable+opStable)!=0){
    	stabilityValue = 100.00 * (((float)opStable - stable)/((float)stable + opStable)); //stability heuristic
    }

    if(count >= 54){ //heuristics are weighted differently intially
    	eval = coinParity * 0.5 + playerMobility * 4.0 + cornerValues * 1.5 + 1.5 * closeCorner + 2.5 * stabilityValue;
    }
    else if(count>16 && count <54){ //if count<=16, it will not come here
    	eval = coinParity * 2.5 + playerMobility * 1.0 + cornerValues * 3.5 + 1.5 * stabilityValue + 1.5 * closeCorner;
    }
    if (player==1) return eval;
    else return -eval;
}

//It recursively finds the maximum from all minValue for all non-filled cells
int maxValue(int** board, int a, int b, int player, int count)
{
    int **temp;
    temp = new int*[8];
    for(int i=0; i<8;i++) {
        temp[i] = new int[8];
    }

    //copy the current board state to temp
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
           temp[i][j] = board[i][j];
        }
    }
    //initializing values
    int max= -INFINITY, minval;
    int static counter = 0;
    counter++;

    //checks whether the board is full or game has a winner
    //checks whether the board is full or game has a winner
    if (isTerminal(board) ) {
       return getUtility(board, player);
    }
    else if ( counter> 100000 && count>16)
    {
		 return evaluation(board, player, count);
    }
    //main pruning is done here
    for(int i=0; i<8; i++) {
        for(int j=0; j<8; j++) {
            if(board[i][j] ==0 && isMoveValid(i, j, player, board))
            {
                playSquare(i, j, player, board);
                minval= minValue(board, a, b, -player, count);
                if(max<minval)
                {
                    max = minval;
                }

                for(int k=0; k<8; k++) {
                   for(int l=0; l<8; l++) {
                      board[k][l] = temp[k][l];
                   }
                }
                if(max>=b) {
                    return max;
                }
                if(a<max) {
                    a=max;
                }
            }
        }
    }
    return max;
}

// It returns the maximizing move for cpu-move using all values returned by minValue for all the non-filled slots based on alpha-beta pruning
//regarding values, evaluation function is used
int *alphabetaMaxDecision(int** board,  int count)
{
    int **temp;
    temp = new int*[8];
    for(int i=0; i<8;i++) {
        temp[i] = new int[8];
    }

    //copy the current board state to temp
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
           temp[i][j] = board[i][j];
        }
    }

    int *action = new int[2];
    action[0]=action[1]=-1;
    int max = -INFINITY, minval, a=-INFINITY, b=INFINITY;
    counter=0;
    int player = 1;
    //choosing the right action using this loop
    for(int i=0; i<8; i++) {
        for(int j=0; j<8; j++) {
           if(board[i][j] ==0 && isMoveValid(i, j, player, board)) //if the square is empty and it is valid for the player
           {
               playSquare(i, j, player, board);
               minval = minValue(board, a, b , -player, count);
               for(int k=0; k<8; k++) {
                   for(int l=0; l<8; l++) {
                       board[k][l] = temp[k][l];
                   }
               }
               if(max<minval) {
                  max = minval;
                  action[0] = i;
                  action[1] = j;
               }
           }
        }
    }
    return action;
}

// It recursively finds the minimum from all maxValue for all non filled cells
int minValue(int** board, int a, int b, int player, int count)
{
    int **temp;
    temp = new int*[8];
    for(int i=0; i<8;i++) {
        temp[i] = new int[8];
    }

    //copy the current board state to temp
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
           temp[i][j] = board[i][j];
        }
    }

    //initializing values
    int min = INFINITY, maxval;
    int static counter = 0;
    counter++;

    //checks whether the board is full or game has a winner
    //checks whether the board is full or game has a winner
    if (isTerminal(board) ) {
       return getUtility(board, player);
    }
    else if ( counter> 100000 && count>16)
    {
		 return evaluation(board, player, count);
    }

    //main pruning is done here
    for(int i=0; i<8; i++) {
       for(int j=0; j<8; j++) {
           if(board[i][j] ==0 &&  isMoveValid(i, j, player, board))
           {
               playSquare(i, j, player, board);
               maxval = maxValue(board, a, b, -player, count);
               if(min>maxval) {
                   min = maxval;
               }
               for(int k=0; k<8; k++) {
                   for(int l=0; l<8; l++) {
                        board[k][l] = temp[k][l];
                   }
               }

               if(min<=a){
                   return min;  //pruning
               }
	       if(b>min){
                   b=min;
               }
            }
       }
    }
    return min;
}


// It returns the maximizing move for cpu-move using all values returned by minValue for all the non-filled slots based on alpha-beta pruning
//regarding values, evaluation function is used
int *alphabetaMinDecision(int** board, int count)
{
    int **temp;
    temp = new int*[8];
    for(int i=0; i<8;i++) {
        temp[i] = new int[8];
    }

    //copy the current board state to temp
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
           temp[i][j] = board[i][j];
        }
    }

    int *action = new int[2];
    action[0]=action[1]=-1;
    int min = INFINITY, maxval, a = -INFINITY, b = INFINITY;
    counter = 0;
    int player = -1;
    //choosing the right action using this loop
    for(int i=0; i<8; i++) {
        for(int j=0; j<8; j++) {
           if (board[i][j]==0&&  isMoveValid(i, j, player, board))   //if the square is empty and it is valid for the player
           {
               playSquare(i, j, player, board);
               maxval = maxValue(board, a, b, -player , count);

               for(int k=0; k<8; k++){
                   for(int l=0; l<8; l++) {
                      board[k][l] = temp[k][l];
                   }
               }
               if(min>maxval) {
                   min = maxval;
                   action[0] = i;
                   action[1] = j;
               }
            }
        }
    }
    return action;
}

// CPU Move using MinMax algorithm + alpha-beta pruning + carefully thought evaluation function
bool make_robothello_move(Robothello * b, int cpuval) {
    int **board, *action, flag=0, count = 0;	//count is used to count the number of available squares

    board = new int*[8];
    for(int i=0; i<8;i++) {
        board[i] = new int[8];
    }

    //Store board board to a board array
    for(int i=0; i<8; i++){
	for(int j=0; j<8; j++)
	{
	     board[i][j]=b->get_square(i+1,j+1);
	     if((b->get_square(i+1,j+1)==0))
	     {
		count++;
		if(b->move_is_valid(i+1, j+1, cpuval))
	        {
		     flag=1;  //if there is any valid move flag is set to 1
		}
	     }
	  }
    }
    //Make move if board is not completely filled
    //flag=1 means board is not full and there is some valid move for computer
    if(flag)
    {
	if(cpuval==1){
	   action = alphabetaMaxDecision(board,  count);
        }
	else {
	   action = alphabetaMinDecision(board, count);
        }

        //play the selected square
        b->play_square(action[0]+1, action[1]+1, cpuval);
	return true;
    }
    else {  //no possible move available, so computer passes
        cout << "Computer passes." << endl;
        return false;
    }
}

//the main function which executes the play
void play() {
    Robothello * b = new Robothello();
    char input;
    int humanPlayer = 1;
    int cpuPlayer = -1;

    cout << b->toString();

    //prompt user to select first or second play
    cout << "Do you want to play first? (Press 'y' for yes, 'n' or any other key for no)"<<endl;
    cin >> input;

    //if user is second, robothello should make the first move
    if(input!='y')
    {
	humanPlayer = -1;
	cpuPlayer = 1;
	make_robothello_move(b, cpuPlayer);
        cout << b->toString();
    }

    int consecutivePasses = 0;
    while(!b->full_board() && consecutivePasses<2) {
	int row, col;
	//check if player must pass:
	if(!b->has_valid_move(humanPlayer)) {
	    cout << "You must pass." << endl;
	    consecutivePasses++;
	}
	else {
            consecutivePasses = 0;
	    cout << "Your move row (1-8): ";
	    cin >> row;
	    if(cin.fail()){
                 std::cerr<< "Illegal move."<<endl;
                 cin.clear();
                 cin.ignore();
                 continue;
            }
	    cout << "Your move col (1-8): ";
	    cin >> col;
	    if(cin.fail()){
               std::cerr<< "Illegal move."<<endl;
               cin.clear();
               cin.ignore();
               continue;
            }
	    if(!b->play_square(row, col, humanPlayer)) {
               cout << "Illegal move." << endl;
	       continue;
            }
	}
        //move for computer:
        if(b->full_board())
	    break;
	else {
	     cout << b->toString() << "..." << endl;
             
/*
///////////////////////// BEGIN: Code to print appropriate evaluation function/////////////////////////////////////////////////////////////////////////  
                int c=0;
   for(int i=0; i<8; i++){
      for(int j=0; j<8; j++) {
          if((b->get_square(i+1,j+1)==0))
	  {
              c++;
          }    
      }
   }
   
    int **temp;
    temp = new int*[8];
    for(int i=0; i<8;i++) {
        temp[i] = new int[8];
    }

    //copy the current board state to temp
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
           temp[i][j] = b->get_square(i+1,j+1);
        }
    }

        cout<<"EvalB: " << evaluation(temp, humanPlayer, c) << endl;
        cout<<"EvalW: " << -evaluation(temp, cpuPlayer, c) << endl; 
        
 ///////////////////////////////////////////// END: VCode to print evaluation function //////////////////////////////////////////////////////////////////////////////////////////

*/ 
	     if(make_robothello_move(b, cpuPlayer))
		consecutivePasses=0;
	     else
		consecutivePasses++;
			cout << b->toString();
	}
   }
   cout << b->toString();
   
   int score = b->score();

   //if human plays second, we make the relative score negative
   if(humanPlayer == -1) score = -score;

   if(score==0)
       cout << "Tie game." << endl;
   else if(score>0)
       cout << "Human wins by " << score << endl;
   else if(score<0)
       cout << "Computer wins by " << -score << endl;

   char a;
   cin >> a;
}
/*
int main(int argc, char * argv[])
{
	play();
	return 0;
}

*/