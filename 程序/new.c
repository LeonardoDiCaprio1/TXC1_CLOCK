#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int
#include "24c02.h"

sbit dula = P2 ^ 6;
sbit wela = P2 ^ 7;
sbit rs = P3 ^ 5;
sbit lcden = P3 ^ 4;

sbit s1 = P3 ^ 0;
sbit s2 = P3 ^ 1;
sbit s3 = P3 ^ 2;
sbit s4 = P3 ^ 3;

sbit rd = P3 ^ 7;
sbit beep = P2 ^ 3;
uchar code table1[] = "Clock:";
uchar code table2[] = "Time:";
uchar count1, count2, s1num1, s1num2, num1, num2;
char R_miao, R_shi, R_fen, T_miao, T_shi, T_fen;
int flag1, flag2, flag3, flag4, beepflag1, beepflag2;
uchar  flicker_flag;
int miao_arrive_flag, fen_arrive_flag, shi_arrive_flag;

void di();
void Init();
void keyscan();
void TimeInit();
void Time_mode();
void Clock_mode();
void LcdDisplay();
void lcd1602init();
void delayms(uint z);
void fliker_rewrite();
void write_com(uchar);
void write_date(uchar);
void At24c02DataReadInit();
void write_sfm1(uchar add, uchar date);
void write_sfm2(uchar add, uchar date);
void stop_beep(char, char, char, char, char, char);

void main() {
	Init();
	while (1) {
		keyscan();
		stop_beep(T_shi, T_fen, T_miao, R_shi, R_fen, R_miao);
		LcdDisplay();
	}
}

void TimeInit() {
	TMOD = 0x01;
	TH0 = (65535 - 50000) / 256;
	TL0 = (65535 - 50000) % 256;
	EA = 1;
	ET0 = 1;
	TR0 = 1;
}
void lcd1602init() {

	write_com(0x38);
	write_com(0x0c);
	write_com(0x06);
	write_com(0x01);

	write_com(0x80 + 0x40 + 15);
	write_date('1');

	write_com(0x80);
	for (num1 = 0; num1 < 6; num1++) {
		write_date(table1[num1]);
		delayms(5);
	}
	write_com(0x80 + 0x08);
	write_date(':');
	write_com(0x80 + 0x0B);
	write_date(':');
	write_sfm1(0x06, R_shi);
	write_sfm1(0x09, R_fen);
	write_sfm1(0x0c, R_miao);

	write_com(0x80 + 0x40);
	for (num2 = 0; num2 < 5; num2++) {
		write_date(table2[num2]);
		delayms(5);
	}

	write_com(0x80 + 0x40 + 8);
	write_date(':');
	write_com(0x80 + 0x40 + 11);
	write_date(':');

	write_sfm2(6, T_shi);
	write_add(4, T_shi);
	write_sfm2(9, T_fen);
	write_add(5, T_fen);
	write_sfm2(12, T_miao);
	write_add(6, T_miao);
}

int isValidTime(int hour, int minute, int second) {
	return (hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59 && second >= 0 && second <= 59);
}

