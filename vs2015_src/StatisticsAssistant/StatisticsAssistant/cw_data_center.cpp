#include "cw_data_center.h"


cw_data_center::cw_data_center()
{
}


cw_data_center::~cw_data_center()
{
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void cw_data_center::init_(const sa_def::en_protocal_type& type)
{
	pro_type_			= type;
}

/// ----------------------------------------------------------------
/// @brief: 获取UDP可用通道IP
/// ----------------------------------------------------------------
int cw_data_center::get_udp_available_channel_id_()
{
	return 0;
}
