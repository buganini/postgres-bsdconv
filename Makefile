PREFIX?=	/usr/local
LOCALBASE?=${PREFIX}

BSDCONV_FLAGS=	-I${LOCALBASE}/include -L${LOCALBASE}/lib -lbsdconv
POSTGRES_FLAGS=	-I`pg_config --includedir-server`
INSTALL_PATH=	`pg_config --pkglibdir`

all: bsdconv.so

bsdconv.so: bsdconv.c
	$(CC) -shared -fPIC ${BSDCONV_FLAGS} ${POSTGRES_FLAGS} -o bsdconv.so bsdconv.c

install:
	install -m 555 bsdconv.so ${INSTALL_PATH}

clean:
	rm bsdconv.so
