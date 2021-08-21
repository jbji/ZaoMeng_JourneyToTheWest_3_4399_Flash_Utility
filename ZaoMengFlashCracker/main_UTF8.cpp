#include <iostream>
#include <fstream>
#include <cstring>

#define DEFAULT_LEN 16777216


//EDIT HERE 在这里编辑

#define MY_PIVOT 200
#define MY_END 275

//!!!     For 3,ZaiXu**,      MY_PIVOT = 200,   MY_END = 275.     !!!
//!!!     For 2,YongChuang**, MY_PIVOT = 96,    MY_END = 165.     !!!

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
        switchHeader(MY_END - MY_PIVOT);
    }
    void decode(){
        switchHeader(MY_PIVOT);
    }
private:
    //分配内存
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
        for(int i=pivot;i<MY_END;i++){
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
        cout << "正确使用方法：\n1.打开Windows PowerShell 或 Command Prompt \n"
                "2.拖拽ZaoMengFlashCracker.exe至PowerShell或Command Prompt窗口\n"
                "3.重复该步骤有限次：输入一个空格再拖拽一个需要处理的swf文件进powershell窗口\n"
                "被拖拽的swf文件将被破解/加密\n";
        system("pause");
        return 1;
    }else{
        cout << "选择模式: Select Mode:" << endl
            << "0 - 解密模式 Decode" << endl << "1 - 加密模式 Encode" << endl << "输入数字代号以选择: ";
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
    //存储
    ByteData fdata;
    //打开文件
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
