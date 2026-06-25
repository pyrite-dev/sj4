#include "sj_kanakan.h"

static void getcost_atrb(SJ4_CONTEXT u_char* p, int* len) {
	*len = 2;
}

static int getcost_ofs(SJ4_CONTEXT u_char* p, int* len) {
	*len = 0;
	return getcost(SJ4_CONTEXT_PASS dicbuf + ((*p & KANJICODEMASK) << 8) + *(p + 1));
}

static int getcost_knj(SJ4_CONTEXT u_char* p, int* len) {
	*len = 1;
	return getcost(SJ4_CONTEXT_PASS askknj[*p & KNJASSYUKUMASK]);
}

static void getcost_hask(SJ4_CONTEXT u_char* p, int* len) {
	*len = 1;
}

static void getcost_kask(SJ4_CONTEXT u_char* p, int* len) {
	*len = 1;
}

static void getcost_norm(SJ4_CONTEXT u_char* p, int* len) {
	*len = codesize(*p);
}

static void getcost_hira(SJ4_CONTEXT u_char* p, int* len) {
	u_char ch;

	*len = codesize(*p);
	ch   = *(p + 1);

	switch(*p) {
	case 0x24:
		if(ch >= 0x21 && ch <= 0x73) return;
		return;

	case 0x25:
		if(ch >= 0x21 && ch <= 0x76) return;
		return;

	default:
		break;
	}
}

int getcost(SJ4_CONTEXT u_char* p) {
	char flg = TRUE;
	int  i;
	int  result = 0;

	if (!p) return result;

	do {
		if(p[codesize(*p)] == KANJISTREND) flg = FALSE;
		switch(*p & KANJIMODEMASK) {
		case ZENHIRAASSYUKU:
			getcost_hask(SJ4_CONTEXT_PASS p, &i);
			break;

		case ZENKATAASSYUKU:
			getcost_kask(SJ4_CONTEXT_PASS p, &i);
			break;

		case OFFSETASSYUKU:
			result = getcost_ofs(SJ4_CONTEXT_PASS p, &i);
			break;

		case AIATTRIBUTE:
			getcost_atrb(SJ4_CONTEXT_PASS p, &i);
			break;

		case LEADINGHANKAKU:
			getcost_norm(SJ4_CONTEXT_PASS p, &i);
			break;

		case KANJIASSYUKU:
			result = getcost_knj(SJ4_CONTEXT_PASS p, &i);
			break;

		case KANAKANCOST:
			result = ((*p & 0xf) << 8) | *(p + 1);
			result++;
			i = 2;
			break;

		default:
			getcost_hira(SJ4_CONTEXT_PASS p, &i);
		}

		if(result) return result - 1;

		p += i;
	} while(flg);

	return 0;
}
