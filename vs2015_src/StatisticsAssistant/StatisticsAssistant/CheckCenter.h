#ifndef check_center_h
#define check_center_h
#include <QObject>





class CheckCenter : public QObject
{
	Q_OBJECT

public:
	CheckCenter();
	~CheckCenter();
	
	/// ----------------------------------------------------------------------------
	/// @brief：		返回单件
	/// @return: 	check_center& - 无
	/// 			
	/// ----------------------------------------------------------------------------
	static CheckCenter& get_ins();


	/// ----------------------------------------------------------------------------
	/// @brief：		计算pdata的字节校验
	/// @param: 	const unsigned char * pdata - pdata
	/// @param: 	const unsigned int length - length
	/// @return: 	unsigned char - 校验结果
	/// 			
	/// ----------------------------------------------------------------------------
	unsigned int sum_(const unsigned char* pdata, const unsigned int length);

	/// ----------------------------------------------------------------------------
	/// @brief：		算CRC校验
	/// @param: 	const unsigned short init_value - CRC初始值，默认为0. 
	/// @param: 	const unsigned char * pdata - 待计算数据
	/// @param: 	const unsigned int data_len - 待计算数据长度
	/// @return: 	unsigned short 
	/// 			 CRC 结果
	/// ----------------------------------------------------------------------------
	unsigned short crc_(const unsigned short init_value, const unsigned char* pdata, unsigned int data_len);

	/// ----------------------------------------------------------------------------
	/// @brief：		qstring 转 std::string
	/// @param: 	const QString & str - str
	/// @return: 	std::string - 无
	/// 			
	/// ----------------------------------------------------------------------------
	std::string qstr_to_std_str_(const QString& str);

	
	/// ----------------------------------------------------------------------------
	/// @brief：		检查指针是否有效
	/// @param: 	check_type * ppppp - ppppp
	/// @return: 	bool - 无
	/// 			
	/// ----------------------------------------------------------------------------
	template<typename check_type>
	bool pointer_is_valid_(check_type * ppppp)
	{
		if ((nullptr == ppppp) || (NULL == ppppp))
			return false;

		return true;
	}

	/// ----------------------------------------------------------------------------
	/// @brief：		将str_src中的空格移除
	/// @param: 	const QString & str_src - 例如： AA 55 33 DD FF EE
	/// @param: 	unsigned char * pout_buf - pout_buf
	/// @param: 	const int pbuf_len - pbuf_len
	/// @return: 	int 
	///				-1 - 失败， str_src为空
	///				-2 - 失败， pout_buf无效 或 pbuf_len为0
	///				-3 - 失败， str_src转换后的的长度大于poutbuf
	/// 			X > 0 - 成功，X为转为数组的长度
	/// ----------------------------------------------------------------------------
	int qstr_to_send_buf_(const QString& str_src, unsigned char* pout_buf, const unsigned int pbuf_len);

	/// ----------------------------------------------------------------------------
	/// @brief：		pbuf转qstring
	/// @param: 	const unsigned char * pout_buf - pout_buf
	/// @param: 	const unsigned int pbuf_len - pbuf_len
	/// @return: 	QT_NAMESPACE::QString - 无
	/// 			
	/// ----------------------------------------------------------------------------
	QString uchar_to_qstr_(const unsigned char* pout_buf, const unsigned int pbuf_len);

	/// ----------------------------------------------------------------------------
	/// @brief：		计算strsrc（AA 55）的长度, 默认使用空格作为分隔符
	/// @param: 	const QString & str_src - str_src
	/// @return: 	unsigned int - 长度
	/// 			
	/// ----------------------------------------------------------------------------
	unsigned int get_send_buf_len_(const QString& str_src);


	/// ----------------------------------------------------------------------------
	/// @brief：		str_src中可能有大于2或小于2的，全部改为2字符
	/// @param: 	QString str_src - str_src
	/// @return: 	QT_NAMESPACE::QString - 无
	/// 			
	/// ----------------------------------------------------------------------------
	QString get_real_show_str_(const QString& str_src);


};


#endif /// check_center_h
