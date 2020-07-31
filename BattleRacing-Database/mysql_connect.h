#pragma once
#include "mysql_header.h"

class mysql_connect {
public:
	mysql_connect(mysql_pool* pool);
	mysql_connect(const std::string& dbName);
	~mysql_connect();

public:
	template<typename _type>
	inline std::vector<_type> Query(const std::string& query) {
		std::vector<_type> results;
		if (mConnection) {
			if (mysql_query(mConnection, query.c_str()) == 0) {
				auto result = mysql_store_result(mConnection);
				if (result) {
					MYSQL_ROW row;
					std::vector<char*> rows;
					const auto rowCount = mysql_num_fields(mConnection);

					while (row = mysql_fetch_fields(result)) {
						for (auto i = 0; i < rowCount; i++) {
							rows.push_back(row);
						}
						_type outData;

						outData.SetData(rows);
						results.push_back(outData);
						rows.clear();
					}
					mysql_free_result(result);
				}
				else if (mysql_field_count(mConnection) != 0) {
					mbIsSuccess = false;
					std::cerr << mysql_error(mConnection) << std::endl;
				}
			}
			else {
				mbIsSuccess = false;
				std::cerr << mysql_error(mConnection) << std::endl;
			}
		}
		else {
			mbIsSuccess = false;
			std::cerr << mysql_error(mConnection) << std::endl;
		}
		return results;
	}

private:
	void BeginTransaction();
	void Commit();
	void Rollback();

private:
	MYSQL* mConnection;
	mysql_pool* mPool;
	bool mbIsSuccess;

};