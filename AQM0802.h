/*****************************************************/
/* AQM0802.h
 * 8x2行LCD
 * AQM0802A-RN-GBW  または
 * AQM0802A-FLW-GBW　に使用できます
 * (I2CスレーブIDは0x3e)
 * 
 * 必要ヘッダ
 * ・I2C.h
 * 
 * PIC16F1827
 * PIC16F1823
 * OSCは8MHz
 */
/*****************************************************/
#ifndef AQM0802_H
#define AQM0802_H

#include "I2C.h"
#define AQM0802_I2C_ADD 0x3e    //スレーブアドレス
#define _XTAL_FREQ 2000000

void lcd_cmd(char data,char type){
    char data2=0;
    if(type){
        data2=0xc0;     //次は文字
    }else{
        data2=0x80;     //次はコマンド
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
    while(txt[i]!='\0'){//文字列の最後「\0」が出るまでLCDにデータ送信
        lcd_cmd(txt[i],1);
        i++;
    }
}
#endif