void At24c02DataReadInit() {
	R_shi = read_add(1);
	R_fen = read_add(2);
	R_miao = read_add(3);
	T_shi = read_add(4);
	T_fen = read_add(5);
	T_miao = read_add(6);

	if (!isValidTime(R_shi, R_fen, R_miao) || !isValidTime(T_shi, T_fen, T_miao)) {
		R_shi = 0;
		R_fen = 0;
		R_miao = 0;
		T_shi = 0;
		T_fen = 0;
		T_miao = 0;
	}
}
void Init() {
	rd = 0;
	dula = 0;
	wela = 0;
	lcden = 0;
	init_24c02();
	At24c02DataReadInit();
	lcd1602init();
	TimeInit();

}
void LcdDisplay() {
	write_sfm2(12, T_miao);
	write_add(6, T_miao);
	if (miao_arrive_flag == 1) {
		write_sfm2(12, T_miao);
		write_add(6, T_miao);
		miao_arrive_flag = 0;
	}
	if (fen_arrive_flag == 1) {
		write_sfm2(9, T_fen);
		write_add(5, T_fen);
		fen_arrive_flag = 0;
	}
	if (shi_arrive_flag == 1) {
		write_sfm2(12, T_shi);
		write_add(4, T_shi);
		shi_arrive_flag = 0;
	}
}
void keyscan() {
	if (s1 == 0 && flag1 == 0) {
		delayms(5);
		if (s1 == 0) {
			beepflag1 = 1;
			beepflag2 = 1;
			while (!s1);
			di();
			s1num2 = 0;
			// write_com(0x80);
			// write_com(0x0f);
		}
		flag1 = 1;
		flag2 = 0;
		flag3 = 1;
		beepflag1 = 0;
		beepflag2 = 0;
	}
	if (flag1 == 1 && flag2 == 0 && flag3 == 1) {
		Clock_mode();
		write_com(0x80 + 0x40 + 15);
		write_date('1');
	}
	if (s1 == 0 && flag1 == 1) {
		delayms(5);
		if (s1 == 0) {
			while (!s1);
			di();
			s1num1 = 0;
			// write_com(0x80 + 0x40);
			// write_com(0x0f);
		}
		flag1 = 0;
		flag2 = 1;
		flag3 = 0;
		beepflag1 = 0;
		beepflag2 = 0;
	}
	if (flag1 == 0 && flag2 == 1 && flag3 == 0) {
		Time_mode();
		write_com(0x80 + 0x40 + 15);
		write_date('2');
	}
}
void Clock_mode() {
	if (s2 == 0) {
		delayms(5);
		if (s2 == 0) {
			beepflag1 = 0;
			beepflag2 = 1;
			s1num2++;
			while (!s2);
			di();
			if (s1num2 == 4) {
				s1num2 = 0;
				beepflag1 = 0;
				beepflag2 = 0;
				// write_com(0x0c);
				// write_com(0x0f);
			}
		}
	}
	if (s1num2 != 0) {
		if (s3 == 0) {
			delayms(5);
			if (s3 == 0) {
				while (!s3);
				di();
				if (s1num2 == 1) {
					beepflag1 = 1;
					beepflag2 = 1;
					R_miao++;
					if (R_miao == 60)
						R_miao = 0;
					write_sfm1(0x0c, R_miao);
					write_add(3, R_miao);
				}
				if (s1num2 == 2) {
					beepflag1 = 1;
					beepflag2 = 1;
					R_fen++;
					if (R_fen == 60)
						R_fen = 0;
					write_sfm1(0x09, R_fen);
					write_add(2, R_fen);
				}
				if (s1num2 == 3) {
					beepflag1 = 1;
					beepflag2 = 1;
					R_shi++;
					if (R_shi == 24)
						R_shi = 0;
					write_sfm1(0x06, R_shi);
					write_add(1, R_shi);
				}

			}
		}
		if (s4 == 0) {
			delayms(5);
			if (s4 == 0) {
				while (!s4);
				di();
				if (s1num2 == 1) {
					beepflag1 = 1;
					beepflag2 = 1;
					R_miao--;
					if (R_miao == -1)
						R_miao = 59;
					write_sfm1(0x0c, R_miao);
					write_add(3, R_miao);
				}
				if (s1num2 == 2) {
					beepflag1 = 1;
					beepflag2 = 1;
					R_fen--;
					if (R_fen == -1)
						R_fen = 59;
					write_sfm1(0x09, R_fen);
					write_add(2, R_fen);
				}
				if (s1num2 == 3) {
					beepflag1 = 1;
					beepflag2 = 1;
					R_shi--;
					if (R_shi == -1)
						R_shi = 23;
					write_sfm1(0x06, R_shi);
					write_add(1, R_shi);
				}
			}
		}
	}
}
void Time_mode() {
	if (s2 == 0) {
		delayms(5);
		if (s2 == 0) {

			s1num1++;
			while (!s2);
			di();
			if (s1num1 == 4) {
				s1num1 = 0;
				write_com(0x0c);
				// write_com(0x0f);
				TR0 = 1;
			}
		}
	}
	if (s1num1 != 0) {
		if (s3 == 0) {
			delayms(5);
			if (s3 == 0) {
				while (!s3);
				di();
				if (s1num1 == 1) {
					T_miao++;
					if (T_miao == 60)
						T_miao = 0;
					write_sfm2(12, T_miao);
					write_add(6, T_miao);
				}
				if (s1num1 == 2) {
					T_fen++;
					if (T_fen == 60)
						T_fen = 0;
					write_sfm2(9, T_fen);
					write_add(5, T_fen);
				}
				if (s1num1 == 3) {
					T_shi++;
					if (T_shi == 24)
						T_shi = 0;
					write_sfm2(6, T_shi);
					write_add(4, T_shi);
				}
			}
		}
		if (s4 == 0) {
			delayms(5);
			if (s4 == 0) {
				while (!s4);
				di();
				if (s1num1 == 1) {
					T_miao--;
					if (T_miao == -1)
						T_miao = 59;
					write_sfm2(12, T_miao);
					write_add(6, T_miao);
				}
				if (s1num1 == 2) {
					T_fen--;
					if (T_fen == -1)
						T_fen = 59;
					write_sfm2(9, T_fen);
					write_add(5, T_fen);
				}
				if (s1num1 == 3) {
					T_shi--;
					if (T_shi == -1)
						T_shi = 23;
					write_sfm2(6, T_shi);
					write_add(4, T_shi);
				}
			}
		}
	}
}
void fliker_rewrite() {
	write_sfm1(0x06, R_shi);
	write_date(':');
	write_sfm1(0x09, R_fen);
	write_date(':');
	write_sfm1(0x0c, R_miao);
}
void stop_beep(char T_shi, char T_fen, char T_miao, char R_shi, char R_fen, char R_miao) {
	uint num;
	if (T_shi == R_shi && T_fen == R_fen && T_miao == R_miao) {
		while (beepflag1 == 0 && beepflag2 == 0) {
			if (flicker_flag == 1) {
				write_com(0x80 + 0x06);
				for (num = 0; num < 8; num++)
					write_date(' ');
			} else
				fliker_rewrite();
			di();
			LcdDisplay();
			delayms(5);
			if (s1 == 0 || s2 == 0 || s3 == 0 || s4 == 0) {
				delayms(5);
				if (s1 == 0) {
					while (!s1);
					beep = 1;
					beepflag1 = 1;
					fliker_rewrite();
				} else if (s2 == 0) {
					while (!s2);
					beep = 1;
					beepflag1 = 1;
					fliker_rewrite();
				} else if (s3 == 0) {
					while (!s3);
					beep = 1;
					beepflag1 = 1;
					fliker_rewrite();
				} else if (s4 == 0) {
					while (!s4);
					beep = 1;
					beepflag1 = 1;
					fliker_rewrite();
				}
			}
		}
	}
	if (beepflag1 == 0 && beepflag2 == 1)
		beep = 1;
}
void di() {
	beep = 0;
	delayms(200);
	beep = 1;
	delayms(200);
}
void write_sfm1(uchar add, uchar date) {
	uchar R_shi, R_ge;
	R_shi = date / 10;
	R_ge = date % 10;
	write_com(0x80 + add);
	write_date(0x30 + R_shi);
	write_date(0x30 + R_ge);
}
void write_sfm2(uchar add, uchar date) {
	uchar T_shi, T_ge;
	T_shi = date / 10;
	T_ge = date % 10;
	write_com(0x80 + 0x40 + add);
	write_date(0x30 + T_shi);
	write_date(0x30 + T_ge);
}
void write_com(uchar com) {
	rs = 0;
	lcden = 0;
	P0 = com;
	delayms(5);
	lcden = 1;
	delayms(5);
	lcden = 0;
}
void write_date(uchar date) {
	rs = 1;
	lcden = 0;
	P0 = date;
	delayms(5);
	lcden = 1;
	delayms(5);
	lcden = 0;
}
void delayms(uint z) {
	uint x, y;
	for (x = z; x > 0; x--)
		for (y = 110; y > 0; y--);
}

void timer0() interrupt 1 {

	TH0 = (65536 - 50000) / 256;
	TL0 = (65536 - 50000) % 256;
	count1++;
	count2++;
	if (count1 == 20) {
		count1 = 0;
		T_miao++;
		write_add(6, T_miao);
		if (T_miao == 60)
			miao_arrive_flag = 1;
	}
	if (T_miao >= 60) {
		T_miao = 0;
		T_fen++;
		fen_arrive_flag = 1;
	}
	write_add(5, T_fen);
	if (T_fen >= 60) {
		T_fen = 0;
		T_shi++;
		shi_arrive_flag = 1;
	}

	if (T_shi == 24)
		T_shi = 0;
	write_add(4, T_shi);
	if (count2 == 10) {
		count2 = 0;
		flicker_flag = !flicker_flag;
	}
}
