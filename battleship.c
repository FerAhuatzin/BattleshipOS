#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdlib.h>
#define N 10 

//create boards
int boardA[N][N] = {0};
int boardB[N][N] = {0};
int XcoordenatesBoatsBoardA[17];
int XcoordenatesBoatsBoardB[17];
int YcoordenatesBoatsBoardA[17];
int YcoordenatesBoatsBoardB[17];
int hitsA = 0;
int hitsB = 0;
int win = 0;
pthread_mutex_t mutex; 

void alrmHandler (int signum) {
	
} //end alrmHandler

int getCoordenateIndex (int placedBoats) {
	if (placedBoats==0) {
		return 0;
	} //end if
	if (placedBoats==1) {
		return 2;
	} //end if
	if (placedBoats==2) {
		return 5;
	} //end if
	if (placedBoats==3) {
		return 8;
	} //end if
	if (placedBoats==4) {
		return 12;
	} //end if
} //end getCoordenateIndex

void printBoard (int turn) {
	if (turn==0) {
	//show shots at board B
		printf("	");
		for (int i=-1; i<N; i++) {
			for (int j=-1;j<N;j++) {
				if (i==-1) {
					if (j>=0) {
						printf("%d	",j+1);
					} //end if
				} //end if
				else
				if (j==-1) {
					if (i>=0) {
						printf("%d	",i+1);
					} //end if
				} //end if else
				else {
					if (boardB[i][j]==-1) {
						printf("%s	",".");
					}//end if
					if (boardB[i][j]==0) {
						printf("%s	"," ");
					}//end if
					if (boardB[i][j]==1) {
						printf("%s	","x");
					}//end if
				} //end else
			} //end for
			printf("\n");
		} //end for
	} //end if
	if (turn==1) {
		printf("	");
		for (int i=-1; i<N; i++) {
			for (int j=-1;j<N;j++) {
				if (i==-1) {
					if (j>=0) {
						printf("%d	",j+1);
					} //end if
				} //end if
				else
				if (j==-1) {
					if (i>=0) {
						printf("%d	",i+1);
					} //end if
				} //end if else
				else {
					if (boardA[i][j]==-1) {
						printf("%s	",".");
					}//end if
					if (boardA[i][j]==0) {
						printf("%s	"," ");
					}//end if
					if (boardA[i][j]==1) {
						printf("%s	","x");
					}//end if
				} //end else
			} //end for
			printf("\n");
		} //end for
	//show shots at board A
	} //end if
} //end printBoard

int writeCoordenate (int squares, int coordenateX, int coordenateY, int horizontal,int placedBoats, int board, int fd) {
	int n;
	int coordenateIndex = getCoordenateIndex(placedBoats);
	
	if (board==0) {
		for (int i = 0; i<squares; i++) {
			n = write(fd, &coordenateX, sizeof(int));
			n = write(fd, &coordenateY, sizeof(int));
			XcoordenatesBoatsBoardA[coordenateIndex] = coordenateX;
			YcoordenatesBoatsBoardA[coordenateIndex] = coordenateY;
			boardA[coordenateX][coordenateY]=1;
			if (horizontal) {
				coordenateY++;
			} //end if
			else {
				coordenateX++;
			} //end else
			coordenateIndex++;
		} //end for
	} //end if
	else {
		for (int i = 0; i<squares; i++) {
			n = write(fd, &coordenateX, sizeof(int));
			n = write(fd, &coordenateY, sizeof(int));
			XcoordenatesBoatsBoardB[coordenateIndex] = coordenateX;
			YcoordenatesBoatsBoardB[coordenateIndex] = coordenateY;
			boardA[coordenateX][coordenateY]=1;
			if (horizontal) {
				coordenateY++;
			} //end if
			else {
				coordenateX++;
			} //end else
			coordenateIndex++;
		} //end for
	} //end else
} //end writeCoordenate 

