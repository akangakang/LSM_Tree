#pragma once

#include "kvstore_api.h"
#include "Skiplist.h"
#include "LSM_tree.h"
using  namespace std;

class KVStore : public KVStoreAPI {
	// You can add your implementation here
private:


    LSM_tree lsm;


public:
	KVStore(const std::string &dir);

	~KVStore();

	void put(uint64_t key, const std::string &s) override;

	std::string get(uint64_t key) override;

	bool del(uint64_t key) override;

	void reset() override;

private:


};
