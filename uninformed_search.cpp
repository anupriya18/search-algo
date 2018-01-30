#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <random>
#include <cmath>
#include <ctime>
using namespace std;

int** create_and_place(int** zoo, int n, int row, int column)
{
	int i, j;

    //Create a new state by replicating the current state
 	int ** state;
 	state= new int*[n];
 	for(int i=0; i<n; i++)
 	 	state[i]= new int[n];
 	for(int i=0; i<n; i++)
 	{
 	 	for (int j=0; j<n; j++)
 			state[i][j]=zoo[i][j];
 	}

	// Place the lizard
	state[row][column]=1;

	// Vertical before
	i=row-1;
	while(i>=0 && state[i][column]!=2)
	{
		if(state[i][column]!=1)
		{
			state[i][column]=3;
			i--;
		}
	}	

	// Vertical after
	i=row+1;
	while(i<n && state[i][column]!=2)
	{
		if(state[i][column]!=1)
		{
			state[i][column]=3;
			i++;
		}
	}

	// Horizontal before
	i=column-1;
	while(i>=0 && state[row][i]!=2)
	{
		if(state[row][i]!=1)
		{
			state[row][i]=3;
			i--;
		}
	}	

	// Mark Horizontal after
	i=column+1;
	while(i<n && state[row][i]!=2)
	{
		if(state[row][i]!=1)
		{
			state[row][i]=3;
			i++;
		}
	}	

	// Right diagonal before
	i=row-1;
	j=column-1;
	while(i>=0 && j>=0 && state[i][j]!=2)
	{
		if(state[i][j]!=1)
		{
			state[i][j]=3;
			i--;
			j--;
		}
	}	

	// Right Diagonal after
	i=row+1;
	j=column+1;
	while(i<n && j<n && state[i][j]!=2)
	{
		if(state[i][j]!=1)
		{
			state[i][j]=3;
			i++;
			j++;
		}
	}	

	// Left diagonal before
	i=row+1;
	j=column-1;
	while(i<n && j>=0 && state[i][j]!=2)
	{
		if(state[i][j]!=1)
		{
			state[i][j]=3;
			i++;
			j--;
		}
	}	

	// Left Diagonal after
	i=row-1;
	j=column+1;
	while(i>=0 && j<n && state[i][j]!=2)
	{
		if(state[i][j]!=1)
		{
			state[i][j]=3;
			i--;
			j++;
		}
	}	
	
	return state;
}

int is_goal(int **zoo, int n, int p)
{
	int n_lizards=0;

	// Counts number of lizards and returns true if they have all been placed
	for(int i=0; i<n; i++)
	{
		for(int j=0; j<n; j++)
		{
			if(zoo[i][j]==1)
				n_lizards++;
		}
	}

	if(n_lizards==p)
		return 1;
	else
		return 0;
}

int exists_in(vector<int**> explored_set, int **state, int n)
{
	int length= explored_set.size();
	int i, j, flag1=1, flag2=0;
	int **explored_state;
	vector<int**>::const_iterator it;

	// Checking whether the state exists in the queue or not
	for (it = begin(explored_set); it!=end(explored_set); it++)
    {
    	explored_state= *it;
    	for(i=0; i<n; i++)
    	{
    		for(j=0; j<n; j++)
    		{
    			if(explored_state[i][j]!=state[i][j])
    			{
    				flag1=1;
    				break;
    			}
    		}
    	}
    	if(flag1==0)
    	{
    		flag2=1;
    		break;
    	}
    }
    return flag2;
}

void print_node(int **node, int n)
{
	int i, j;

	ofstream myfile;

	// Convert all attacked positions back to empty
 	for(i=0; i<n; i++)
	{
	 	for(j=0; j<n; j++)
	 	{
	 		if(node[i][j]==3)
	 			node[i][j]=0;
	 	}
  	}

  	// Write the result to output file
  	myfile.open ("output.txt");
	myfile << "OK" << endl;

 	for(i=0; i<n; i++)
 	{
 		for(j=0; j<n; j++)
 		{
 			myfile << node[i][j];
 		}
 		if(i!=n-1)
	 		myfile << endl;
 	}
 	myfile.close();
}