int checkRepeatedCoordenate (int squares, int coordenateX, int coordenateY, int horizontal, int placedBoats, int board) {
	int coordenateIndex = getCoordenateIndex(placedBoats);
	
	if (board == 0) {
		for (int i = 0; i<squares; i++) {
			for (int j = 0; j<coordenateIndex; j++) {
				if (horizontal) {
					if ((XcoordenatesBoatsBoardA[j]==(coordenateX))&&(YcoordenatesBoatsBoardA[j]==(coordenateY+i))) {
						return 0;
					} //end if
				} //end if
				else {
					if ((XcoordenatesBoatsBoardA[j]==(coordenateX+i))&&(YcoordenatesBoatsBoardA[j]==(coordenateY))) {
						return 0;
					} //end if
				} //end else
			} //end for
		} //end for
	} //end if
	else {
		for (int i = 0; i<squares; i++) {
			for (int j = 0; j<coordenateIndex; j++) {
				if (horizontal) {
					if ((XcoordenatesBoatsBoardB[j]==(coordenateX))&&(YcoordenatesBoatsBoardB[j]==(coordenateY+i))) {
						return 0;
					} //end if
				} //end if
				else {
					if ((XcoordenatesBoatsBoardB[j]==(coordenateX+i))&&(YcoordenatesBoatsBoardB[j]==(coordenateY))) {
						return 0;
					} //end if
				} //end else
			} //end for
		} //end for
	} //end else
	return 1;
} //end checkRepeatedCoodenate

int confirmPosition (int squares, int coordenateX, int coordenateY, int horizontal, int placedBoats, int board, int fd) {
	int notRepeatedCoordenate = checkRepeatedCoordenate(squares,coordenateX,coordenateY,horizontal,placedBoats,board);
	
	if (horizontal) {
		if ((coordenateY+squares)<=N && coordenateX<=10 && coordenateY>=0 && coordenateX>=0 && notRepeatedCoordenate) {
			writeCoordenate(squares, coordenateX,coordenateY,horizontal,placedBoats,board,fd);
			printf("We have confirmed your coordenates from the coordenate %d,%d to the coordenate %d,%d\n", coordenateX+1, coordenateY+1, coordenateX+1, coordenateY+squares);
			alarm(3);
			pause();
			return 1;
		} //end if
		else {
			alarm(1);
			pause();
			printf ("The coordenates you selected are invalid, try other ones...\n");
			return 0;
		} //end else
	} //end if
	else {
		if ((coordenateX+squares)<=N && coordenateY<=10 && coordenateX>=0 && coordenateY>=0 && notRepeatedCoordenate) {
			writeCoordenate(squares, coordenateX,coordenateY,horizontal,placedBoats,board,fd);
			printf("We have confirmed your coordenates from the coordenate %d,%d to the coordenate %d,%d\n", coordenateX+1, coordenateY+1, coordenateX+squares, coordenateY+1);
			alarm(3);
			pause();
			return 1;
		} //end if
		else {
			alarm(1);
			pause();
			printf ("The coordenates you selected are invalid, try other ones...\n");
			return 0;
		} //end else
	} //end else
} //end checkPosition

void askPosition (int squares, int placedBoats, int board, int fd) {
	int coordenateX = 0;
	int coordenateY = 0;
	int horizontally = -1;
	int confirmedPosition = 0;
	
	while (horizontally!=0 && horizontally!=1) {
		printf("This boat occupies %d squares...you want it to place it horizontally or vertically?\nHorizontally type 1\nVertically type 0\n", squares);
		alarm(2);
		pause();
		scanf("%d", &horizontally);
		if (horizontally!=0 && horizontally!=1) {
			alarm(1);
			pause();
			printf("Type a valid input...\n");
			alarm(1);
			pause();
		} //end if
	} //end while

	while (confirmedPosition==0) {
		printf("Type in which row you want to place your boat(1-10): ");
		scanf("%d", &coordenateX);
		printf("Type in which column you want to place your boat(1-10): ");
		scanf("%d", &coordenateY);
		confirmedPosition = confirmPosition(squares, coordenateX-1, coordenateY-1, horizontally, placedBoats, board, fd);
		if (confirmedPosition==0){
			askPosition(squares,placedBoats,board,fd);
			confirmedPosition=1;
		} //end if
	} //end while
	
} //end askPosition

