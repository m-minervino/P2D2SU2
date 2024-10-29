#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

using namespace std;
int main(int argc, char** argv) {

int ni=0;
int nk=0;
int nskip=atoi(argv[2]);

ifstream in_file(argv[1]);
    if (in_file.is_open()) {
        string line;
        int i=0;
		int m=0;
        getline(in_file, line);
		i = line.find_first_of("123456789");
		m = line.find_last_of("123456789");
		line=line.substr(i,m+1);
		i = line.find_first_of("\t ");
        m = line.find_last_of("\t ");
		ni = atof(line.substr(0,i).data());
		nk = atof(line.substr(m+1).data());
		cout << "ni= " << ni << " ; nk= " << nk << "\n";
        in_file.close();
    }

double values[ni][nk][2];
	for (int v = 0; v < 2; v++) {
        for (int k = 0; k < nk; k++) {
            for (int i = 0; i < ni; i++) {
                values[i][k][v] = NAN;
            }
        }
	}
int ntot=ni*nk*2;
double in_values[ntot];
    for (int i = 0; i < ntot; i++) {
		in_values[i] = NAN;
	}

in_file.open(argv[1]);
	if (in_file.is_open()) {
		string line;
		int i=0;
		getline(in_file, line);	//skip first line
		while (getline(in_file, line)) {
			in_values[i] = atof(line.data());
			i++;
		}
		in_file.close();
	}
for (int v = 0; v < 2; v++) {
    for (int k = 0; k < nk; k++) {
        for (int i = 0; i < ni; i++) {
			values[i][k][v] = in_values[ni*nk*v+ni*k+i];
		}
	}
}

FILE* pFile;
string OutFileName="coarsened.p3d";
pFile = fopen (OutFileName.c_str(),"w");
fprintf(pFile, "%d\t%d\n",(ni-1)/(int)pow(2,nskip)+1, (nk-1)/(int)pow(2,nskip)+1);
for (int v = 0; v < 2; v++) {
	for (int k = 0; k < nk; k=k+pow(2,nskip)) {
		for (int i = 0; i < ni; i=i+pow(2,nskip)) {
			fprintf(pFile, "%15.8E\n",values[i][k][v]);
		}
	}
}
fclose (pFile);

}
