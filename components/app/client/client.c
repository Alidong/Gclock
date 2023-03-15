#include "client.h"
#include "http_client/http_client.h"
#include "ntp/ntp.h"
void client_init(void)
{
    //http_req();
    ntp_init();
}