void fillBoard (int board) {
	int placedBoats = 0;
	int fd;
	int error;
	system("clear");
	if (board == 0) {
		fd = creat("boatsBoardA.txt", 777);
		printf("Lets place your boats player A.\n");
		alarm(2);
		pause();
	} //end if
	else {
		fd = creat("boatsBoardB.txt", 777);
		printf("Lets place your boats player B.\n");
		alarm(2);
		pause();
	} //end else
	
	printf("For each boat you will be indicated how many squares it will occupy.\n");
	alarm(3);
	pause();
	printf("Also before your choose where to place your boat you will have the chance to select if you want it to be placed horizontally or vertically.\n");
	alarm(5);
	pause();
	printf("If you choose horizontal the boat will be placed from the coordenate you choose to spaces that that boat occupy to the right.\n");
	alarm(6);
	pause();
	printf("If you choose vertical the boat will be placed from the coordenate you choose to spaces that that boat occupy down.\n");
	alarm(6);
	pause();
	
	printf("\nThis is your board until now\n");
	printBoard(1);
	while (placedBoats!=5) {
		if (placedBoats==0) {
			askPosition(2, placedBoats, board, fd);
		} //end if
		if (placedBoats==1) {
			askPosition(3, placedBoats, board, fd);
		} //end if
		if (placedBoats==2) {
			askPosition(3, placedBoats, board, fd);
		} //end if
		if (placedBoats==3) {
			askPosition(4, placedBoats, board, fd);
		} //end if
		if (placedBoats==4) {
			askPosition(5, placedBoats, board, fd);
		} //end if
		printf("\nThis is your board untiL now\n");
		printBoard(1);
		placedBoats++;
	} //end while 
	
	alarm(4);
	pause();
	
	close(fd);
	
	
	
} //end fillBoard

void retrieveBoats () {
	int fdA, fdB;
	int status;
	pid_t pidA, pidB;
	system("clear");
	//retrieve positions from child
	fdA = open("boatsBoardA.txt", 0);
	if (fdA==-1) {
		perror("Error");
	} //end if
	fdB = open("boatsBoardB.txt", 0);
	if (fdB==-1) {
		perror("Error");
	} //end if
	for (int i=0; i<17; i++) {
		read(fdA, &XcoordenatesBoatsBoardA[i], sizeof(int));
		read(fdB, &XcoordenatesBoatsBoardB[i], sizeof(int));
		read(fdA, &YcoordenatesBoatsBoardA[i], sizeof(int));
		read(fdB, &YcoordenatesBoatsBoardB[i], sizeof(int));

	} //end for
	
	close(fdA);
	close(fdB);
	
	pidA = fork();
	if (pidA==0) {
		execl("/bin/rm","rm", "boatsBoardA.txt", NULL);
	} //end if
	waitpid(pidA,&status,0);
	pidB = fork();
	if (pidB==0) {
		execl("/bin/rm","rm", "boatsBoardB.txt", NULL);
	} //end if
	waitpid(pidB,&status,0);
	system("clear");
	
} //end retrieveBoats

void showWinMessage (int winner) {
	system("clear");
	if (winner==0) {
		printf("PLAYER A IS THE WINNER\nTHANKYOUR FOR PLAYING BATTLESHIP:)\n");
	} //end if
	if (winner==1) {
		printf("PLAYER B IS THE WINNER\nTHANKYOUR FOR PLAYING BATTLESHIP:)\n");
	} //end if
} //end showWinMessage

void *checkWinA (void *arg) {
	while (win==0) {
		if (hitsA==17) {
			pthread_mutex_lock(&mutex);
			win=1;
			pthread_mutex_unlock(&mutex);
			showWinMessage(0);
		} //end if
	} //end while
	pthread_exit(NULL);
} //end checkWin

void *checkWinB (void *arg) {
	while (win==0) {
		if (hitsB==17) {
			pthread_mutex_lock(&mutex);
			win=1;
			pthread_mutex_unlock(&mutex);
			showWinMessage(1);
		} //end if
	} //end while
	pthread_exit(NULL);
} //end checkWin

