
PRGNAME = msudo

all: $(PRGNAME)

$(PRGNAME): $(PRGNAME).c
	$(CC) -s $(PRGNAME).c -o $(PRGNAME)

clean:
	rm $(PRGNAME)
