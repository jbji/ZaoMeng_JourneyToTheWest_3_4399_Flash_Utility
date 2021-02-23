#include <iostream>
#include <fstream>
#include <zlib.h>
#include "amf3.h"

#define DEFAULT_LEN 16777216

using namespace std;

struct ByteData{
    char * data;
    int max_len;
    int file_loc;
    ByteData():file_loc(0),max_len(0){
        allocateMemory();
    }
    ByteData(char * memory, int _max_len,int _file_loc):data(memory),max_len(_max_len),file_loc(_file_loc){}
    ByteData(const ByteData& to_copy){
        data = new char[to_copy.max_len];
        max_len = to_copy.max_len;
        for(int i = 0; i < to_copy.file_loc ; i++){
            data[i] = to_copy.data[i];
        }
        file_loc = to_copy.file_loc;
    }
    ByteData(string byte_str){
        data = new char[byte_str.length()];
        max_len = byte_str.length();
        for(int i = 0; i < byte_str.length() ; i++){
            data[i] = byte_str[i];
        }
        file_loc = byte_str.length();
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
        return file_loc;
    }
    string toString(){
        char * tmp_str = new char[this->getByteLen() + 1];
        for(int i=0; i < this->getByteLen() ; i++){
            tmp_str[i] = this->data[i];
        }
        tmp_str[this->getByteLen()] = '\0';
        return string(tmp_str);
    }
private:
    void allocateMemory(int len = DEFAULT_LEN){
        if(max_len >= len) return;
        if(max_len <= 0){
            data = (char *) malloc(sizeof(char) * len);
        }else{
            realloc(data,sizeof(char)*len);
        }
        this->max_len = len;
    }
};

namespace crack{

    int compress_times = 10;
    const string dict = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

    ByteData * zlib_uncompress(ByteData * compressed_data){
        char * tmp_data = (char *) malloc(sizeof(char) * compressed_data->max_len);
        uLongf tmp_len = (uLongf) compressed_data->max_len;
        int status = uncompress((Bytef *)(tmp_data), &tmp_len,
                                (Bytef *)(compressed_data->getData()), (uLongf)compressed_data->getByteLen());
        if(status!= Z_OK){
            cout << "Uncompress Runtime Error. Code:" << status << endl;
        }
        ByteData * uncompressed_data = new ByteData(tmp_data,compressed_data->max_len,(int)tmp_len);
        return uncompressed_data;
    }
    ByteData * zlib_compress(ByteData * in){
        char * tmp_data = (char *) malloc(sizeof(char) * in->max_len);
        uLongf tmp_len = (uLongf) in->max_len;
        int status = compress2((Bytef *)(tmp_data), &tmp_len,
                               (Bytef *)(in->getData()), (uLongf)in->getByteLen(),8);
        if(status != Z_OK){
            cout << "Compress Runtime Error. Code:" << status << endl;
        }
        ByteData * compressed_data = new ByteData(tmp_data, in->max_len, (int)tmp_len);
        return compressed_data;
    }

