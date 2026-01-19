	/* CONVERT2SU2 program, version 1.0
	The program reads in a PLOT3D file containing the mesh
	(it can be either a direct output from CONSTRUCT2D or a coarsened mesh file from COARSEN_P2D)
	and a second real argument (scale_f) that specifies the grid scaling to be applied to the output.
	The code then provides, as an output, a new grid file in the native format used by the
	Stanford University Unstructured (SU2) suite (https://su2code.github.io/), which can be used
	for example to simulate the flow-field around the selected aerofoil using its flow solver module (SU2_CFD).
	Copyright (C) 2024  Mauro Minervino

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <vector>

using namespace std;
int main(int argc, char** argv) {

int ni=0;
int nk=0;
double scale_factor=1;
if (argc > 2) {
       scale_factor = atof(argv[2]);
       cout << "scale factor:" << scale_factor << "\n";
}

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

vector < vector < vector < double > > > values(ni, vector < vector < double > > (nk, vector < double > (2,NAN)));
int ntot=ni*nk*2;
vector < double > in_values(ntot,NAN);

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
string OutFileName=argv[1];
OutFileName.resize(OutFileName.length()-3);
OutFileName=OutFileName+"su2";
pFile = fopen (OutFileName.c_str(),"w");
fprintf(pFile, "%s\n","NDIME= 2");
int nskip=0;
for (int k = 0; k < nk; k++) {
	for (int i = 0; i < ni; i++) {
		if ((k==0) && (values[i][k][0]>=1) && (i>ni/2)) {
			nskip++;
		}
	}
}
fprintf(pFile, "%s%d\n","NPOIN= ",ni*nk-nskip);
for (int k = 0; k < nk; k++) {
	for (int i = 0; i < ni; i++) {
		if ((k==0) && (values[i][k][0]>=1) && (i>ni/2)) {
			//do nothing
		} else {
			fprintf(pFile, "%15.8E\t%15.8E\n",values[i][k][0]*scale_factor,values[i][k][1]*scale_factor);
		}
	}
}
cout << "nskip = " << nskip << "\n";
fprintf(pFile, "%s%d\n","NELEM= ",(ni-1)*(nk-1));
for (int k = 0; k < (nk-1); k++) {
    for (int i = 0; i < (ni-1); i++) {
		if (k==0) {
			if (i<ni-nskip-1) {
		        fprintf(pFile, "%d\t%d\t%d\t%d\t%d\n",9, i, i+1, ni-nskip+i+1, ni-nskip+i);
			} else {
				if (i==ni-nskip-1) {
					fprintf(pFile, "%d\t%d\t%d\t%d\t%d\n",9, i, ni-2-i, ni-nskip+i+1, ni-nskip+i);
				} else {
					fprintf(pFile, "%d\t%d\t%d\t%d\t%d\n",9, ni-1-i, ni-2-i, ni-nskip+i+1, ni-nskip+i);
				}
			}
		} else {
			fprintf(pFile, "%d\t%d\t%d\t%d\t%d\n",9,ni-nskip+(k-1)*ni+i, ni-nskip+(k-1)*ni+i+1, ni-nskip+k*ni+i+1, ni-nskip+k*ni+i);
		}
    }
}
fprintf(pFile, "%s%d\n","NMARK= ",2);
fprintf(pFile, "%s\n","MARKER_TAG= FF");
fprintf(pFile, "%s%d\n","MARKER_ELEMS= ",ni-1+nk-1+nk-1);
for (int k = 0; k < nk-1; k++) {
	if (k==0) {
		fprintf(pFile, "%d\t%d\t%d\n",3, 0, ni-nskip);
		fprintf(pFile, "%d\t%d\t%d\n",3, 0, ni-nskip+ni-1);
	} else {
		fprintf(pFile, "%d\t%d\t%d\n",3, ni-nskip+(k-1)*ni, ni-nskip+k*ni);
		fprintf(pFile, "%d\t%d\t%d\n",3, ni-nskip+k*ni-1, ni-nskip+(k+1)*ni-1);
	}
}	
for (int i = 0; i < ni-1; i++) {
	fprintf(pFile, "%d\t%d\t%d\n",3, ni*nk-nskip-1-(ni-1)+i, ni*nk-nskip-1-(ni-1)+i+1);
}
fprintf(pFile, "%s\n","MARKER_TAG= AIRFOIL");
fprintf(pFile, "%s%d\n","MARKER_ELEMS= ",ni-2*nskip+2-1);
for (int i = nskip-1; i < ni-nskip; i++) {
	if (i==ni-nskip-1) {
		fprintf(pFile, "%d\t%d\t%d\n",3, i, nskip-1);
	} else {
		fprintf(pFile, "%d\t%d\t%d\n",3, i, i+1);
	}
}

fclose (pFile);

}
