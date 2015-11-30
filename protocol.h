/* Group 9: Mitchell Devenport, Nathan Jones,
 * 	Richard Orr, and Sameer Singh
 * This header file holds the header information for our protocols.
 * Due Date: 12/2/2015
 * Sekou Remy's class
*/

#include <inttypes.h>

//class protocol packet header
union Protocol
{
	struct classProtocol
	{
		//define each field to have a width of 32 bits
		int32_t protocol;
		int32_t password;
		int32_t cliRequest;
		int32_t requestData;
		int32_t offset;
		int32_t totalSize;
		int32_t payloadSize;
		char payload[272]; //max payload 272 bytes
	} class;
	struct nineProtocol
	{
	     int32_t protocol; 
	     int32_t password;     
	     int32_t offset;     
	     int32_t totalSize;  
	     int32_t payloadSize;
	     char payload[280];

	} nine;
	char data_arr[300];
};
