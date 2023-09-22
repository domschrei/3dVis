CC	= g++
#CCFLAGS	= -O2 -Wall -g
CCFLAGS	= -fopenmp -O3 -Wall -DNDEBUG
LNFLAGS	= -fopenmp -lglut -lXi -lX11 -lXext -lGL -lGLU

PROG1 = cnf3d2
OBJS1 = Vector.o Node.o Graph.o SpaceGrid.o SceneParameters.o arcball.o cnf3d2.o

progs: $(PROG1)

all: progs

$(PROG1): $(OBJS1)
	$(CC) -o $@ $(OBJS1) $(LNFLAGS)

%.o: %.cpp
	$(CC) $(CCFLAGS) -c $<
%.o: arcball/%.cpp
	$(CC) $(CCFLAGS) -c $<

depend:
	g++ -MM arcball/*.cpp *.cpp

clean:
	rm -f $(OBJS1)

realclean: clean
	rm -f $(PROG1)

# dependencies generated with 'make depend'
arcball.o: arcball/arcball.cpp arcball/arcball.h
sample.o: arcball/sample.cpp arcball/arcball.h
Graph.o: Graph.cpp Graph.h Node.h Vector.h SceneParameters.h SpaceGrid.h
Node.o: Node.cpp Node.h Vector.h
SceneParameters.o: SceneParameters.cpp SceneParameters.h
SpaceGrid.o: SpaceGrid.cpp SpaceGrid.h Node.h Vector.h
Vector.o: Vector.cpp Vector.h
cnf3d2.o: cnf3d2.cpp CLI11.hpp Graph.h Node.h Vector.h SceneParameters.h \
 arcball/arcball.h
