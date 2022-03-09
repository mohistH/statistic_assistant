#pragma once
#include "universe_def.h"


// to definite an export flag
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32)
//---------------------------------------------------------------------------------------------
	#ifndef _lib_sp_api_
		#define _lib_sp_api_	__declspec(dllexport)
	#else
		#define _lib_sp_api_	__declspec(dllimport)
	#endif /// !_lib_pipe_api_

#elif defined(_unix_)  || defined(_unix) || defined(_linux_)|| defined(__linux) || defined(__APPLE__)
//---------------------------------------------------------------------------------------------

	#ifndef _lib_sp_api_
		#define _lib_sp_api_	__attribute__((visibility ("default")))
	#endif /// !_lib_pipe_api_
#endif /// 



// to check cxx11
#ifdef has_cxx_11
#include <tuple>
#include <string>
#include <list>

// lib_sp = lib_serial port
namespace lib_sp
{

	/// ----------------------------------------------------------------------------
	/// @brief: to output serial port information
	/// ----------------------------------------------------------------------------
	struct serial_port_available_
	{
		/// 串口
		std::string			_name;
		/// des
		std::string			_description;

		void zero()	noexcept
		{
			_name = { "" };
			_description = { "" };
		}

		serial_port_available_() noexcept
		{
			zero();
		}
	};
	// to output serial port information
	using sp_name_desc = serial_port_available_;

	// the set of available serial port
	using list_sp_name = std::list< sp_name_desc>;



	/// ----------------------------------------------------------------------------
	/// @brief: the mode of operating serial port, includeing reading and writting
	/// ----------------------------------------------------------------------------
	enum operator_mode
	{
		// Asynchronous
		mode_async,

		// Synchronous
		mode_sync,
	};

	/// ----------------------------------------------------------------------------
	/// @brief: the baud rate enum
	/// ----------------------------------------------------------------------------
	enum baud_rate
	{
		baud_rate_110 = 110,
		baud_rate_300 = 300,
		baud_rate_600 = 600,
		baud_rate_1200 = 1200,
		baud_rate_2400 = 2400,
		baud_rate_4800 = 4800,
		baud_rate_9600 = 9600,
		baud_rate_14400 = 14400,
		baud_rate_19200 = 19200,
		baud_rate_38400 = 38400,
		baud_rate_56000 = 56000,
		baud_rate_57600 = 57600,
		baud_rate_115200 = 115200,
		baud_rate_921600 = 921600
	};

	/// ----------------------------------------------------------------------------
	/// @brief:  the data bits enum
	/// ----------------------------------------------------------------------------
	enum data_bits
	{
		data_bits_5 = 5,
		data_bits_6 = 6,
		data_bits_7 = 7,
		data_bits_8 = 8,
	};

	/// ----------------------------------------------------------------------------
	/// @brief: the stop bits enum
	/// ----------------------------------------------------------------------------
	enum stop_bits
	{
		// 1
		stop_1 = 0,

		// 1.5
		stop_1_5 = 1,

		// 2
		stop_2 = 2,
	};

	/// ----------------------------------------------------------------------------
	/// @brief: 流控制
	/// ----------------------------------------------------------------------------
	enum flow_control
	{
		// No flow control
		flow_ctl_none = 0,

		// Hardware(RTS / CTS) flow control 
		flow_ctl_hardware = 1,

		// Software(XON / XOFF) flow control 
		flow_ctl_software = 2
	};

	/// ----------------------------------------------------------------------------
	/// @brief: 校验位
	/// ----------------------------------------------------------------------------
	enum parity
	{
		// No Parity
		parity_none = 0,

		// Odd Parity
		parity_odd = 1,

		// Even Parity 
		parity_even = 2,

#ifdef os_is_win
		// Mark Parity
		parity_mark = 3,
#endif // !os_is_win

		// Space Parity
		parity_space = 4,
	};


	/// ----------------------------------------------------------------------------
	/// @brief: 初始化串口结构体
	/// ----------------------------------------------------------------------------
	struct serial_port_prop_
	{
		// win:COM1, 
		std::string		_name;

		// baud
		baud_rate		_baud_rate;

		// parity
		parity			_parity;

		// data bits
		data_bits		_data_bits;

		// stop bits
		stop_bits		_stop_bits;

		// flow control
		flow_control	_flow_ctl;

		// operate mode
		operator_mode	_op_mode;

		/// 接收数据相邻字节之间的间隔， 单位，ms
		unsigned int read_interval_timeout_;

