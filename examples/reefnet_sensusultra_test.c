#include <stdio.h>	// fopen, fwrite, fclose
#include <time.h>	// time

#include "reefnet_sensusultra.h"
#include "utils.h"

#define WARNING(expr) \
{ \
	message ("%s:%d: %s\n", __FILE__, __LINE__, expr); \
}


int test_dump_memory_dives (const char* name, const char* filename)
{
	device_t *device = NULL;
	unsigned char handshake[REEFNET_SENSUSULTRA_HANDSHAKE_SIZE] = {0};

	message ("reefnet_sensusultra_device_open\n");
	int rc = reefnet_sensusultra_device_open (&device, name);
	if (rc != DEVICE_STATUS_SUCCESS) {
		WARNING ("Error opening serial port.");
		return rc;
	}

	message ("device_handshake\n");
	rc = device_handshake (device, handshake, sizeof (handshake));
	if (rc != DEVICE_STATUS_SUCCESS) {
		WARNING ("Cannot read handshake.");
		device_close (device);
		return rc;
	}

	time_t now = time (NULL);
	char datetime[21] = {0};
	strftime (datetime, sizeof (datetime), "%Y-%m-%dT%H:%M:%SZ", gmtime (&now));
	message ("time=%lu (%s)\n", (unsigned long)now, datetime);

	message ("device_foreach\n");
	rc = device_foreach (device, NULL, NULL);
	if (rc != DEVICE_STATUS_SUCCESS) {
		WARNING ("Cannot read dives.");
		device_close (device);
		return rc;
	}

	message ("device_close\n");
	rc = device_close (device);
	if (rc != DEVICE_STATUS_SUCCESS) {
		WARNING ("Cannot close device.");
		return rc;
	}

	return DEVICE_STATUS_SUCCESS;
}


int test_dump_memory_data (const char* name, const char* filename)
{
	device_t *device = NULL;
	unsigned char data[REEFNET_SENSUSULTRA_MEMORY_DATA_SIZE] = {0};
	unsigned char handshake[REEFNET_SENSUSULTRA_HANDSHAKE_SIZE] = {0};

	message ("reefnet_sensusultra_device_open\n");
	int rc = reefnet_sensusultra_device_open (&device, name);
	if (rc != DEVICE_STATUS_SUCCESS) {
		WARNING ("Error opening serial port.");
		return rc;
	}

	message ("device_handshake\n");
	rc = device_handshake (device, handshake, sizeof (handshake));
	if (rc != DEVICE_STATUS_SUCCESS) {
		WARNING ("Cannot read handshake.");
		device_close (device);
		return rc;
	}

	time_t now = time (NULL);
	char datetime[21] = {0};
	strftime (datetime, sizeof (datetime), "%Y-%m-%dT%H:%M:%SZ", gmtime (&now));
	message ("time=%lu (%s)\n", (unsigned long)now, datetime);

	message ("device_dump\n");
	rc = device_dump (device, data, sizeof (data));
	if (rc < 0) {
		WARNING ("Cannot read memory.");
		device_close (device);
		return rc;
	}

	message ("Dumping data\n");
	FILE* fp = fopen (filename, "wb");
	if (fp != NULL) {
		fwrite (data, sizeof (unsigned char), rc, fp);
		fclose (fp);
	}

	message ("device_close\n");
	rc = device_close (device);
	if (rc != DEVICE_STATUS_SUCCESS) {
		WARNING ("Cannot close device.");
		return rc;
	}

	return DEVICE_STATUS_SUCCESS;
}


int test_dump_memory_user (const char* name, const char* filename)
{
	device_t *device = NULL;
	unsigned char data[REEFNET_SENSUSULTRA_MEMORY_USER_SIZE] = {0};
	unsigned char handshake[REEFNET_SENSUSULTRA_HANDSHAKE_SIZE] = {0};

	message ("reefnet_sensusultra_device_open\n");
	int rc = reefnet_sensusultra_device_open (&device, name);
	if (rc != DEVICE_STATUS_SUCCESS) {
		WARNING ("Error opening serial port.");
		return rc;
	}

	message ("device_handshake\n");
	rc = device_handshake (device, handshake, sizeof (handshake));
	if (rc != DEVICE_STATUS_SUCCESS) {
		WARNING ("Cannot read handshake.");
		device_close (device);
		return rc;
	}

	time_t now = time (NULL);
	char datetime[21] = {0};
	strftime (datetime, sizeof (datetime), "%Y-%m-%dT%H:%M:%SZ", gmtime (&now));
	message ("time=%lu (%s)\n", (unsigned long)now, datetime);

	message ("reefnet_sensusultra_device_read_user\n");
	rc = reefnet_sensusultra_device_read_user (device, data, sizeof (data));
	if (rc != DEVICE_STATUS_SUCCESS) {
		WARNING ("Cannot read memory.");
		device_close (device);
		return rc;
	}

	message ("Dumping data\n");
	FILE* fp = fopen (filename, "wb");
	if (fp != NULL) {
		fwrite (data, sizeof (unsigned char), sizeof (data), fp);
		fclose (fp);
	}

	message ("device_close\n");
	rc = device_close (device);
	if (rc != DEVICE_STATUS_SUCCESS) {
		WARNING ("Cannot close device.");
		return rc;
	}

	return DEVICE_STATUS_SUCCESS;
}


const char* errmsg (int rc)
{
	switch (rc) {
	case DEVICE_STATUS_SUCCESS:
		return "Success";
	case DEVICE_STATUS_UNSUPPORTED:
		return "Unsupported operation";
	case DEVICE_STATUS_TYPE_MISMATCH:
		return "Device type mismatch";
	case DEVICE_STATUS_ERROR:
		return "Generic error";
	case DEVICE_STATUS_IO:
		return "Input/output error";
	case DEVICE_STATUS_MEMORY:
		return "Memory error";
	case DEVICE_STATUS_PROTOCOL:
		return "Protocol error";
	case DEVICE_STATUS_TIMEOUT:
		return "Timeout";
	default:
		return "Unknown error";
	}
}


int main(int argc, char *argv[])
{
	message_set_logfile ("SENSUSULTRA.LOG");

#ifdef _WIN32
	const char* name = "COM1";
#else
	const char* name = "/dev/ttyS0";
#endif

	if (argc > 1) {
		name = argv[1];
	}

	message ("DEVICE=%s\n", name);

	int a = test_dump_memory_data (name, "SENSUSULTRA_DATA.DMP");
	int b = test_dump_memory_user (name, "SENSUSULTRA_USER.DMP");
	int c = test_dump_memory_dives (name, "SENSUSULTRA_DIVES.DMP");

	message ("SUMMARY\n");
	message ("-------\n");
	message ("test_dump_memory_data:     %s\n", errmsg (a));
	message ("test_dump_memory_user:     %s\n", errmsg (b));
	message ("test_dump_memory_dives:    %s\n", errmsg (c));

	message_set_logfile (NULL);

	return 0;
}
