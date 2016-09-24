/****************************************/
/* �����x�C���Z���T�̃e�X�g             */
/* �R���p�C��:XC8                       */
/* PIC16F1829                           */
/****************************************/
#define _XTAL_FREQ 2000000
#include <xc.h>
#include <stdlib.h>
#include <stdbool.h>
#include "I2C.h"
#include "AQM0802.h"
#include "BSE280.h"

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable (PWRT enabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = ON        // Internal/External Switchover (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF       // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

void init();

void set_font();//���ꕶ���ǉ�

int cnt_meas=0; //0.016s���ƂɃC���N�������g ����̎����p
int cnt_scrl=0; //2�s�ڂ̕����X�N���[���̎����p

#define button !RC0

enum state{
    disp_hum=1,
    scroll_hum_press,
    disp_press,
    scroll_press_hum,
};

void main(){
    int i;
      signed int temp=0;
    unsigned int press=0;
    unsigned int hum=0;
    char buffer[9];   //������ɂȂ����ϐ����i�[
    char str_temp[9]    ={'�','�','�','�','�','�','�','\0'};
    char str_press[9]   ={0};
    char str_hum[9]     ={0};
    char str_press_hum[17]={0};
    enum state pattern=disp_hum;
    bool read_en=false;
    char scroll=0;//�X�N���[���̊J�n����

    init();
    init_i2c();
    init_lcd();
    set_font(); //1����s����΂悢���悭�Ȃ�����
    init_comb_sens();
    __delay_ms(12);//����҂�
    while(1){
        if(cnt_meas>=125){//2s
            comb_sens_get();
            cnt_meas=0;
            read_en=true;
        }
        if(read_en && cnt_meas>=3){//16ms�ȏ�o���Ă�
            read_TPH10x(&temp,&press,&hum);//�����񂩂���
            read_en=false;
            //���x�̕����񑀍�
            itoa(str_temp,temp,10);
            add_point(str_temp);
            for(i=0;str_temp[i]!='\0';i++);
            str_temp[i++]=0x01;//��
            for(;i<=7;i++){         //8�����ڂ܂ŋ󔒂Ŗ��߂�
                str_temp[i]=' ';
            }
            str_temp[i]='\0';
            //�ŏI�I��str_temp��"TT.T��___ \0"
            //���x�̕����񑀍�
            itoa(str_hum,hum,10);
            add_point(str_hum);
            for(i=0;str_hum[i]!='\0';i++);
            str_hum[i++]='%';//%
            for(;i<=7;i++){         //8�����ڂ܂ŋ󔒂Ŗ��߂�
                str_hum[i]=' ';
            }
            str_temp[i]='\0';
            //�ŏI�I��str_hum��"HH.H%�Q__ \0"
            //�C���̕����񑀍�
            itoa(str_press,press,10);
            add_point(str_press);
            for(i=0;str_press[i]!='\0';i++);
            str_press[i++]='h';//h
            str_press[i++]=0x02;//Pa
            for(;i<=7;i++){         //8�����ڂ܂ŋ󔒂Ŗ��߂�
                str_press[i]=' ';
            }
            str_temp[i]='\0';
            //�ŏI�I��str_press��"PPPP.Ph'Pa' \0"
        }
        switch(pattern){
            case disp_hum:
                if(cnt_scrl>=50){//0.8s���Ƃ�
                    cnt_scrl=0;
                    lcd_cmd(0x80,0);    //1-0 (0x80|DDRAM)
                    printf(str_temp);
                    lcd_cmd(0xc0,0);    //2-0 (0x80|DDRAM)
                    printf(str_hum);
                }
                if(button){
                    pattern=scroll_hum_press;
                    for(i=0;str_hum[i]!='\0';i++){
                        str_press_hum[i]=str_hum[i];
                    }
                    for(i=0;str_press[i]!='\0';i++){
                        str_press_hum[i+8]=str_press[i];
                    }
                    scroll=2;
                    cnt_scrl=20;
                }
                break;
            case scroll_hum_press:
                if(cnt_scrl>=10){//0.32s���Ƃ�
                    cnt_scrl=0;
                    lcd_cmd(0x80,0);    //1-0 (0x80|DDRAM)
                    printf(str_temp);
                    lcd_cmd(0xc0,0);    //2-0 (0x80|DDRAM)
                    for(i=0;i<=7;i++){
                        lcd_cmd(str_press_hum[scroll+i],1);
                    }
                    scroll+=2;
                    if(scroll>=9){
                        pattern=disp_press;
                        cnt_scrl=50;
                    }
                }
                break;
            case disp_press:
                if(cnt_scrl>=50){//0.8s���Ƃ�
                    cnt_scrl=0;
                    lcd_cmd(0x80,0);    //1-0 (0x80|DDRAM)
                    printf(str_temp);
                    lcd_cmd(0xc0,0);    //2-0 (0x80|DDRAM)
                    printf(str_press);
                }
                if(button){
                    pattern=scroll_press_hum;
                    for(i=0;str_press[i]!='\0';i++){
                        str_press_hum[i]=str_press[i];
                    }
                    str_press_hum[8]=' ';
                    for(i=0;str_hum[i]!='\0';i++){
                        str_press_hum[i+9]=str_hum[i];
                    }
                    scroll=2;
                    cnt_scrl=20;
                }
                break;
            case scroll_press_hum:
                if(cnt_scrl>=10){//0.32s���Ƃ�
                    cnt_scrl=0;
                    lcd_cmd(0x80,0);    //1-0 (0x80|DDRAM)
                    printf(str_temp);
                    lcd_cmd(0xc0,0);    //2-0 (0x80|DDRAM)
                    for(i=0;i<=7;i++){
                        lcd_cmd(str_press_hum[scroll+i],1);
                    }
                    scroll+=2;
                    if(scroll>=10){
                        pattern=disp_hum;
                        cnt_scrl=50;
                    }
                }
                break;
        }
//        if(0){
//            lcd_cmd(0x08,0);//display_off
//            SLEEP();
//        }
    }
}
void init(){
    OSCCON = 0b01100000;//���U���g��2MHz
    ANSELA = 0x00;      //D(0) or A(1)
    ANSELB = 0x00;      //D(0) or A(1)
    ANSELC = 0x00;      //D(0) or A(1)
    WPUA   = 0x00;      //�v���A�b�v�s���w��
    WPUB   = 0x50;      //�v���A�b�v�s���w��
    WPUC   = 0x01;      //�v���A�b�v�s���w��
    nWPUEN = 0;         //�v���A�b�v�L����
    TRISA  = 0x00;      //I(1) or O(0)
    TRISB  = 0x50;      //I(1) or O(0)
    TRISC  = 0x01;      //I(1) or O(0)
    PORTA=0;
    PORTB=0;
    PORTC=0;

    /****************************************/
    /* 4/2000000= 0.5us���ƂɂP�p���X       */
    /* �v���X�P�[�� = 1:32                  */
    /* 0.064ms���Ƃ�1�J�E���g               */
    /* �����ݒ� = 6                         */
    /* 0.016s���ƂɃI�[�o�[�t���[           */
    /****************************************/
    OPTION_REG=0b01000100;
    TMR0=6;

    /****************************************/
    /* ���荞�ݐݒ�                         */
    /****************************************/
    GIE    = 1;     //�S�̊��荞�݋���
    INTE   = 1;     //���Ӄ��W���[�����荞�݋���
    PEIE   = 0;     //�O�����荞�݋���
    INTEDG = 0;     //��1=�����オ�� 0=����������
    TMR0IE = 1;     //�^�C�}0���荞�݋���
    SSP1IE = 0;     //SSP���荞�݋���
}
void set_font(){//���ꕶ���ǉ�
    ///////////////////////
    lcd_cmd(0x48,0);
    lcd_cmd(0b00001000,1);
    lcd_cmd(0b00010100,1);
    lcd_cmd(0b00001000,1);
    lcd_cmd(0b00000011,1);
    lcd_cmd(0b00000100,1);
    lcd_cmd(0b00000100,1);
    lcd_cmd(0b00000100,1);
    lcd_cmd(0b00000011,1);//0x01:��
    /////////////////////////
    lcd_cmd(0b00011100,1);
    lcd_cmd(0b00010010,1);
    lcd_cmd(0b00011100,1);
    lcd_cmd(0b00010000,1);
    lcd_cmd(0b00010000,1);
    lcd_cmd(0b00010011,1);
    lcd_cmd(0b00000101,1);
    lcd_cmd(0b00000011,1);//0x02:Pa
    /////////////////////////
    lcd_cmd(0b00001000,1);
    lcd_cmd(0b00001010,1);
    lcd_cmd(0b00010100,1);
    lcd_cmd(0b00000100,1);
    lcd_cmd(0b00000000,1);
    lcd_cmd(0b00000000,1);
    lcd_cmd(0b00000000,1);
    lcd_cmd(0b00000000,1);//0x03:clock1(lifegame)
    ////////////////////////
    lcd_cmd(0b00000100,1);
    lcd_cmd(0b00011000,1);
    lcd_cmd(0b00000110,1);
    lcd_cmd(0b00001000,1);
    lcd_cmd(0b00000000,1);
    lcd_cmd(0b00000000,1);
    lcd_cmd(0b00000000,1);
    lcd_cmd(0b00000000,1);//0x04:clock2(lifegame)
    ////////////////////////
    init_lcd();
}
void interrupt myint(void){
    if(TMR0IF){
        TMR0IF=0;
        cnt_meas++;
        cnt_scrl++;
    }
}