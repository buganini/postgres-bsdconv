PREFIX?=	/usr/local

BSDCONV_FLAGS=	-I${PREFIX}/include -L${PREFIX}/lib -lbsdconv
POSTGRES_FLAGS=	

all: bsdconv.so

bsdconv.so: bsdconv.c
	$(CC) -shared -fPIC ${BSDCONV_FLAGS} ${POSTGRES_FLAGS} -o bsdconv.so bsdconv.c

install:
	@echo NOT IMPLEMENTED

clean:
	rm bsdconv.so