		void zero_() noexcept
		{
			_name = { "" };
			_baud_rate = { baud_rate_9600 };
			_parity = parity_even;
			_data_bits = data_bits_8;
			_stop_bits = stop_1;
			_flow_ctl = flow_ctl_none;
			_op_mode = mode_async;
			read_interval_timeout_	= 5;
		}

		serial_port_prop_() noexcept
		{
			zero_();
		}
	};

	// serialprot properties
	using sp_prop = serial_port_prop_;


	/// ----------------------------------------------------------------------------
	/// @brief: to recv data, you should inherit this class to get recv data
	/// ----------------------------------------------------------------------------
	class irecv_data
	{
	public:

			
		/// ----------------------------------------------------------------------------
		/// @brief：		接收数据
		/// @param: 	const char * precv_data - 收到的数据
		/// @param: 	const unsigned int len_recv_data - 数据长度
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		virtual void on_recv_data(const char* precv_data, const unsigned int len_recv_data) noexcept = 0;
	};

	/// ----------------------------------------------------------------------------
	/// @brief: 串口接口类
	/// ----------------------------------------------------------------------------
	class iserial_port
	{
	public:

		/// ----------------------------------------------------------------------------
		/// @ brief: to initialize serial port's parameters
		/// @ spp - the properties of serial port
		/// @ precv_data - the object inherited irecv_data class is to recv data
		/// 			nullptr - doesnt recv data
		/// @ return - int
		/// 	0 - success
		/// 	-1 - failure, the spp.name is null
		/// 	-2 - failure, the spp._baud_rate is unknow
		/// 	-3 - failure, the spp._stop_bits is unknow
		/// 	-4 - failure, the spp._data_bits is unknow
		/// 	-5 - failure, the spp._flow_ctl is unknow
		/// 	-6 - failure, the spp._parity is unknow
		/// ----------------------------------------------------------------------------
		virtual int init_(const sp_prop& spp, irecv_data* precv_data = nullptr) = 0;

		/// ----------------------------------------------------------------------------
		///	@ brief: to open serial port
		///	@ return - int
		///	0 - success
		///	-1 - failure, comm doesnt initialize
		///	-2 - failure, cannot find the comm
		///	-3 - failure, the com denied access
		///	-4 - failure, unknow error
		///	-5 - failure, the comm is opened
		///	-6 - failure, parameters are invalid
		///	x > 0 - failure, x is the result of calling GetLastError
		/// ----------------------------------------------------------------------------
		virtual int open_() noexcept = 0;

		///
		/// brief: to close serial port that has been opened
		/// return - int
		///		0 - success
		///		-1 - failure, serial port doesnt open
		///		x > 0 - failure, x is the result of calling GetLastError
		///
		virtual int close_() noexcept = 0;

		///
		/// brief: to set the bytes of reading to notify
		/// len - the bytes to read of the next time 
		/// return - void
		///
		virtual void set_recv_len_(const unsigned int len) noexcept = 0;

		///
		/// brief: to check if the current serial port opened
		/// return - bool
		///		true - it is opened
		///		false - it is closed
		///
		virtual bool is_opened_() noexcept = 0;

		///
		/// brief: write data to the serial port after opened
		/// pdata - write data
		/// data_len - the length of written data
		/// return - int
		///		-1 - failure, pdata parameter is null /  data_len is 0
		///		-2 - failure, serial port doesnt open
		///		X >= 0 - has written data
		///
		virtual int send_(const char* pdata, const unsigned int data_len) = 0;

		///
		/// brief: to get  information of current library version
		/// return - std::string
		///		the string of version
		///
		virtual std::string version_() noexcept = 0;

		/// 
		/// @ brief: to get available information of serial port
		/// @ return - lib_sp::list_sp_name
		/// 
		/// 
		virtual list_sp_name available_sp_() noexcept = 0;

	};
		
	/// ----------------------------------------------------------------------------
	/// @brief：		to use iserial_port, it will create an iserial_port object
	/// @return: 	iserial_port * 
	/// 			
	/// ----------------------------------------------------------------------------
	extern "C" _lib_sp_api_ iserial_port *create_sp_() noexcept;

	/// ----------------------------------------------------------------------------
	/// @brief: to release an object that was created by function sp_create_win after released , parameter psp sets nullptr
	/// ----------------------------------------------------------------------------
	extern "C" _lib_sp_api_ void release_sp_(iserial_port* psp) noexcept;


} //! lib_sp

#endif // has_cxx11
