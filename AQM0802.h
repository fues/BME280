/*****************************************************/
/* AQM0802.h
 * 8x2�sLCD
 * AQM0802A-RN-GBW  �܂���
 * AQM0802A-FLW-GBW�@�Ɏg�p�ł��܂�
 * (I2C�X���[�uID��0x3e)
 * 
 * �K�v�w�b�_
 * �EI2C.h
 * 
 * PIC16F1827
 * PIC16F1823
 * OSC��8MHz
 */
/*****************************************************/
#ifndef AQM0802_H
#define AQM0802_H

#include "I2C.h"
#define AQM0802_I2C_ADD 0x3e    //�X���[�u�A�h���X
#define _XTAL_FREQ 2000000

void lcd_cmd(char data,char type){
    char data2=0;
    if(type){
        data2=0xc0;     //���͕���
    }else{
        data2=0x80;     //���̓R�}���h
    }
    i2c_write(AQM0802_I2C_ADD,data2,data);
}
void init_lcd(){
    lcd_cmd(0x38,0);    //Fuction set1
    __delay_us(30);
    lcd_cmd(0x39,0);    //Fuction set2
    __delay_us(30);
    lcd_cmd(0x14,0);    //OSCfreq
    __delay_us(30);
    lcd_cmd(0x70,0);    //Contrast
    __delay_us(30);
    lcd_cmd(0x56,0);    //power/icon/contrast
    __delay_us(30);
    lcd_cmd(0x6c,0);    //follower
    __delay_ms(250);
    lcd_cmd(0x38,0);    //Function set3
    __delay_us(30);
    lcd_cmd(0x0c,0);    //display ON/OFF
    __delay_us(30);
    lcd_cmd(0x01,0);    //clear display
    __delay_ms(3);
}
void printf(char *txt){
    unsigned char i=0;
    while(txt[i]!='\0'){//������̍Ō�u\0�v���o��܂�LCD�Ƀf�[�^���M
        lcd_cmd(txt[i],1);
        i++;
    }
}
#endif