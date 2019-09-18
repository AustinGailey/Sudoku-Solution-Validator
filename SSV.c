/*

Author: Austin Gailey

Written for CS3600 with Professor Zhu HW03


  #############################################
  #                                           #
  #         SUDOKU  SOLUTION  VALIDATOR       #
  #                                           #
  #############################################


*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef enum { false,true } Bool;
typedef struct {
   int topRow;
   int bottomRow;
   int leftColumn;
   int rightColumn;
} zone;

int  sudokuPuzzle[9][9];
Bool  colVal[9];
Bool  rowVal[9];
Bool gridVal[9];

zone columns[9];
zone    rows[9];
zone   grids[9];

pthread_t tid_columns[9];
pthread_t    tid_rows[9];
pthread_t   tid_grids[9];

main() {
   FILE *puzzle = fopen("SudokuPuzzle.txt", "r");
   readPuzzle(puzzle);
   printPuzzle();
   zoneColumns();
   zoneRows();
   zoneGrids();
}// End Main


readPuzzle(FILE *puzzle){
  int i; 
  char inputStr[100];
  for(i=0;i<9;i++){
      fgets(inputStr, 100,puzzle);
      fillPuzzleArray(inputStr,i);
   }
}


fillPuzzleArray(char inputStr[100],int i){
   int j;
   int offset = 0;
   int len = strlen(inputStr);
   for(j=0;j<len;j++){
      if(inputStr[j]-48 != -39 && inputStr[j]-48 != -38){
         sudokuPuzzle[i][j-offset] = inputStr[j]-48;
	// printf("Value Read was: %d i=%d j=%d\n", inputStr[j]-48,i,j);
      }else{
         offset++;
      }
   }
}


printPuzzle(){
   int row, col;
   printf("The Sudoku Puzzle Was Input:\n\n");
   for(row=0;row<9;row++){
      for(col=0;col<9;col++) {
         printf("%d\t",sudokuPuzzle[row][col]);
      }
      printf("\n");
   } 
   printf("\nEnd of Puzzle\n");
}


zone createZone(int br, int tr, int lc, int rc){
   zone z;
   z.bottomRow  = br;
   z.topRow     = tr;
   z.leftColumn = lc;
   z.rightColumn= rc;
   return z;
}

waitForChildThreads(pthread_t childIds[]){
   int i;
   for(i=0;i<9;i++) {
    pthread_join(childIds[i], NULL);
   }
}

Bool toggletf(Bool b) {
   if(b == true){
      return false;
   }
   if(b == false){
      return true;
   }
}

validateZone(zone z, pthread_t tid[],int i, Bool zoneVal[]){
   int c,r,j;
   int rowStart,rowEnd,colStart,colEnd;
   rowStart = z.bottomRow;
   rowEnd   = z.topRow;
   colStart = z.leftColumn;
   colEnd   = z.rightColumn;
   Bool v[9];
   for(j=0;j<9;j++){ //Initialize array
    v[j] = false;
   }
   for(r = rowStart; r <= rowEnd;r++){
      for(c = z.leftColumn; c <= z.rightColumn;c++) {
         int index = (sudokuPuzzle[r][c])-1;
         v[index] = toggletf(v[index]);
      }
   }
   if(v[0] == true && v[1] == true && v[2] == true &&
      v[3] == true && v[4] == true && v[5] == true &&
      v[6] == true && v[7] == true && v[8] == true) {
      zoneVal[i] = true;
      printf("%x TRow: %d, BRow: %d, LCol: %d, RCol: %d valid!\n",
            (unsigned int)tid[i],z.topRow+1,z.bottomRow+1,
             z.leftColumn+1, z.rightColumn)+1;
   } else {
      zoneVal[i] = false;
      printf("%x TRow: %d, BRow: %d, LCol: %d, RCol: %d invalid!\n",
            (unsigned int)tid[i],z.topRow+1,z.bottomRow+1,
             z.leftColumn+1, z.rightColumn+1);
   }
}


void * valCol(void  *_i) {
   int i = *((int *) _i);
   zone z = columns[i];
   validateZone(z,tid_columns,i,colVal);
   pthread_exit (NULL);
}


zoneColumns(){
   int i;
   for(i=0;i<9;i++) {
      columns[i] = createZone(0,8,i,i);
      int *arg = malloc(sizeof(*arg));
      *arg = i;
      pthread_create(&tid_columns[i], NULL, valCol, arg);
   }
   waitForChildThreads(tid_columns);
}


void * valRow(void *_i) {
   int i = *((int *) _i);
   zone z = rows[i];
   validateZone(z,tid_rows,i,rowVal);
   pthread_exit (NULL);
}


zoneRows(){
   int i;
   for(i=0;i<9;i++) {
      rows[i] = createZone(i,i,0,8);
      int *arg = malloc(sizeof(*arg));
      *arg = i;
      pthread_create(&tid_rows[i], NULL, valRow, arg);
   }
   waitForChildThreads(tid_rows);
}


void * valGrid(void *_i) {
   int i = *((int *) _i);
   zone z = grids[i];
   validateZone(z, tid_grids,i,gridVal);
   pthread_exit (NULL);
}


zoneGrids(){
   int i;
   grids[0] = createZone(0,2,0,2);//Manually entered to complete assignment on time
   grids[1] = createZone(0,2,3,5);
   grids[2] = createZone(0,2,6,8);
   grids[3] = createZone(3,5,0,2);
   grids[4] = createZone(3,5,3,5);
   grids[5] = createZone(3,5,6,8);
   grids[6] = createZone(6,8,0,2);
   grids[7] = createZone(6,8,3,5);
   grids[8] = createZone(6,8,6,8);
   for(i=0;i<9;i++) {
      int *arg = malloc(sizeof(*arg));
      *arg = i;
      pthread_create(&tid_grids[i], NULL, valGrid, arg);
   }
   waitForChildThreads(tid_grids);
}

/************  END of Sudoku Solution Validator  ****************/
