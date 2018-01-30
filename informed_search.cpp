#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <cstdlib>
using namespace std;

#define STAR -99999
#define MAXDEPTH 4
#define ALPHAINIT -99999
#define BETAINIT 99999

struct gametree
{
	int row;
	int col;
	int depth;
	int score;
	int value;
	int** board;
	struct gametree *parent;
};

struct moves
{
	int row;
	int col;
	int score;
	bool operator<( const moves& val ) const { 
        return score < val.score; 
    }
};

struct Xgreater
{
    bool operator()( const moves& lhs, const moves& rhs ) const {
        return lhs.score < rhs.score;
    }
};

struct Xlesser
{
    bool operator()( const moves& lhs, const moves& rhs ) const {
        return lhs.score > rhs.score;
    }
};

int is_terminal(int** board, int n)
{
	int i,j, flag=0;
	for(i=0; i<n; i++)
	{
		for(j=0; j<n; j++)
		{
			if(board[i][j]!=STAR)
			{
				flag=1;
				break;
			}
		}
		if(flag==1)
			break;
	}
	return !flag;
}

void print_board(struct gametree* node, int n)
{
	int i,j;

	//cout<<  "Child of : " << (char)(node->parent->col+65) << node->parent->row+1 << " Score:<< " << node->parent->score << " Move : " << (char)(node->col+65) << node->row+1 << " Score:<< " << node->score << endl;
	for(i=0; i<n; i++)
	{
		for(j=0; j<n; j++)
		{
			if(node->board[i][j]==STAR)
				cout<< "*";
			else
				cout<< node->board[i][j];
		}
		if(i!=n-1)
			cout<< endl;
	}
	cout<<endl;
}

void display_board(struct gametree* node, int n)
{
	int i,j;
	ofstream myfile;
	myfile.open ("output.txt");
	myfile<< (char)(node->col+65) << node->row+1 << endl;
	for(i=0; i<n; i++)
	{
		for(j=0; j<n; j++)
		{
			if(node->board[i][j]==STAR)
				myfile<< "*";
			else
				myfile<< node->board[i][j];
		}
		if(i!=n-1)
			myfile<< endl;
	}
	myfile.close();
}

void call_display(struct gametree* node, int n)
{
	struct gametree* ptr= node;
	while(ptr->parent->parent!=NULL)
	 	ptr= ptr->parent;

	display_board(ptr,n);
}

int** apply_gravity(int **board, int n)
{
 	int i, j, k, low, count;

 	for(i=0; i<n; i++)
 	{
 		count=0;

 		for(j=n-1; j>=0; j--)
 		{
 			while(j-count>=0 && board[j-count][i]==STAR)
	 			count++;

	 		if(j-count>=0)
	 			board[j][i]= board[j-count][i];
 		}

 		for(j=0; j<count; j++)
 			board[j][i]= STAR;
 	}
 	return board;
}

int move_score(int **board, int n, int row, int col, int num)
{
	if(row==-1 || row==n || col==-1 || col==n || board[row][col]!=num)
		return 0;

	board[row][col]=STAR;
	return num + move_score(board,n,row-1, col,num) + move_score(board,n,row+1, col,num) + move_score(board,n,row, col-1,num) + move_score(board,n,row, col+1,num);
}

void print_scorelist(vector<struct moves> sc)
{
	vector<struct moves>::const_iterator it;

	for (it=begin(sc); it!=end(sc); it++)
	   	cout<< "Score:" << (it)->score << " Row:" << (it)->row << " Col:" << (it)->col << endl;
}

vector<struct moves> find_moves(int** board, int n, int maxplayer)
{
	vector<struct moves> moves_list;

	int i,j,count, flag=0;

	// Create copy of the board to find components
	int** board_copy;
	board_copy= new int*[n];
	for(i=0; i<n; i++)
		board_copy[i]= new int[n];
	for(i=0; i<n; i++)
	{
		for(j=0; j<n; j++)				
			board_copy[i][j]=board[i][j];
	}

	// To find all children
	for(i=0; i<n; i++) 
	{
		for(j=0; j<n; j++)
		{
			if(board_copy[i][j]!=STAR)
			{
				struct moves* m= (struct moves*)malloc(sizeof(struct moves));
				m->score= move_score(board_copy,n,i,j,board_copy[i][j]); // Mark component
				m->score= m->score*m->score;
				m->row= i;
				m->col= j;
				moves_list.push_back(*m);
			}
		}
	}
	if(maxplayer)
		sort( moves_list.begin( ), moves_list.end( ), Xlesser());
	else
		sort( moves_list.begin( ), moves_list.end( ), Xgreater());
	return moves_list;
}

struct gametree* create_child(struct gametree* par, int n, int r, int c, int partype)
{
	struct gametree* child= (struct gametree*)malloc(sizeof(struct gametree));
	child->row= r;
	child->col= c;
	child->depth= par->depth+1;

	child->board= new int*[n];
	for(int i=0; i<n; i++)
		child->board[i]=new int[n];
	for(int i=0; i<n; i++)
	{
		for(int j=0; j<n; j++)
			child->board[i][j]=par->board[i][j];
	}

	child->value= move_score(child->board, n, child->row, child->col, par->board[r][c]);
	child->value= child->value*child->value;
	apply_gravity(child->board,n);

