// http_servlet.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

using namespace acl;

//////////////////////////////////////////////////////////////////////////

class http_servlet : public HttpServlet
{
public:
	http_servlet(socket_stream* stream, session* session)
		: HttpServlet(stream, session)
	{

	}

	~http_servlet(void)
	{

	}

	virtual bool doOther(HttpServletRequest&, HttpServletResponse& res,
		const char* method)
	{
		res.setStatus(400);
		res.setContentType("text/xml; charset=gb2312");
		// 发送 http 响应头
		if (res.sendHeader() == false)
			return false;
		// 发送 http 响应体
		string buf;
		buf.format("<root error='unkown method: %s' />\r\n", method);
		(void) res.getOutputStream().write(buf);
		return false;
	}

	virtual bool doGet(HttpServletRequest& req, HttpServletResponse& res)
	{
		return doPost(req, res);
	}

	virtual bool doPost(HttpServletRequest& req, HttpServletResponse& res)
	{
		const char* sid = req.getSession().getAttribute("sid");
		if (*sid == 0)
			req.getSession().setAttribute("sid", "xxxxxx");
		sid = req.getSession().getAttribute("sid");

		const char* cookie1 = req.getCookieValue("name1");
		const char* cookie2 = req.getCookieValue("name2");

		// 创建 HTTP 响应头
                res.addCookie("name1", "value1");
		res.addCookie("name2", "value2", ".test.com", "/", 3600 * 24);
//		res.setStatus(400);  // 可以设置返回的状态码

		// 两种方式都可以设置字符集
		if (0)
			res.setContentType("text/xml; charset=gb2312");
		else
		{
			res.setContentType("text/xml");
			res.setCharacterEncoding("gb2312");
		}

		const char* param1 = req.getParameter("name1");
		const char* param2 = req.getParameter("name2");

		// 创建 xml 格式的数据体
		xml body;
		body.get_root().add_child("root", true)
			.add_child("sessions", true)
				.add_child("session", true)
					.add_attr("sid", sid ? sid : "null")
					.get_parent()
				.get_parent()
			.add_child("cookies", true)
				.add_child("cookie", true)
					.add_attr("name1", cookie1 ? cookie1 : "null")
					.get_parent()
				.add_child("cookie", true)
					.add_attr("name2", cookie2 ? cookie2 : "null")
					.get_parent()
				.get_parent()
			.add_child("params", true)
				.add_child("param", true)
					.add_attr("name1", param1 ? param1 : "null")
					.get_parent()
				.add_child("param", true)
					.add_attr("name2", param2 ? param2 : "null");
		string buf;
		body.build_xml(buf);

		// 发送 http 响应头
		if (res.sendHeader() == false)
			return false;
		// 发送 http 响应体
		if (res.getOutputStream().write(buf) == -1)
			return false;
		return true;
	}
protected:
private:
};

//////////////////////////////////////////////////////////////////////////

static void do_run(socket_stream* stream)
{
	memcache_session session("127.0.0.1:11211");
	http_servlet servlet(stream, &session);
	servlet.setLocalCharset("gb2312");
	servlet.doRun();
}

// 服务器方式运行时的服务类
class master_service : public master_proc
{
public:
	master_service() {}
	~master_service() {}
protected:
	virtual void on_accept(socket_stream* stream)
	{
		do_run(stream);
	}
};

// WEB 服务模式
static void do_alone(void)
{
	master_service service;
	printf("listen: 0.0.0.0:8888 ...\r\n");
	service.run_alone("0.0.0.0:8888", NULL, 0);  // 单独运行方式
}

// WEB CGI 模式
static void do_cgi(void)
{
	do_run(NULL);
}

int main(int argc, char* argv[])
{
#ifdef WIN32
	acl::acl_cpp_init();
#endif

	// 开始运行
	if (argc >= 2 && strcmp(argv[1], "alone") == 0)
		do_alone();
	else
		do_cgi();

	return 0;
}

