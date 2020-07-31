#pragma once
#include "mysql_pool.h"
#include "mysql_manager.h"
#include "mysql_connect.h"

class mysql_data {
public:
	virtual void SetData(const std::vector<char*>& datas) = 0;
};