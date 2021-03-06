#include "bootpack.h"

void init_pic(void)
{
	io_out8(PIC0_IMR,	0xff);  //禁止所有中断
	io_out8(PIC1_IMR,	0xff); //禁止所有中断
	
	io_out8(PIC0_ICW1,	0x11);	//边沿触发模式(edge trigger mode)
	io_out8(PIC0_ICW2,	0x20);	//IRQ0-7由INT20-27接收
	io_out8(PIC0_ICW3,	1<<2);	//PIC1由IRQ2连接
	io_out8(PIC0_ICW4,	0x01);	//无缓冲区模式
	
	io_out8(PIC1_ICW1,	0x11);//边沿触发模式(edge trigger mode)
	io_out8(PIC1_ICW2,	0x28);//IRQ8-15由INT28-2f接收
	io_out8(PIC1_ICW3,	2);//PIC1由IRQ2连接
	io_out8(PIC1_ICW4,	0x01);//无缓冲区模式
	
	io_out8(PIC0_IMR,	0x11); //11111011 PIC1以外全部禁止
	io_out8(PIC1_IMR,	0x11); //11111111 禁止所有中断
	
	return ;
}

#define PORT_KEYDAT 0x0060

struct FIFO8 keyfifo;

void inthandler21(int *esp)
/*来自PS/2键盘的中断*/
{
	unsigned char data;
	io_out8(PIC0_OCW2,0x61); //通知PIC"ICQ-01已经受理完毕"
	data=io_in8(PORT_KEYDAT);
	fifo8_put(&keyfifo,data);
	return ;
}

struct FIFO8 mousefifo;

void inthandler2c(int *esp)
/*来自PS/2鼠标的中断*/
{
	unsigned char data;
	io_out8(PIC1_OCW2,0x64); //通知PIC1 IRQ-12的受理已经完成
	io_out8(PIC0_OCW2,0x62); //通知PIC0 IRQ-02的受理已经完成
	data=io_in8(PORT_KEYDAT);
	fifo8_put(&mousefifo,data);
	return;
}

void inthandler27(int *esp)
/* 对于一部分机种而言，随着PIC的初始化，会产生一次IRQ7中断，如果不对该中断处理程序执行STI，操作系统的启动就会失败*/
{
	io_out8(PIC0_OCW2, 0x67); 
	return;
}