    string encrypt(ByteData * to_encrypt){
        string result;
        int translated_str[4] = {0};
        int byte_len = to_encrypt->getByteLen();
        for(int k = 0; k < byte_len;){
            int unencrypted_tmp[3] = {0};
            int index;
            for(index = 0 ; index < 3 && k < byte_len; index++){
                unencrypted_tmp[index] = (unsigned char)((to_encrypt->data)[k++]);
            }
            translated_str[0] = (unencrypted_tmp[0] & 252) >> 2;
            translated_str[1] = (unencrypted_tmp[0] & 3) << 4 | unencrypted_tmp[1] >> 4;
            translated_str[2] = (unencrypted_tmp[1] & 15) << 2 | unencrypted_tmp[2] >> 6;
            translated_str[3] = unencrypted_tmp[2] & 63;
            for(int j = index; j < 3; j++){
                translated_str[j + 1] = 64;
            }
            for(int j = 0; j < 4; j++){
                result += dict[translated_str[j]];
            }
        }
        return result;
    }
    ByteData * decrypt(string to_decrypt_str){
        ByteData * decrypt_result = new ByteData();
        int inputTmp[4] = {0};
        char translatedInput[3] = {'\0'};
        unsigned int len = to_decrypt_str.length();
        for(unsigned int current_pos = 0; current_pos < len; current_pos += 4){
            for(int i = 0 ; i < 4 && current_pos + i < len; i++){
                inputTmp[i] = dict.find_first_of(to_decrypt_str[current_pos + i]);
            }
            translatedInput[0] = (char)((inputTmp[0] << 2) + ((inputTmp[1] & 48) >> 4));
            translatedInput[1] = (char)(((inputTmp[1] & 15) << 4) + ((inputTmp[2] & 60) >> 2));
            translatedInput[2] = (char)(((inputTmp[2] & 3) << 6) + inputTmp[3]);
            for(int j = 0 ; j < 3 ; j++){
                if(inputTmp[j + 1] == 64) break;
                decrypt_result->write(translatedInput[j]);
            }
        }
        return decrypt_result;
    }
    ByteData * decrypt_and_uncompress(string to_process){
        ByteData * decrypted = decrypt(to_process);
        ByteData * tmp = nullptr;
        for(int i = 0 ; i < compress_times ; i++){
            tmp = zlib_uncompress(decrypted);
            delete decrypted;
            decrypted = tmp;
        }
        return decrypted;
    }
    string recompress_and_encrypt(ByteData * to_process){
        ByteData * compressed = to_process;
        ByteData * tmp = nullptr;
        for(int i = 0 ; i < compress_times ; i++){
            tmp = zlib_compress(compressed);
            delete compressed;
            compressed = tmp;
        }
        string encrypted_str = encrypt(compressed);
        return encrypted_str;
    }
}

namespace web_crack{
    using namespace crack;
    const int operation_times[6] = {2, 1, 10, 10, 10, 1};

    ByteData * web_decrypt_and_uncompress(ByteData * encrypted, int uncompress_times){
        //解密
        encrypted = decrypt(string(encrypted->toString()));
        //解压缩
        ByteData * tmp = nullptr;
        for(int i = 0 ; i < uncompress_times ; i++){
            tmp = zlib_uncompress(encrypted);
            delete encrypted;
            encrypted = tmp;
        }
        return encrypted;
    }
    ByteData * web_compress_and_encrypt(ByteData * decrypted, int compress_times){
        //压缩
        ByteData * tmp = nullptr;
        for(int i = 0 ; i < compress_times ; i++){
            tmp = zlib_compress(decrypted);
            delete decrypted;
            decrypted = tmp;
        }
        //加密
        decrypted = new ByteData(encrypt(decrypted));
        return decrypted;
    }
    ByteData * web_decrypt_and_uncompress(string to_process){
        auto * tmp = new ByteData(to_process);
        for(int operation_time : operation_times){
            tmp = web_decrypt_and_uncompress(tmp, operation_time);
        }
        return tmp;
    }
    string web_recompress_and_encrypt(ByteData * to_process){
        auto * tmp = to_process;
        for (int i = 5; i >= 0; i--) {
            tmp = web_compress_and_encrypt(tmp, operation_times[i]);
        }
        return tmp->toString();
    }
}

namespace file_util{

    void saveByteFile(char * fileName, ByteData * toSave){
        ofstream fout(fileName, ios::out | ios::binary);
        fout.write(toSave->getData(),toSave->getByteLen());
    }
    void saveTextFile(string fileName, string toSave){
        ofstream fout(fileName);
        fout << toSave;
    }
    void saveTextFile(char * fileName, string toSave){
        ofstream fout(fileName);
        fout << toSave;
    }
    void saveAsXmlFile(char * fileName, ByteData * toSave){
        string tmpOutName = fileName;
        tmpOutName += ".xml";
        ofstream fout(tmpOutName,ios::out | ios::binary);
        fout.write(toSave->getData(),toSave->getByteLen());
    }
    void saveAsXmlFile(char * fileName, string toSave){
        string tmpOutName = fileName;
        tmpOutName += ".xml";
        ofstream fout(tmpOutName);
        fout << toSave;
    }

