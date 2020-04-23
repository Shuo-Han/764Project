// #include <stdio.h>
// #include <vector>
// #include <thread>
#include "txnManager.h"

using namespace littleBadger;

int main(int argc, char **argv) {
	printf("begin setup.\n");
	
	bulildDataSet();

	printf("begin TxnSet\n");
	buildTxnSet();

	printf("begin check\n");
	readTxnSet();

	return 1;
}
