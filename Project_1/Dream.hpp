#pragma  once

#include <iostream>
#include <string>
#include <json/json.h>
#include <sstream>
#include <unistd.h>
#include <memory>
#include <unistd.h>
#include <stdlib.h>
#include <map>
#include <unordered_map>
#include <speech.h>
#include "speech.h"
#include "base/http.h"





#define ASR_PATH "temp_file/asr.wav" //语音识别Automatic Speech Recognition //将语音识别的文件放在这个目录里面。
#define CMD_ETC "command.etc"
#define TTS_PATH "temp_file/tts.mp3"

class Util//工具方法类
{
	public:
		static bool Exec(std::string command, bool is_print) //第二个参数是判断是否需要打印
		{
			if(!is_print)
			{
				command += ">/dev/null 2>&1"; //将标准输出和标准错误全都放进垃圾箱中。
			}

			FILE *fp = popen(command.c_str(),"r");
			if(nullptr == fp)
			{
				std::cerr << "popen exec \'" << command << "\' Error" << std::endl;
				return false;
			}
			
			//如果想打印的话就执行下面的代码
			if(is_print)
			{
		   	    char ch;
		   	    while(fread(&ch , 1 , 1 , fp) > 0)
		   	    {
		   	    	fwrite(&ch , 1 , 1 ,stdout);
		   	    }
			}

			pclose(fp);
			return true;
		}
};

class Robot//图灵机器人交互式类 
{
private:
     std::string url;//接口地址
     std::string api_key;//机器人唯一标识
     std::string user_id;//用户唯一标识
     aip::HttpClient client;//使用百度语音识别自带的客户端进行识别。
private:
	bool IsCodeLegal(int code)
	{
		bool result = false;
		switch(code)
		{
			case 5000:
				break;
			case 10004:
				result = true;
				break;
			default:
				result = true;
				break;
		}
		return result;
	}
	std::string MessageToJson(std::string &message)
	{
		Json::Value root;
		Json::StreamWriterBuilder yj;
		std::ostringstream os;
		
		Json::Value item_;
		item_["text"] = message;

		Json::Value item;
		item["inputText"] = item_;

		root["reqType"] = 0;//0表示注释是文本
		root["perception"] = item;//传入的信息

		item.clear();
		item["apiKey"] = api_key;
		item["userId"] = user_id;
		root["userInfo"] = item;

		std::unique_ptr<Json::StreamWriter> sw(yj.newStreamWriter());
		sw->write(root,&os);
		std::string json_string = os.str();
		//std::cout << "debug " << json_string << std::endl;
		return json_string;
	}

	std::string RequestTL(std::string &request_json)
	{
		std::string response;
		int status_code = client.post(url,nullptr, request_json, nullptr, &response);//用http post进行请求,要严格按照官网上的要求进行填写。
		//判断请求成功与否
		if(status_code != CURLcode::CURLE_OK)
		{
			std::cerr << "post error" << std::endl;
			return  "";
		}
		return response;

	}

	std::string JsonToEchoMessage(std::string &str)
	{
		//std::cout << "JsonToEchoMessage" << str << std::endl;
		//进行反序列化的代码实现
		JSONCPP_STRING errs;
		Json::Value root;
		Json::CharReaderBuilder rb;
		std::unique_ptr<Json::CharReader> const cr(rb.newCharReader());

		bool res = cr->parse(str.data(),str.data() + str.size(),&root,&errs);
		if(!res || !errs.empty())
		{
			std::cerr << "parse error!" << std::endl;
			return "";
		}

		int code = root["intent"]["code"].asInt();
		if(!IsCodeLegal(code))
		{
			std::cerr << "parse error!" << std::endl;
			return "";
		}

		Json::Value item = root["results"][0];
		std::string message = item["values"]["text"].asString();
		return message;
	}

public:
	Robot(std::string id = "1")
	{
		
		this->url = "http://openapi.tuling123.com/openapi/api/v2";//接口地址
		this->api_key = "91fca620a42446338272bb9c37b49db0";//机器人唯一标识
		this->user_id = id;//用户唯一标识
	}
	
	std::string Talk(std::string message)//是一种轻量级的数据交换格式，机器语言和人类识别的语言之间的交换
	{
		std::string json = MessageToJson(message);//消息转换成机器语言。
		std::string response = RequestTL(json);//内核进行语言处理，将返回机器语言。
		std::string echo_message = JsonToEchoMessage(response);//机器语言转化为人类所能识别的语言。
		return echo_message;
	}

	~Robot()
	{
			
	}
};