	child->score= child->value;
	if(par!=NULL)
		child->score= (-partype * child->score) + par->score;
	child->parent= par;

	return child;
}

void copy_pointers(struct gametree* source, struct gametree** dest, int n)
{
	int i,j;
	(*dest)->row= source->row;
	(*dest)->col= source->col;
	(*dest)->depth= source->depth;

	for(int i=0; i<n; i++)
	{
		for(int j=0; j<n; j++)
			(*dest)->board[i][j]=source->board[i][j];
	}
	(*dest)->value= source->value;
	(*dest)->score= source->score;
	(*dest)->parent= source->parent;
}

int min_alphabeta(struct gametree* node, int n, int maxdepth, int alpha, int beta);
int max_alphabeta(struct gametree* node, int n, int maxdepth, int alpha, int beta)
{
	if(node->depth>maxdepth || is_terminal(node->board,n)) {//call_display(node,n); 
		return node->score;}

	int i,j,count, flag=0;

	int chd; 
	// Generate possible moves
	vector<struct moves> moves_list= find_moves(node->board,n,1);

	struct gametree* maxchild= (struct gametree*)malloc(sizeof(struct gametree));
	maxchild->board= new int*[n];
	for(int i=0; i<n; i++)
		maxchild->board[i]=new int[n];

	int max= ALPHAINIT;

	// Iterate through all possible moves
	vector<struct moves>::const_iterator it;

	for (it=begin(moves_list); it!=end(moves_list); it++)
	{
		// Create child
		struct gametree* child= create_child(node,n,it->row,it->col,1);

		chd= min_alphabeta(child,n,maxdepth,alpha,beta);
		if(chd>max)
		{
			max= child->score;
			copy_pointers(child,&maxchild,n);
		}

		if(alpha<chd)
			alpha= chd;
	
		if (alpha >= beta)
			return beta;
	}

	if(node->depth==0)
		call_display(maxchild,n); 

	return alpha;
}

int min_alphabeta(struct gametree* node, int n, int maxdepth, int alpha, int beta)
{
	if(node->depth>maxdepth || is_terminal(node->board,n)) { return node->score;}

	int i,j,count, flag=0;

	int chd; 
	// Generate possible moves
	vector<struct moves> moves_list= find_moves(node->board,n,0);

	struct gametree* minchild= (struct gametree*)malloc(sizeof(struct gametree));
	minchild->board= new int*[n];
	for(int i=0; i<n; i++)
		minchild->board[i]=new int[n];

	int min=BETAINIT;
	// Iterate through all possible moves
	vector<struct moves>::const_iterator it;

	for (it=begin(moves_list); it!=end(moves_list); it++)
	{

		// Create child
		struct gametree* child= create_child(node,n,it->row,it->col,-1);
		//print_board(child,n);

		chd= max_alphabeta(child,n,maxdepth,alpha,beta);
		if(chd<min)
		{
			min= child->score;
			copy_pointers(child,&minchild,n);
		}

		if(beta>chd)
			beta= chd;
	
		if (alpha >= beta)
			return alpha;
	}

	if(node->depth==0)
		call_display(minchild,n); 

	return beta;
}

int calcdepth(int n, int t_sec)
{
	if(t_sec<3)
		return 0;

	int x= n/5;
	int a= t_sec/60;

	if(x+a>=5)
		return MAXDEPTH-4;
	else if(x+a==4)
		return MAXDEPTH-3;
	else if(x+a==3)
		return MAXDEPTH-2;
	else if(x+a==2)
		return MAXDEPTH-1;
	else
		return MAXDEPTH;
}

int main()
{
	string str, search_type;
	int **arr;
	int n=0, p=0;
	int i=0, j=0;
	float t_sec;

	ifstream file("input.txt");		
	while(getline(file,str))
	{
		if(i==0) // Fetching size of the board and creating the board matrix
		{
			n= stoi(str);
			arr= new int*[n];

			for(j=0; j<n; j++)
				arr[j]= new int[n];
		}
		else if(i==1) // Fetching types of fruits
		{
			p= stoi(str);
		}
		else if(i==2) // Taking Run Time
		{
			t_sec=stof(str);
		}
		else // Taking board structure as input
		{
			for(j=0; j<n; j++)
			{
				if(str[j]=='*')
					arr[i-3][j]=STAR;
				else
					arr[i-3][j]= str[j]-48;
			}
		}
		i++;
	}
	
	// Creating root node
	struct gametree* root= (struct gametree*)malloc(sizeof(struct gametree));
	root->row= -1;
	root->col= -1;
	root->depth= 0;

	root->board= new int*[n];
	for(int i=0; i<n; i++)
		root->board[i]=new int[n];
	for(int i=0; i<n; i++)
	{
		for(int j=0; j<n; j++)
			root->board[i][j]=arr[i][j];
	}
	root->value= 0;
	root->score= 0;
	root->parent= NULL;

	int d= calcdepth(n,t_sec);
	
	// Calling the minimax decision
	if(d==0)
	{
		int r= rand()%n;
		int c= rand()%n;
		while(root->board[r][c]==STAR)
		{
			r= rand()%n;
			c= rand()%n;
		}
		struct gametree *res= create_child(root,n,r,c,1);
		call_display(res,n);
	}
	else
		max_alphabeta(root,n,d,ALPHAINIT,BETAINIT); // Calling the minimax decision
	
	return 0;
}