void bfs(int** zoo, int n, int p)
{
	vector<int**> frontier;
	vector<int**> explored_set;

	int flag=0, col=-1, i=0, j=0;

	frontier.push_back(zoo);
	while(!frontier.empty())
	{
		int **node= frontier.front();

		// Goal test
		if(is_goal(node,n,p))
		{
			flag=1;
	 		print_node(node,n);
	 		return;
	 	}

		// Find the next available free column
	 	for(i=0; i<n; i++)
	 	{
	 		for(j=0; j<n; j++)
	 		{
	 			if(node[j][i]==0)
	 			{
	 				col=i;
	 				break;
	 			}
	 			if(col!=-1)
	 				break;
	 		}
	 	}

	 	//BFS Expansion
	 	if(col>-1 && col<n)
	 	{	//Expand all nodes, and add them to the search tree
	 		for(i=0; i<n; i++)
	 		{
	 			if(node[i][col]==0)
	 			{
	 				int **state= create_and_place(node,n,i,col);
	 				if(!exists_in(explored_set, state, n) && !exists_in(frontier, state, n))
	 					frontier.push_back(state);
	 			}
	 		}
	 	}
	 	col=-1;
	 	// Remove state from search tree
	 	frontier.erase(frontier.begin());
	 	explored_set.push_back(node);
	}

	if(flag==0)
	{
		ofstream myfile;
		myfile.open ("output.txt");
		myfile << "FAIL";
		myfile.close();
	}	
}

int dfs_traverse(int **node, vector<int**> explored_set, int n, int p)
{
	explored_set.push_back(node);

	// Goal test
	if(is_goal(node,n,p))
	{
		print_node(node,n);
	 	return 1;
	}

	int col=-1, i, j;
	// Find the next available free column
	for(i=0; i<n; i++)
	{
		for(j=0; j<n; j++)
		{
		 	if(node[j][i]==0)
		 	{
		 		col=i;
		 		break;
			}
			if(col!=-1)				
				break;
		}
	}

	//DFS Expansion
	int x=0;
	if(col>-1 && col<n)
	{	//Expand each node, traverse them depth first, and add them to the search tree
	 	for(i=0; i<n; i++)
	 	{
	 		if(node[i][col]==0)
	 		{
	 			int **state= create_and_place(node,n,i,col);
	 			if(!exists_in(explored_set, state, n))
	 				x=dfs_traverse(state,explored_set,n,p);
	 			if(x==1)
	 				break;
	 		}
	 	}
	}
	if(x==0)
	{
		ofstream myfile;
		myfile.open ("output.txt");
		myfile << "FAIL";
		myfile.close();
	}
	return x;
}

void dfs(int** zoo, int n, int p)
{
	vector<int**> explored_set;

	// Driver to call the recursive dfs_traverse() routine
	dfs_traverse(zoo, explored_set, n, p);
}

int count_empty_spots(int** zoo, int n)
{
	int count=0, i=0, j=0;

	// Count the number of empty spots in the zoo
	for(i=0; i<n; i++)
	{
		for(j=0; j<n; j++)
		{
			if(zoo[i][j]==0)
				count++;
		}
	}
	return count;
}

void throw_lizards_in_zoo(int** zoo, int n, int p)
{
	int i=0, j=0, k=0;

	// Randomly throw p lizards in any p empty positions in the zoo
	for(k=0; k<p; k++)
	{
		i= rand()%n;
		j= rand()%n;
		while(zoo[i][j]!=0)
		{
			i= rand()%n;
			j= rand()%n;
		}
		zoo[i][j]=1;
	}
}

int** copy_state(int** source, int n)
{
	int ** dest;
 	dest= new int*[n];
 	for(int i=0; i<n; i++)
 	 	dest[i]= new int[n];

 	 // Create the replica of the state given and return that new state
	int i=0, j=0;
	for(i=0; i<n; i++)
 	{
 		for(j=0; j<n; j++)
 			dest[i][j]=source[i][j];
 	}
 	return dest;
}

void assign_state(int** source, int** dest, int n)
{
	int i=0, j=0;

	// Copy the source state into the destination
	for(i=0; i<n; i++)
 	{
 		for(j=0; j<n; j++)
 			dest[i][j]=source[i][j];
 	}
}

