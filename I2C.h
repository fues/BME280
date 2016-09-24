/*****************************************************/
/* I2C.h
 * 
 * �K�v�w�b�_
 *  ����
 * 
 * PIC16F1829
 * OSC��2MHz
 */
/*****************************************************/
#ifndef I2C_H
#define I2C_H

void init_i2c(){
    //I2C�N���b�N=FOSC/(ADD+1)*4
    //100k=2M/(ADD+1)*4
    //ADD=4
    SSP1ADD=3;      //����̓N���b�N�����Ɏg�p���� I2C���[�h�ł�3�����̓T�|�[�g���Ă��Ȃ�
    SSP1CON1=0x28;  //RC0,1��I2C���[�h�BTRIS�œ��͂ɐݒ肷�邱��
                    //�}�X�^���[�h�A�N���b�N�͒���
    SSP1CON2bits.SEN=0;          //�X�^�[�g����
    SSP1IE=0;
}
void i2c_write(char add,char data1,char data2){
    SSP1CON2bits.SEN=1;              //IC2�J�n
        while(SSP1CON2bits.SEN);     //I2C�J�n�m�F
    SSP1IF = 0;         //�t���O�N���A
    SSP1BUF=add<<1;      //�X���[�uwrite�A�h���X
        while(!SSP1IF); //���M�I���҂�
    SSP1IF = 0;         //�I���t���O�N���A
    SSP1BUF=data1;       //write1
        while(!SSP1IF); //���M�I���҂�
    SSP1IF = 0;         //�I���t���O�N���A
    SSP1BUF=data2;       //write2
        while(!SSP1IF); //���M�I���҂�
    SSP1IF = 0;         //�I���t���O�N���A
    SSP1CON2bits.PEN=1;              //I2C�I��
        while(SSP1CON2bits.PEN);     //I2C�I���m�F
}
char i2c_read(char add,char data){
    char output=0;
    SSP1CON2bits.SEN=1;              //IC2�J�n
        while(SSP1CON2bits.SEN);     //I2C�J�n�m�F
    SSP1IF = 0;         //�t���O�N���A
    SSP1BUF=add<<1;      //�X���[�uwrite�A�h���X
        while(!SSP1IF); //���M�I���҂�
    SSP1IF = 0;         //�I���t���O�N���A
    SSP1BUF=data;        //write
        while(!SSP1IF); //���M�I���҂�
    SSP1IF = 0;         //�I���t���O�N���A
    SSP1CON2bits.RSEN=1;             //I2C�ĊJ
        while(SSP1CON2bits.RSEN);    //I2C�ĊJ�m�F
    SSP1IF = 0;         //�t���O�N���A
    SSP1BUF=add<<1|0b1;  //�X���[�uread�A�h���X
        while(!SSP1IF); //���M�I���҂�
    SSP1IF = 0;         //�I���t���O�N���A
    SSP1CON2bits.RCEN=1;             //��M���[�h
        while(SSP1CON2bits.RCEN);    //��M���[�h�m�F
    SSP1IF = 0;         //�I���t���O�N���A
    output=SSP1BUF;      //�f�[�^��������
    SSP1CON2bits.ACKDT = 1;          //ACK�Z�b�g
    SSP1CON2bits.ACKEN = 1;          //ACK��Ԃ�
        while(SSP1CON2bits.ACKEN);   //ACK���M�m�F
    SSP1CON2bits.PEN=1;              //I2C�I��
        while(SSP1CON2bits.PEN);     //I2C�I���m�F
    return output;
}

#endif	/* I2C_H */

