#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/connpool/connect_manager.hpp"

namespace acl
{

/**
 * HTTP 客户端请求连接池管理类
 */
class ACL_CPP_API http_request_manager : public acl::connect_manager
{
public:
	/**
	 * 构造函数
	 * @param conn_timeout {int} 连接超时时间(秒)
	 * @param rw_timeout {int} 网络 IO 读写超时时间(秒)
	 */
	http_request_manager(int conn_timeout = 30, int rw_timeout = 30);
	virtual ~http_request_manager();

protected:
	/**
	 * 基类纯虚函数，用来创建连接池对象
	 * @param addr {const char*} 服务器监听地址，格式：ip:port
	 * @param count {size_t} 连接池的大小限制，当该值为 0 时则没有限制
	 * @param idx {size_t} 该连接池对象在集合中的下标位置(从 0 开始)
	 * @return {connect_pool*} 返回创建的连接池对象
	 */
	connect_pool* create_pool(const char* addr, size_t count, size_t idx);

private:
	int conn_timeout_;
	int rw_timeout_;
};

} // namespace acl
