/*****************************************************/
/* BSE280.h
 * 温湿度気圧センサ
 * (I2CスレーブIDは0x76もしくは0x77)
 * 
 * 必要ヘッダ
 * ・I2C.h
 * 
 * PIC16F1829
 * OSCは1MHz
 */
/*****************************************************/

#ifndef BSE280_H
#define	BSE280_H

#include "I2C.h"
#define BSE280_I2C_ADD 0x76    //スレーブアドレス
#define _XTAL_FREQ 2000000

signed long int t_fine;

unsigned short dig_T1;
  signed short dig_T2;
  signed short dig_T3;
unsigned short dig_P1;
  signed short dig_P2;
  signed short dig_P3;
  signed short dig_P4;
  signed short dig_P5;
  signed short dig_P6;
  signed short dig_P7;
  signed short dig_P8;
  signed short dig_P9;
          char dig_H1;
  signed short dig_H2;
          char dig_H3;
  signed short dig_H4;
  signed short dig_H5;
          char dig_H6;
          
void comb_sens_write(char data1,char data2){
    i2c_write(BSE280_I2C_ADD,data1,data2);
}
char comb_sens_read(char data){
    return i2c_read(BSE280_I2C_ADD,data);
}
void comb_sens_get(){
    comb_sens_write(0xF4,0b00100101);//ctrl_meas(動作モードの設定)  Forced modeに設定(1度測定したらスリープ)
}
void init_comb_sens(){
    comb_sens_write(0xF5,0b00000000);   //config
    comb_sens_write(0xF2,0b00000010);   //ctrl_hum(ctrl_measより先に書き込むこと) oversamplingx2
    comb_sens_get();                    //ctrl_meas         oversampling各x2
    //データシートより最大測定時間は1回11.6ms
}
/****************************************************
 * データ補正関数 
 * スイッチサイエンスのプログラムのコピペ
 *****************************************************/
signed long int calibration_T(signed long int adc_T)
{
    
    signed long int var1, var2, T;
    var1 = ((((adc_T >> 3) - ((signed long int)dig_T1<<1))) * ((signed long int)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((signed long int)dig_T1)) * ((adc_T>>4) - ((signed long int)dig_T1))) >> 12) * ((signed long int)dig_T3)) >> 14;
    
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T; 
}

unsigned long int calibration_P(signed long int adc_P)
{
    signed long int var1, var2;
    unsigned long int P;
    var1 = (((signed long int)t_fine)>>1) - (signed long int)64000;
    var2 = (((var1>>2) * (var1>>2)) >> 11) * ((signed long int)dig_P6);
    var2 = var2 + ((var1*((signed long int)dig_P5))<<1);
    var2 = (var2>>2)+(((signed long int)dig_P4)<<16);
    var1 = (((dig_P3 * (((var1>>2)*(var1>>2)) >> 13)) >>3) + ((((signed long int)dig_P2) * var1)>>1))>>18;
    var1 = ((((32768+var1))*((signed long int)dig_P1))>>15);
    if (var1 == 0)
    {
        return 0;
    }    
    P = (((unsigned long int)(((signed long int)1048576)-adc_P)-(var2>>12)))*3125;
    if(P<0x80000000)
    {
       P = (P << 1) / ((unsigned long int) var1);   
    }
    else
    {
        P = (P / (unsigned long int)var1) * 2;    
    }
    var1 = (((signed long int)dig_P9) * ((signed long int)(((P>>3) * (P>>3))>>13)))>>12;
    var2 = (((signed long int)(P>>2)) * ((signed long int)dig_P8))>>13;
    P = (unsigned long int)((signed long int)P + ((var1 + var2 + dig_P7) >> 4));
    return P;
}

unsigned long int calibration_H(signed long int adc_H)
{
    signed long int v_x1;
    
    v_x1 = (t_fine - ((signed long int)76800));
    v_x1 = (((((adc_H << 14) -(((signed long int)dig_H4) << 20) - (((signed long int)dig_H5) * v_x1)) + 
              ((signed long int)16384)) >> 15) * (((((((v_x1 * ((signed long int)dig_H6)) >> 10) * 
              (((v_x1 * ((signed long int)dig_H3)) >> 11) + ((signed long int) 32768))) >> 10) + (( signed long int)2097152)) * 
              ((signed long int) dig_H2) + 8192) >> 14));
   v_x1 = (v_x1 - (((((v_x1 >> 15) * (v_x1 >> 15)) >> 7) * ((signed long int)dig_H1)) >> 4));
//   v_x1 = (v_x1 < 0 ? 0 : v_x1);
//   v_x1 = (v_x1 > 419430400 ? 419430400 : v_x1);
   return (unsigned long int)(v_x1 >> 12);   
}
/************************
 * コピペおわり
 ************************/