int count_conflicts(int** zoo, int n)
{
	int count=0, i=0, j=0, k=0;
	for(j=0; j<n; j++)
	{
		for(i=0; i<n; i++)
		{
			if(zoo[i][j]==1)
			{
				// Check vertically downwards
				k=1;
				while((i+k)<n && zoo[i+k][j]!=2)
				{
					if(zoo[i+k][j]==1)
						count++;
					k++;
				}

				// Check horizontally downwards
				k=1;
				while((j+k)<n && zoo[i][j+k]!=2)
				{
					if(zoo[i][j+k]==1)
						count++;
					k++;
				}

				// Check diagonally upwards
				k=1;
				while((i-k)>=0 && (j+k)<n && zoo[i-k][j+k]!=2)
				{
					if(zoo[i-k][j+k]==1)
						count++;
					k++;
				}

				// Check diagonally downwards
				k=1;
				while((i+k)<n && (j+k)<n && zoo[i+k][j+k]!=2)
				{
					if(zoo[i+k][j+k]==1)
						count++;
					k++;
				}
			}
		}
	}
	return count;
}

void pick_and_change_random(int** zoo, int n)
{
	int i=0, j=0, k=0;

	// Pick random lizard;
	i= rand()%n;
	j= rand()%n;
	while(zoo[i][j]!=1)
	{
		i= rand()%n;
		j= rand()%n;
	}
	// Remove it from position
	zoo[i][j]=0;

	// Move it to any other free position on the board
	i= rand()%n;
	j= rand()%n;
	while(zoo[i][j]!=0)
	{
		i= rand()%n;
		j= rand()%n;
	}
	zoo[i][j]=1;
}

void sa(int** zoo, int n, int p)
{
	int flag=0, del_e=-1;
	double T=1, t, prob, r;

	int** current_state= copy_state(zoo,n);
 	int** new_state= copy_state(zoo,n);
	
	// Declaring time structures
	time_t start_t = time(NULL);
	time_t seconds = 290;
	time_t end_t = start_t + seconds;
	time_t present_t;

	// Random number generator
 	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
 	default_random_engine generator(seed);
  	uniform_real_distribution<double> distribution(0.0,1.0);

	int count = count_empty_spots(zoo,n);

	// If enough blank spots exist
	if(count>=p)
	{
		// Randomly throw p lizards into the zoo
		throw_lizards_in_zoo(current_state, n, p);
		new_state= copy_state(current_state,n);

		t=1;

		while(T!=0)
		{
			T=1/(log(t+1));

			// Goal test
			if(count_conflicts(current_state,n)==0)
			{
				print_node(current_state,n);
				flag=1;
				return;
			}

			// Break if time constraint is breached
			present_t = time(NULL);
			if(present_t >= end_t)
				break;

			// Randomly change the position of any lizard
			pick_and_change_random(new_state,n);
			del_e= count_conflicts(current_state,n)-count_conflicts(new_state,n);

			// If new state is better, accept it
			if(del_e>=0)
			{
				assign_state(new_state,current_state,n);
			}
			// Else accept it with some probability
			else if(del_e<0) 
			{
				prob= exp(del_e/T);
				r = distribution(generator); // Generating random number
				if(r<=prob)
					assign_state(new_state,current_state,n);
				else
					assign_state(current_state,new_state,n);
			}
			t++;
		}	
	}

	// If goal state has not been reached, output FAIL
	if(flag==0)
	{
		ofstream myfile;
		myfile.open ("output.txt");
		myfile << "FAIL";
		myfile.close();
	}
}

int main()
{
	string str, search_type;
	int **arr;
	int n=0, p=0;
	int i=0, j=0;

	ifstream file("input.txt");
	while(getline(file,str))
	{
		if(i==0) // Fetching and storing the type of search
		{
            	search_type.assign(str);
		}
		else if(i==1) // Fetching size of the zoo and creating the zoo matrix
		{
			n= stoi(str);
			arr= new int*[n];

			for(j=0; j<n; j++)
				arr[j]= new int[n];
		}
		else if(i==2) // Fetching number of lizards
		{
			p= stoi(str);
		}
		else // Taking zoo structure as input
		{
			for(j=0; j<n; j++)
			{
				arr[i-3][j]= str[j]-48;
			}
		}
		i++;
	}
 
 	// Calling routine based on type of search
	if(search_type=="BFS") 
		bfs(arr, n, p);
	if(search_type=="DFS")
		dfs(arr, n, p);
	if(search_type=="SA")
		sa(arr, n, p);
	return 0;
}
