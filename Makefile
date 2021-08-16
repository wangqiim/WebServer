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
CFLAGS = -g -I${IDIR} -Wall -lpthread

_OBJECTS = \
	Channel \
	Poller \
	EventLoop \
	Socket \
	TcpConnection \
	TcpServer \
	EchoServer \
	HttpSession \
	HttpServer \
	EventLoopThread \
	EventLoopThreadPool \
	ThreadPool \

OBJECTS = $(patsubst %,${OBJECTDIR}/%.o,$(_OBJECTS))

_DEPS = *.h
DEPS = $(patsubst %,${IDIR}/%,$(_DEPS))

.PHONY: clean echomain.out httpmain.out

echomain.out: ${SRCDIR}/main.cc
	$(CC) -c $< $(CFLAGS) -o $@ -D ECHOSERVER

httpmain.out: ${SRCDIR}/main.cc
	$(CC) -c $< $(CFLAGS) -o $@ -D HTTPSERVER

echoserver: echomain.out ${OBJECTS}
	${CC} $^ ${CFLAGS} -o ${BINDIR}/$@
	${BINDIR}/$@

httpserver: httpmain.out ${OBJECTS}
	${CC} $^ ${CFLAGS} -o ${BINDIR}/$@
	${BINDIR}/$@

# echoserver: ${OBJECTDIR}/main.o ${OBJECTS}
# 	${CC} ${CFLAGS} $^ -o ${BINDIR}/$@
# 	${BINDIR}/$@

# httpserver: ${OBJECTDIR}/main.o ${OBJECTS}
# 	${CC} ${CFLAGS} $^ -o ${BINDIR}/$@
# 	${BINDIR}/$@

${OBJECTDIR}/%.o: ${SRCDIR}/%.cc ${DEPS}
	$(CC) -c $< $(CFLAGS) -o $@

format:
	find . -name *.cc -o -name *.h | xargs clang-format -style=file -i


clean: 
	rm -rf ./build/*
