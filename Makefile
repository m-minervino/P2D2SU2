#Build P2d2SU2 tools, by Mauro Minervino

CRS = COARSEN_P2D
CNV = CONVERT2SU2
CC = icc
CFLAGS = -std=c++11

default_target: $(ALL)

.PHONY : clean

clean:
	-rm $(CRS) $(CNV)

$(ALL): COARSEN_P2D.cpp CONVERT2SU2.cpp
	$(CC) $(CFLAGS) -o $(CRS) COARSEN_P2D.cpp
	$(CC) $(CFLAGS) -o $(CNV) CONVERT2SU2.cpp

$(CRS): COARSEN_P2D.cpp
	$(CC) $(CFLAGS) -o $(CRS) COARSEN_P2D.cpp

$(CNV): CONVERT2SU2.cpp
	$(CC) $(CFLAGS) -o $(CNV) CONVERT2SU2.cpp