void checkHit (int coordenateX,int coordenateY,int turn) {
	int hit = 0;
	
	if (turn==0) {
		for (int i = 0; i<17; i++) {
			if ((coordenateX == XcoordenatesBoatsBoardB[i])&&(coordenateY==YcoordenatesBoatsBoardB[i])) {
				hit = 1;
				break;
			} //end if
		} //end for
		if (hit) {
			hitsA++;
			boardB[coordenateX][coordenateY] = 1;
			printf("Hit number %d!\n", hitsA);
			alarm(2);
			pause();
		} //end if
		else {
			boardB[coordenateX][coordenateY] = -1;
			printf("Miss!\n");
			alarm(2);
			pause();
		} //end else
	} //end if
	
	if (turn==1) {
		for (int i = 0; i<17; i++) {
			if ((coordenateX == XcoordenatesBoatsBoardA[i])&&(coordenateY==YcoordenatesBoatsBoardA[i])) {
				hit = 1;
				break;
			} //end if
		} //end for
		if (hit) {
			hitsB++;
			boardA[coordenateX][coordenateY] = 1;
			printf("Hit number %d!\n", hitsB);
			alarm(2);
			pause();
		} //end if
		else {
			boardA[coordenateX][coordenateY] = -1;
			printf("Miss!\n");
			alarm(2);
			pause();
		} //end else
	} //end if
	
	
} //end checkHit

int checkNotPreviousShot (int coordenateX,int coordenateY,int turn) {
	if (turn==0) {
		if (boardB[coordenateX][coordenateY]==0) {
			return 1;
		} //end if
	} //end if
	if (turn==1) {
		if (boardA[coordenateX][coordenateY]==0) {
			return 1;
		} //end if
	} //end if
	return 0;
} //end checkNotPreviousSHot

int checkShootCoordenate(int coordenateX,int coordenateY, int turn) {
	int notPreviousShot = checkNotPreviousShot(coordenateX,coordenateY,turn);
	if (coordenateY<=N && coordenateX<=N && coordenateY>=0 && coordenateX>=0 && notPreviousShot) {
			alarm(1);
			pause();
			printf("We have confirmed your shooting coordenates %d,%d\n", coordenateX+1, coordenateY+1);
			return 0;
	} //end if

	else {
		alarm(1);
		pause();
		printf ("The coordenates you selected are invalid, try other ones...\n");
		return 1;
	} //end else

} //end checkShootCoordenate

void changeTurn() {
	int change = 0;
	system("clear");
	while (change!=1) {
		printf("Press 1 to change turn: ");
		scanf("%d", &change);
	} //end while
	system("clear");
} //end changeTurn

void shoot(int turn) {
	int coordenateX, coordenateY;
	int notCheckedCoordenate = 1;
	alarm(2);
	pause();
	printf("This is what we know about the enemy until now\n");
	printBoard(turn);
	while (notCheckedCoordenate) {
		alarm(2);
		pause();
		printf("Select the row you want yo shoot (1-10): ");
		scanf("%d", &coordenateX);
		printf("Select the column you want yo shoot (1-10): ");
		scanf("%d", &coordenateY);
		notCheckedCoordenate = checkShootCoordenate(coordenateX-1,coordenateY-1, turn);
	} //end while
	
	checkHit(coordenateX-1,coordenateY-1,turn);
} //end shoot

void play () {
	int turn = 0;
	pthread_t threadCheckWinA;
	pthread_create(&threadCheckWinA,NULL,checkWinA,NULL);
	pthread_t threadCheckWinB;
	pthread_create(&threadCheckWinB,NULL,checkWinB,NULL);
	
	while (win==0) {
		if (win==0) {
			printf ("PLAYER A\n\nThe enemy has hit %d boats, keep it up!\n", hitsB );
			shoot(0);
		} //end
		if (win==0) {
			changeTurn();
		} //end
		if (win==0) {
			printf ("PLAYER B\n\nThe enemy has hit %d boats, keep it up!\n", hitsA );
			shoot(1);
		} //end
		if (win==0) {
			changeTurn();
		} //end
		
	} //end while
		
} //end play

int main () {
	signal (SIGALRM, alrmHandler);
	pid_t pidBoardA, pidBoardB;
	int status;
	pthread_mutex_init (&mutex,0);
	//get position boats boardA
	pidBoardA = fork();
	if (pidBoardA == 0) {
		fillBoard(0);
		return 0;
	} //end pidBoardA
	
	waitpid(pidBoardA, &status, 0);
	
	//get position boats boardB
	pidBoardB = fork();
	if (pidBoardB == 0) {
		changeTurn();
		fillBoard(1);
		return 0;
	} //end pidBoardB
	
	waitpid(pidBoardB, &status, 0);
	
	//save the coordenate of the boats in the main process
	retrieveBoats(); 
	
	//begin cycle of game
	play();
	
	pthread_mutex_destroy (&mutex);
	
	return 0;
} //end main