void read_TPH10x(int *temp,int *press,int *hum){//それぞれ10倍した整数を返す(exp.30.5℃→305)
      signed int buff_temp=0;
    unsigned int buff_press=0;
    unsigned int buff_hum=0;
    
    int i;
    char calib[32];
    
    char raw_hum_r[2];
    char raw_temp_r[3];
    char raw_press_r[3];
    long buff;//ビットシフト時の一時保存
    
    unsigned int  raw_hum=0;
    unsigned long raw_temp=0;
    unsigned long raw_press=0;
    
      signed long cal_temp=0;
    unsigned long cal_press=0;
    unsigned long cal_hum=0;
    
    raw_hum_r[0]=comb_sens_read(0xFE);
    raw_hum_r[1]=comb_sens_read(0xFD);

    raw_temp_r[0]=comb_sens_read(0xFC);
    raw_temp_r[1]=comb_sens_read(0xFB);
    raw_temp_r[2]=comb_sens_read(0xFA);

    raw_press_r[0]=comb_sens_read(0xF9);
    raw_press_r[1]=comb_sens_read(0xF8);
    raw_press_r[2]=comb_sens_read(0xF7);

    raw_hum=raw_hum_r[0] | raw_hum_r[1]<<8;
    raw_temp=raw_temp_r[0]>>4 | raw_temp_r[1]<<4;
    buff=raw_temp_r[2];
    raw_temp|=buff<<12;
    raw_press=raw_press_r[0]>>4 | raw_press_r[1]<<4;
    buff=raw_press_r[2];
    raw_press|=buff<<12;

    for(i=0;i<=23;i++){//レジスタ 0x88~0x9F読み込み
        calib[i]=comb_sens_read(0x88+i);
    }
    calib[24]=comb_sens_read(0xA1);//レジスタ 0xA1読み込み
    for(i=25;i<=31;i++){//レジスタ 0xE1~0xE7読み込み
        calib[i]=comb_sens_read(0xC8+i);
    }
    
    //打つのめんどかったからスイッチサイエンスのコピペ
    dig_T1 = (calib[1] << 8) | calib[0];
    dig_T2 = (calib[3] << 8) | calib[2];
    dig_T3 = (calib[5] << 8) | calib[4];
    dig_P1 = (calib[7] << 8) | calib[6];
    dig_P2 = (calib[9] << 8) | calib[8];
    dig_P3 = (calib[11]<< 8) | calib[10];
    dig_P4 = (calib[13]<< 8) | calib[12];
    dig_P5 = (calib[15]<< 8) | calib[14];
    dig_P6 = (calib[17]<< 8) | calib[16];
    dig_P7 = (calib[19]<< 8) | calib[18];
    dig_P8 = (calib[21]<< 8) | calib[20];
    dig_P9 = (calib[23]<< 8) | calib[22];
    dig_H1 = calib[24];
    dig_H2 = (calib[26]<< 8) | calib[25];
    dig_H3 = calib[27];
    dig_H4 = (calib[28]<< 4) | (0x0F & calib[29]);
    dig_H5 = (calib[30] << 4) | ((calib[29] >> 4) & 0x0F);
    dig_H6 = calib[31];
    //コピペおわり
    
    cal_temp = calibration_T(raw_temp);
    cal_press = calibration_P(raw_press);
    cal_hum = calibration_H(raw_hum);
    
    buff_temp=cal_temp/10;
    buff_press=cal_press/10;
    buff_hum=(int)((double)cal_hum/102.4);
    
    *temp=buff_temp;
    *press=buff_press;
    *hum=(int)buff_hum;
    
}
void add_point(char *buff){//最後の文字の前に小数点を置く("abcd"を"abc.d"にする)
    unsigned char i=0;
    char buff2=0;
    for(i=0;buff[i]!='\0';i++);//文字列の最後「\0」が出るまでインクリメント
    buff2=buff[i-1];//小数第1位
    buff[i-1]='.';
    buff[i]=buff2;
    buff[i+1]='\0';
}
#endif	/* BSE280_H */