class SpeechRec//访问百度语音识别类 
{
	private:
		//请填写appid，请填写Api key，请填写Secret Key。
		std::string app_id;
		std::string api_key;
		std::string secret_key;
		aip::Speech *client;
	private:
		bool IsCodeLegal(int code)
		{
			bool result = false;
			switch(code)
			{
				case 3300:
					std::cout << "用户输入的参数不正确" << std::endl;
					break;
				case 3301:
					std::cout << "用户输入错误，音频质量过差" <<std::endl;
					break;
				case 3303:
					std::cout << "语音服务器后端的问题" <<std::endl;
				default:
					result = true;
					break;
			}
			return result;
		}
	public:
		SpeechRec()
		{
			app_id = "16868339";
			api_key = "1c6E8kK1GrGG7ZwGjvn0vFur";
			secret_key = "2t1MPk56umhUmi95IlMU57Rm1POxVpIG";
			client = new aip::Speech(app_id,api_key,secret_key);
		}

		bool ASR(std::string path, std::string &out)//进行语音识别
		{
			std::map<std::string,std::string> options;
			options["dev_pid"] = "1536";
			std::string file_content;
			aip::get_file_content(ASR_PATH, &file_content);
			Json::Value result = client->recognize(file_content, "wav", 16000, options);
			//std::cout << "debug: " << result.toStyledString() << std::endl;
			
			int code = result["err_no"].asInt();
			if(!IsCodeLegal(code))
			{
				std::cerr << "recognize error" << std::endl;
				return false;
			}

			out = result["result"][0].asString();
			return true;
		}
		
		bool  TTS(std::string message)//语音合成
		{
			bool ret;
			std::ofstream ofile;
			std::string ret_file;
			std::map<std::string ,std::string> options;
			options["spd"] = "7";//语速0 - 15
			options["pit"] = "7";//音调0 - 15
			options["vol"] = "7"; //音量0 - 15
			options["per"] = "111";//1 0 3 4 度博文=106，度小童=110，度小萌=111，度米朵=103，度小娇=5
			options["aue"] = "6";
			ofile.open(TTS_PATH, std::ios::out | std::ios::binary);

			Json::Value result = client->text2audio(message, options, ret_file);
			if(!ret_file.empty())
			{
				ofile << ret_file;
				ret = true;
			}
			else
			{
				std::cerr << result.toStyledString() << std::endl;
				ret = false;
			}
			ofile.close();
			return ret;

		}

		~SpeechRec()
		{
			
		}
};

class Dream//整合资源
{
	private:
		Robot rt;
		SpeechRec sr;
		std::unordered_map<std::string , std::string> commands;
	private:
		bool Record()//录音的功能
		{
			std::cout << "debug: " << "Record ,,,," << std::endl;
			std::string command = "arecord -t wav -c 1 -r 16000 -d 3 -f S16_LE ";
			command += ASR_PATH;
			bool ret = Util::Exec(command,false);
			std::cout << "debug: Record ... done" << std::endl;
			return ret;
		}

		bool Play()//播放的功能
		{
			std::string command = "cvlc --play-and-exit ";
			command += TTS_PATH;
			bool ret = Util::Exec(command,false);
			return ret;
		}

	public:
		Dream()
		{
			
		}

		bool LoadEtc()
		{
			std::ifstream in(CMD_ETC);
			if(!in.is_open())
			{
				std::cerr << "open error" << std::endl;
				return false;
			}
			
			std::string sep = ":";
			char line[256];
			while(in.getline(line,sizeof(line)))
			{
				std::string str = line;
				std::size_t pos = str.find(sep);
				if(std::string::npos == pos)
				{
					std::cerr << "not find :" << std::endl;
					continue;
				}

				std::string k = str.substr(0, pos);
				std::string v = str.substr(pos + sep.size());
				
				k += "。";
				commands.insert(std::make_pair(k,v));
			
			}
			in.close();
			return true;
		}
	
		
		bool IsCommand(std::string message, std::string &cmd)
		{
			auto iter = commands.find(message);
			if(iter == commands.end())
			{
				return false;
			}
			

		}
		void Run()
		{
			volatile bool quit = false;
			
			while(!quit)
			{
				if(this->Record())
				{
					std::string message;
					if(sr.ASR(ASR_PATH, message))
					{
						std::string cmd = "";
						//1.先判断是否是用户自定义的命令。
						if(IsCommand(message, cmd))
						{
							std::cout << "[Dream@loaclhost]$ " << cmd  << std::endl;
							Util::Exec(cmd,true);
							continue;
						}
						
						if(message == "你走吧。")
						{
							std::string quit_message = "那我就走了。";

							if(sr.TTS(quit_message))
							std::cout << "Dream机器人# " << quit_message << std::endl; 
							{
								this->Play();	
							}
							exit(0);
						}
						//2.将识别的消息交给图灵机器人。
						std::cout << "我# " << message << std::endl;
						std:: string echo = rt.Talk(message);
						if(sr.TTS(echo))
						std::cout << "Dream机器人# " << echo << std::endl; 
						{
							this->Play();	
						}
					}
					else
					{
						std::cerr << "Recognize error... " << std::endl; 
					}
				}
				else
				{
					std::cerr << "Record error..." <<std::endl;
				}
				//sleep(1);
			}
		}

		~Dream()
		{
			
		}

};
