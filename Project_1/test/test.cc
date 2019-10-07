#include <iostream>
#include <json/json.h>
#include <string>
#include <sstream>
#include <memory>


using namespace std;

int main()
{
	//string cmd = "ls -a -l ";
	string cmd = "arecord -t wav -c 1 -r 16000 -d 5 -f S16_LE demo.wav";
	FILE *fp = popen(cmd.c_str(),"r");
	if(nullptr == fp)
	{
		perror("popen");
		return 1;
	}

	char c;
	while(fread(&c,1,1,fp) > 0)//fread 参数1你从哪里读，2你读的基本单位是多少，3你要读多少个基本单位，4从哪个流当中读数据。
	{
		fwrite(&c,1,1,stdout);
	}

	pclose(fp);
	return 0;
}
//
//void JsonParse(string &s)
//{
//	JSONCPP_STRING errs;
//	Json::Value root;
//	Json::CharReaderBuilder rb;
//	unique_ptr<Json::CharReader> cr(rb.newCharReader());
//	
//	bool res = cr->parse(s.data(),s.data() + s.size(), &root, &errs);
//	if(!res || !errs.empty())
//	{
//		cerr << "parse error!" << endl;
//		return ;
//	}
//
//	cout << "Age: " << root["Age"].asInt() << endl;
//	cout << "Lang: " << root["Lang"].asString() << endl;
//	cout << "Name: " << root["Name"].asString() << endl;
//
//	
//}
//int main()
//{
//	string s = "{\"Age\" : 26,\"Lang\" : \"c++\",\"Name\" : \"zhangsan\"}"; //“要进行转义。\"
//	
//	JsonParse(s);
//	return 0;
//}
//int main()
//{
	//Json::Value root;
	//Json::StreamWriterBuilder yj;
	//ostringstream os;
	//
	//Json::Value item;//Json里面的字符串里可以套字符串。
	//item["school"] = "qinghua";
	//item["high"] = "172.0f";

	//root["nick_name"] = "hello";
	//root["date"] = "2019-7-24/4/5/6";
	//root["message"] = "你好";
	//root["age"] = 12;
	//root["info"][0] = item;
	//root["info"][1] = item;
	//root["info"][2] = item;

	//std::unique_ptr<Json::StreamWriter> sw(yj.newStreamWriter());
	//sw->write(root,&os);

	//string s = os.str();
	//cout << s << endl;

	//return 0;
//}

