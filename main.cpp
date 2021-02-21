#include <iostream>
#include <fstream>
#include <cstring>

#define DEFAULT_LEN 16777216

using namespace std;

struct ByteData{
    char * data;
    int max_len;
    int file_loc;
    ByteData():file_loc(0),max_len(0){
        allocateMemory();
    }
    ~ByteData(){
        free(data);
    }
    void write(char byte_to_save){
        if(file_loc >= max_len){
            allocateMemory(max_len *2);
        }
        *(data + file_loc++ ) = byte_to_save;
    }
    char * getData(){
        return data;
    }
    int getByteLen(){
        return sizeof(char) * file_loc;
    }
    void encode(){
        switchHeader(75);
    }
    void decode(){
        switchHeader(200);
    }
private:
    //�����ڴ�
    void allocateMemory(int len = DEFAULT_LEN){
        if(max_len >= len) return;
        if(max_len <= 0){
            data = (char *) malloc(sizeof(char) * len);
        }else{
            realloc(data,sizeof(char)*len);
        }
        this->max_len = len;
    }
    void switchHeader(int pivot){
        char * tmpdata = (char *) malloc(sizeof(char) * max_len);
//        strcpy(tmpdata,data);
        int index = 0;
        for(int i=pivot;i<275;i++){
            tmpdata[index++] = data[i];
        }
        for(int i=0;i<pivot;i++){
            tmpdata[index++] = data[i];
        }
        while(index < file_loc){
            tmpdata[index] = data[index];
            index++;
        }
        free(data);
        this->data = tmpdata;
    }
};

void processFile(char * file_name,int mode);

int main(int argc, char *argv[]) {
    if(argc == 1){
        cout << "Wrong Usage! Example: main.exe swf_file_name[s]" << endl;
        cout << "��ȷʹ�÷�����\n1.��Windows PowerShell �� Command Prompt \n"
                "2.��קZaoMengFlashCracker.exe��PowerShell��Command Prompt����\n"
                "3.�ظ��ò������޴Σ�����һ���ո�����קһ����Ҫ�����swf�ļ���powershell����\n"
                "����ק��swf�ļ������ƽ�/����\n";
        system("pause");
        return 1;
    }else{
        cout << "ѡ��ģʽ: Select Mode:" << endl
            << "0 - ����ģʽ Decode" << endl << "1 - ����ģʽ Encode" << endl << "�������ִ�����ѡ��: ";
        int mode = 0;
        cin >> mode;
        for(int i = 1 ; i < argc ; i++){
            processFile(argv[i],mode);
            if(mode == 0) cout << "Decode Successful! File: " << argv[i] << endl;
            if(mode == 1) cout << "Encode Successful! File: " << argv[i] << endl;
        }
    }
    return 0;
}

void processFile(char * file_name, int mode){
    //�洢
    ByteData fdata;
    //���ļ�
    ifstream fin(file_name,ios::in | ios::binary);
    for(char tmp; fin.read( (char *) &tmp, sizeof(char));){
        fdata.write(tmp);
    }
    fin.close();
    if(mode == 0) fdata.decode();
    if(mode == 1) fdata.encode();
    ofstream fout(file_name, ios::out | ios::binary);
    fout.write(fdata.getData(),fdata.getByteLen());
    fout.close();
}
