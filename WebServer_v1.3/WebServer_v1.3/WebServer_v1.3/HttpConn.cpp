#include "HttpConn.h"

HttpConn::HttpConn(int fd1, sockaddr_in addr1)
	:fd(fd1), addr(addr1){}


int HttpConn::ReadRequest() 
{
	int len = 0, total = 0;
	char tmp[1024] = { 0 };//临时接收数据，然后放入buf中
	char buf[4096] = { 0 };
	while ((len = recv(fd, tmp, sizeof(tmp), 0)) > 0) {
		if (total + len < sizeof(buf)) {
			memcpy(buf + total, tmp, len);
		}
		total += len;
	}
	if ((len == -1) && (errno == EAGAIN))
	{
		//对应缓冲区的数据被读取完了
		ParseRequest(buf);  //解析请求
		printf("success!\n");
		ProcessRequest();   //处理请求
	}
	return len;
}

void HttpConn::ParseRequest(char* buf)
{
	//读取首行
	char* p = strchr(buf, '\n');
	int indexRequest = (p - buf);
	char Request[indexRequest];
	strncpy(Request, buf, indexRequest);
	//printf("RequestFirstLine is [%s ]\n", Request);
	//解析请求    GET /a.txt     HTTP/1.1\r\n
	printf("read ok\n");
	char method[256] = "";
	char content[256] = "";
	char protocol[256] = "";
	sscanf(buf, "%[^ ] %[^ ] %[^ \r\n]", method, content, protocol);
	//printf("method :%s \n", method);
	//printf("content :%s \n", content);
	//printf("protocol :%s \n", protocol);
	httpRequest.METHOD = method;
	httpRequest.CONTENT = content;
	httpRequest.PROTOCOL = protocol;

}

void HttpConn::ProcessRequest() 
{
	if (strcasecmp(httpRequest.METHOD.c_str(), "get") == 0) //判断是否是get请求
	{
		//1、设置路径 ？
		std::string fileStr = httpRequest.CONTENT.substr(httpRequest.CONTENT.find('/') + 1);
		//std::cout << "fileStr:" << "[" << fileStr << "]" << std::endl;
		if (fileStr.c_str() == 0)
		{
			fileStr = "./";
		}
		//2、判断file是目录还是文件
		struct stat st;
		int ret = stat(fileStr.c_str(), &st);
		if (ret < 0)
		{
			printf("file not exit!\n");
			SendHeader(404, "NOT Found", GetFileType(".html"));
			SendFile("error.html");
			return;
		}
		if (S_ISDIR(st.st_mode))
		{
			//是目录
			//printf("[DIR:]\n");
			//SendHeader(200, "OK", getFileType(".html"));
			//SendDir(fileStr.c_str());
		}
		else if (S_ISREG(st.st_mode))
		{
			//是文件
			//printf("[Request File]\n");
			SendHeader(200, "OK", GetFileType(".html"));
			SendFile(fileStr.c_str());
			printf("success!\n");
		}
	}
}

void HttpConn::SendHeader(int statusCode, const char* descr, const char* type) 
{
	std::string buf1;

	buf1 = "HTTP/1.1 " + std::to_string(statusCode) + " " + descr + "\r\n";
	send(fd, buf1.c_str(), buf1.length(), 0);

	buf1 = "Content-Type:" + (std::string)type + "\r\n";
	send(fd, buf1.c_str(), buf1.length(), 0);
	//发送空行
	send(fd, "\r\n", 2, 0);
}

int HttpConn::SendFile(const char* filename)
{
	int fdFile = open(filename, O_RDONLY);

	int flag_send = 0;
	if (fdFile < 0)
	{
		perror("");
		return -1;
	}
	char buf[2048] = "";
	int len = 0;
	while (1)
	{

		len = read(fdFile, buf, sizeof(buf));
		if (len < 0)
		{
			perror("read");
			break;
		}
		else if (len == 0)
		{
			break;
		}
		else
		{
			flag_send = send(fd, buf, len, 0);
			if (flag_send < 0)
			{
				printf("sendfile error ");
			}
		}
	}
	close(fdFile);
	close(fd);
	return 0;
}

int HttpConn::GetFd()
{
	return fd;
}

void HttpConn::CloseFd()
{
	close(fd);
}

const char* HttpConn::GetFileType(const char* name)
{
	// 从右往左找，找后缀名
	const char* dot = strrchr(name, '.');
	if (dot == NULL) {
		return "text/plain; charset=utf-8";
	}
	if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0) {
		return "text/html; charset=utf-8\r\n";
	}
	if (strcmp(dot, ".txt") == 0) {
		return "text/plain; charset=utf-8";
	}
	if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0) {
		return "image/jpeg";
	}
	if (strcmp(dot, ".png") == 0) {
		return "image/png";
	}
	if (strcmp(dot, ".gif") == 0) {
		return "image/gif";
	}
	if (strcmp(dot, ".mkv") == 0) {
		return "video/x-matroska";
	}
	if (strcmp(dot, ".mp4") == 0) {
		return "audio/mp4";
	}
	if (strcmp(dot, ".mp3") == 0) {
		return "audio/mp3";
	}
	if (strcmp(dot, ".csv") == 0) {
		return "text/csv";
	}
	if (strcmp(dot, ".wbmp") == 0) {
		return "image/vnd.wap.wbmp";
	}
	return "text/plain; charset=utf-8";

}

