IDIR = ./src/include
SRCDIR = ./src
BUILDDIR = ./build
OBJECTDIR = ./build/object
BINDIR = ./build/bin
# create build dir
$(shell mkdir -p ${BUILDDIR})
$(shell mkdir -p ${OBJECTDIR})
$(shell mkdir -p ${BINDIR})

CC = g++
CFLAGS = -I${IDIR}

_OBJECTS = \
	Channel \
	Poller \
	EventLoop \
	Socket \
	TcpConnection \
	TcpServer \
	EchoServer
OBJECTS = $(patsubst %,${OBJECTDIR}/%.o,$(_OBJECTS))

_DEPS = *.h
DEPS = $(patsubst %,${IDIR}/%,$(_DEPS))

echoserver: main
	${BINDIR}/$<

${OBJECTDIR}/%.o: ${SRCDIR}/%.cc ${DEPS}
	$(CC) -c $< $(CFLAGS) -o $@

main: ${OBJECTDIR}/main.o ${OBJECTS}
	${CC} ${CFLAGS} $^ -o ${BINDIR}/$@

format:
	find . -name *.cc -o -name *.h | xargs clang-format -style=file -i

.PHONY: clean

clean: 
	rm -rf ./build/*
