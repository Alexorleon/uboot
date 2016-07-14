#ifndef REAL210_KEY_H
#define REAL210_KEY_H

typedef struct{
	char esc;
	char home;
	char menu;
	char up;
	char down;
	
	char flag;/*按键按下标志，目的是只读取一次键值数据，以防在等待释放检测之后把键值给赋值复位了*/

}real210_key_def;

int key_loop_conf(void);
char tst_key(void);
char key_loop_read(real210_key_def *key_vals);
#endif