    string importFile(char * fileName){
        ifstream fin(fileName,ios::in);
        string saved_game;
        fin >> saved_game;
        return saved_game;
    }
    ByteData * importXmlFile(char * fileName){
        //get standard xml file
        ifstream fin(fileName,ios::in | ios::binary);
        ByteData * fdata = new ByteData();
        for(char tmp; fin.read( (char *) &tmp, sizeof(char));){
            fdata -> write(tmp);
        }
        //convert to amf3 stream
        char * amf3data = new char[fdata->max_len];
        amf3writer writer(amf3data);
        writer.Write(fdata->toString());
        //convert to ByteData object
        ByteData * fdata_amf3 = new ByteData(amf3data,fdata->max_len,writer.position);
        return fdata_amf3;
    }
}

namespace save_hacker{
    void hackIntoXml(char * file_name){
        string saved_game = file_util::importFile(file_name);
        ByteData * byte_xml = crack::decrypt_and_uncompress(saved_game);
        amf3parser parser(byte_xml->data);
        amf3object obj_str_xml =  parser.ReadNextObject();
        file_util::saveAsXmlFile(file_name,obj_str_xml.text);
    }
    void xmlBackToGameSave(char * file_name){
        ByteData * byte_xml = file_util::importXmlFile(file_name);
        string to_save = crack::recompress_and_encrypt(byte_xml);
        string file_str = file_name;
        string str_name = file_str.substr(0,file_str.find(".xml"));
        file_util::saveTextFile(str_name, to_save);
    }
    void web_hackIntoXml(char * file_name){
        string saved_game = file_util::importFile(file_name);
        ByteData * byte_xml = web_crack::web_decrypt_and_uncompress(saved_game);
        amf3parser parser(byte_xml->data);
        amf3object obj_str_xml =  parser.ReadNextObject();
        file_util::saveAsXmlFile(file_name,obj_str_xml.text);
    }
    void web_xmlBackToGameSave(char * file_name){
        ByteData * byte_xml = file_util::importXmlFile(file_name);
        string to_save = web_crack::web_recompress_and_encrypt(byte_xml);
        string file_str = file_name;
        string str_name = file_str.substr(0,file_str.find(".xml"));
        file_util::saveTextFile(str_name, to_save);
    }
    enum Mode{
        TO_XML, TO_NORMAL
    };
    enum DocType{
        LOCAL,WEB
    };
}

int main(int argc, char * argv[]) {
    using namespace save_hacker;
    if(argc == 1){
        cout << "[ZaoMeng 3 Save Converter]\nWrong Usage. CorrectUsage: [Main Program].exe [save file][s]" << endl;
        cout << "正确使用方法：\n1.打开Windows PowerShell 或 Command Prompt \n"
                "2.拖拽主程序至PowerShell或Command Prompt窗口\n"
                "3.重复该步骤有限次：输入一个空格，再拖拽一个需要处理的游戏存档文件至PowerShell或Command Prompt窗口\n"
                "你将可以对有限个造梦西游3游戏存档进行处理\n";
        system("pause");
    }else{
        cout << "> Specify Manipulation Type: 选择操作类型 \n0) Local 本地单机游戏 \n1) Web网页版游戏\nPlease Choose:";
        bool type_tmp;
        cin >> type_tmp;
        cout << "> Pick up a mode 选择模式: \n0) Convert ZaoMeng Standard Save Into Xml Doc. \n   转换存档为xml文档 \n1) Convert Given Xml Doc Back to Standard ZaoMeng Save\n   重新将xml文档转换回游戏存档\nPlease Choose:";
        bool mode_tmp;
        cin >> mode_tmp;
        for(int i = 1 ; i < argc; i++){
            if( (DocType) type_tmp == LOCAL){
                if( (Mode) mode_tmp == TO_XML) hackIntoXml(argv[i]);
                if( (Mode) mode_tmp == TO_NORMAL) xmlBackToGameSave(argv[i]);
            }
            if( (DocType) type_tmp == WEB){
                if( (Mode) mode_tmp == TO_XML) web_hackIntoXml(argv[i]);
                if( (Mode) mode_tmp == TO_NORMAL) web_xmlBackToGameSave(argv[i]);
            }
            cout << "Complete: " << argv[i] << endl;
        }
    }
    return 0;
}
