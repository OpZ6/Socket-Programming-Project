all:
	g++ client.cpp supportFunc.cpp -g -std=c++11 -o client
	g++ serverS.cpp supportFunc.cpp -g -std=c++11 -o serverS
	g++ serverL.cpp supportFunc.cpp -g -std=c++11 -o serverL
	g++ serverH.cpp supportFunc.cpp -g -std=c++11 -o serverH
	g++ serverM.cpp supportFunc.cpp -g -std=c++11 -o serverM

clean:
	rm -f client serverS serverL serverH serverM
