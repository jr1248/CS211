#include<stdio.h>
#include<stdlib.h>

int** allocate_intialize_matrix(int rows,int cols){
  int**value=malloc(rows*sizeof(int*));
  for(int i=0;i<rows;i++){
    value[i]=malloc(cols*sizeof(int));
  }
  for(int i=0;i<rows;i++){
    for(int j=0;j<cols;j++){
      value[i][j]=-1;
    }
  }
  return value;
}
int check_sub(int**matrix,int rows,int cols,int hexvalue){
  int temp=(rows/4)*4;
  int temp2=(cols/4)*4;
  int temp_row=temp+4;
  int temp_col=temp2+4;
  for(int i=temp;i<temp_row;i++){
    for(int j=temp2;j<temp_col;j++){
      if(matrix[i][j]==hexvalue){
	return 0;
      }
    }
  }
  return 1;
}
int is_valid(int**matrix,int rows,int cols, int  hexValue){
  for(int i=0;i<16;i++){
    if(matrix[rows][i]==hexValue){
      return 0;
    }
    if(matrix[i][cols]==hexValue){
      return 0;
    }
    if(check_sub(matrix,rows,cols,hexValue)==hexValue){
      return 0;
    }
  }
  return 1;
}
int solution_exist(int**matrix,int row, int col){
    for(int k=0;k<16;k++){
      if(!is_valid(matrix,row,col,k)){
        return 0;
      }
    }
  return 1;
}
int solve(int**matrix){
  //int count=0;
  int flag=0;
  for(int i=0;i<16;i++){
    for(int j=0;j<16;j++){
      if(matrix[i][j]==-1){
        flag=1;
      }
    }
  }
  if(flag==0){
    return 1;
  }
  for(int i=0;i<16;i++){
    for(int j=0;j<16;j++){
      if(matrix[i][j]==-1){
	for(int k=0;k<16;k++){
	  if(is_valid(matrix,i,j,k)){
	    matrix[i][j]=k;
	    if(solve(matrix)){
	      return 1;
	      matrix[i][j]=-1;
	    }
	  }
	}
      }
    }
  }
  return 0;
}
int main(int argc, char** argv){
  char c;
  FILE* fp=fopen(argv[1],"r");
  if (fp==NULL){
    return 0 ;
  }
  int rows=16;
  int cols=16;
  int** matrix=allocate_intialize_matrix(rows,cols);
  for (int i = 0 ; i < 16 ; i++){
    for (int j = 0 ; j < 16 ; j++){
      fscanf(fp,"%c\t",&c);
      if (c == '-'){
	matrix[i][j]=-1;

      }
      else if (c > 57) {
	matrix[i][j]=c-55;
      }
      else {
	matrix[i][j]=c-48;
      }
    }
  }
  for(int i=0;i<16;i++){
    for(int j=0;j<16;j++){
      solve(matrix);
    if(matrix[i][j]==-1){
      printf("%d\t",0 );
    }
    else{
      printf("%X\t", matrix[i][j] );
    }
    }
    printf("\n");
  }
  return 0 ;
}
