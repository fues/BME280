/*****************************************************/
/* I2C.h
 * 
 * 必要ヘッダ
 *  無し
 * 
 * PIC16F1829
 * OSCは2MHz
 */
/*****************************************************/
#ifndef I2C_H
#define I2C_H

void init_i2c(){
    //I2Cクロック=FOSC/(ADD+1)*4
    //100k=2M/(ADD+1)*4
    //ADD=4
    SSP1ADD=3;      //今回はクロック調整に使用する I2Cモードでは3未満はサポートしていない
    SSP1CON1=0x28;  //RC0,1はI2Cモード。TRISで入力に設定すること
                    //マスタモード、クロックは調整
    SSP1CON2bits.SEN=0;          //スタート許可
    SSP1IE=0;
}
void i2c_write(char add,char data1,char data2){
    SSP1CON2bits.SEN=1;              //IC2開始
        while(SSP1CON2bits.SEN);     //I2C開始確認
    SSP1IF = 0;         //フラグクリア
    SSP1BUF=add<<1;      //スレーブwriteアドレス
        while(!SSP1IF); //送信終了待ち
    SSP1IF = 0;         //終了フラグクリア
    SSP1BUF=data1;       //write1
        while(!SSP1IF); //送信終了待ち
    SSP1IF = 0;         //終了フラグクリア
    SSP1BUF=data2;       //write2
        while(!SSP1IF); //送信終了待ち
    SSP1IF = 0;         //終了フラグクリア
    SSP1CON2bits.PEN=1;              //I2C終了
        while(SSP1CON2bits.PEN);     //I2C終了確認
}
char i2c_read(char add,char data){
    char output=0;
    SSP1CON2bits.SEN=1;              //IC2開始
        while(SSP1CON2bits.SEN);     //I2C開始確認
    SSP1IF = 0;         //フラグクリア
    SSP1BUF=add<<1;      //スレーブwriteアドレス
        while(!SSP1IF); //送信終了待ち
    SSP1IF = 0;         //終了フラグクリア
    SSP1BUF=data;        //write
        while(!SSP1IF); //送信終了待ち
    SSP1IF = 0;         //終了フラグクリア
    SSP1CON2bits.RSEN=1;             //I2C再開
        while(SSP1CON2bits.RSEN);    //I2C再開確認
    SSP1IF = 0;         //フラグクリア
    SSP1BUF=add<<1|0b1;  //スレーブreadアドレス
        while(!SSP1IF); //送信終了待ち
    SSP1IF = 0;         //終了フラグクリア
    SSP1CON2bits.RCEN=1;             //受信モード
        while(SSP1CON2bits.RCEN);    //受信モード確認
    SSP1IF = 0;         //終了フラグクリア
    output=SSP1BUF;      //データ書き込み
    SSP1CON2bits.ACKDT = 1;          //ACKセット
    SSP1CON2bits.ACKEN = 1;          //ACKを返す
        while(SSP1CON2bits.ACKEN);   //ACK送信確認
    SSP1CON2bits.PEN=1;              //I2C終了
        while(SSP1CON2bits.PEN);     //I2C終了確認
    return output;
}

#endif	/* I2C_H */

