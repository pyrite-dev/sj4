#include <sj4ime.h>
#include <stdio.h>

#ifdef _WIN32
#define CHARSET SJ4SJIS
#else
#define CHARSET SJ4UTF8

#include <termios.h>

#define getch getchar
#endif

static void ime_packet(Sj4Ime* ime, int type, void* data) {
	printf("%d %p\n", type, data);
}

int main() {
	Sj4Ime* ime = sj4_ime(CHARSET, "sj4main.dic", ime_packet);
	char	b;
	char*	conv  = sj4_ime_convbuf(ime);
	char*	kouho = sj4_ime_kouho(ime);

	if(ime == NULL) {
		fprintf(stderr, "Failed to create context\n");
		return 1;
	}

#ifdef _WIN32
#else
	{
		struct termios new;
		tcgetattr(0, &new);
		new.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(0, TCSANOW, &new);
	}
#endif

	while((b = getch()) != EOF) {
		sj4_ime_key(ime, b);

		if(conv[0]) {
			printf("[%s]\n", conv);

			conv[0] = 0;
		} else if(kouho[0]) {
			printf("<%s>", kouho);
		}

		if(b != '\n' && b != '\n') {
			putchar(b);
		}
	}
}
