#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

int main() {
	string fname = "graph_test.txt";
	fstream thefile (fname, ios::in);

	vector<vector<string> > content;
	vector<string> row;
	string line, word;

	int N = 100;
	int* graph[N];

	if (thefile.is_open()) {
		while(getline(thefile, line)) {
			row.clear();
			stringstream str(line);
			while(getline(str, word, ',')) {
				row.push_back(word);
				content.push_back(row);
			}
		}
	} else {
		cout<<"Could not open the file\n";
	}

	for(int i=0;i<content.size();i++) {
		int n_neighbours = content[i].size();
		int graph[i][n_neighbours];
		for(int j=0;j<content[i].size();j++) {
			graph[i][j] = stoi(content[i][j]);
			cout<<graph[i][j]<<" ";
		}
		cout<<""<<endl;
	}
	return 0;
}
