#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h> 
#include <arpa/inet.h>
#include <sys/socket.h>

#include "dns.h"
#include "request.h"
#include "response.h"
#include "query.h"

#include "dnsc.h"

#define DEFAULT_PORT		53
#define DEFAULT_TIMEOUT		5
#define DEFAULT_MAX_RETRIES	3
#define MAX_IP_STRING_SIZE	16

/* Formats a 32-bit IP address into a dotted quad string and
   copies it into the given buffer. */
static void format_ip_address(uint32_t ip_address, char *buffer) {
  uint8_t *segments = (uint8_t *)&ip_address;

  sprintf(buffer, "%d.%d.%d.%d", segments[3], segments[2],
  	  segments[1], segments[0]);
}

bool DNSC::isIP(const char* host) {
	while(true) {
		char c = *(host++);
		if(c == 0)
			break;
		if(c != '.' && (c < '0' || c > '9'))
			return false;
	}
	return true;
}

bool DNSC::resolv(const char* domain, int type, std::vector<DNSRes>& res, const char* server) {
  char error_message[ERROR_BUFFER + 1], ip_buffer[MAX_IP_STRING_SIZE];
  int i, request_id, packet_size, answer_count;
  int port, timeout, retries, request_q_type;
  void *request_buffer, *response_buffer;
  struct dns_response *responses;

  *error_message = 0;

  /* Use the current time as a seed for the random number generator. */
  srand((unsigned int)time(0));

  /* set defaults for optional arguments in case none are specified */
  port		 = DEFAULT_PORT;
  timeout	 = DEFAULT_TIMEOUT;
  retries	 = DEFAULT_MAX_RETRIES;
  request_q_type = DNS_A_RECORD;

  /* Build the DNS request packet for the supplied domain name. */
  request_buffer = build_dns_request_packet(domain, &packet_size, &request_id, 
					    request_q_type, error_message);
  if (request_buffer == 0) {
		return false;
  }

  /* Send the request packet and wait for a response from the server. */
  response_buffer = query_dns_server(request_buffer, &packet_size, server, 
				     port, timeout, retries, error_message);
  free_dns_request_packet(request_buffer);
  if (response_buffer == 0) {
		return false;
  }

  /* Parse the response from the server. */
  responses = parse_dns_response(response_buffer, packet_size, request_id, 
				 domain, &answer_count, error_message);
  free_response_buffer(response_buffer);

  /* If a null value was returned, it could either mean there was an error or
     the domain name was not found. Check the error_message buffer to see
     if it contains any data. */
  if (responses == 0) {
		return false;
  }

  for (i = 0; i < answer_count; ++i) {
		if(type != responses[i].response_type) {
			continue;
		}

		DNSRes r;

    switch(type) {
    case DNS_A_RECORD:
      format_ip_address(responses[i].ip_address, ip_buffer);
      r.res = ip_buffer;
			r.cacheTime = responses[i].cache_time;
			res.push_back(r);
      break;

    case DNS_NS_RECORD:
      r.res = responses[i].name;
			r.cacheTime = responses[i].cache_time;
			res.push_back(r);
      break;

    case DNS_CNAME_RECORD:
      r.res = responses[i].name;
			r.cacheTime = responses[i].cache_time;
			res.push_back(r);
      break;

    case DNS_MX_RECORD:
      r.res = responses[i].name;
			r.cacheTime = responses[i].cache_time;
			res.push_back(r);
      break;

    default:
      break;
    }
  }

  free(responses);
  return true;
}

std::string DNSC::ip(const std::string& domain) {
	std::vector<DNSRes> r;
	DNSC::resolv(domain.c_str(), DNS_A_RECORD, r);
	if(r.size() == 0)
		return "";
	
	return r[0].res;
}

