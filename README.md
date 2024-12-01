# ngx_http_extra_variables_module

A collection of extra variables for NGINX. Used to meet logging or other needs.

## Embedded Variables

### \$request_path

original request path (without arguments).

### \$request_args

original request arguments.

### \$request_is_args

“?” if original request uri has arguments, or an empty string otherwise.

### \$request_has_args

"&" if original request uri has arguments, or "?" otherwise.

### \$current_uri

full current uri (with arguments), normalized.

the value of $current_uri may change during request processing, e.g. when doing internal redirects, or when using index files.

### \$current_path

current path (without arguments), normalized.

the value of $current_path is the same as $uri. it may change during request processing, e.g. when doing internal redirects, or when using index files.

### \$escaped_current_uri

full current uri in request (with arguments), normalized and escaped again.

the value of $escaped_current_uri may change during request processing, e.g. when doing internal redirects, or when using index files.

### \$escaped_current_path

current path in request (without arguments), normalized and escaped again.

the value of $normalized_current_uri_path may change during request processing, e.g. when doing internal redirects, or when using index files.

### \$has_args
"&" if a request line has arguments, or "?" otherwise.

### \$dollar
a literal dollar sign.

### \$sec
current time in seconds.

### \$ext
the extension from \$uri.

### \$is_internal
"1" if the current request is an "internal request", i.e., a request initiated from inside the current Nginx server instead of from the client side, or "0" otherwise.

### \$is_subrequest
"1" if the current request is an Nginx subrequest, or "0" otherwise.

### \$location
the name of the current location block.

### \$redirect_count
the number of times the current request has been internally.

### \$subrequest_count
the number of subrequests performed for this request.

### \$connection_established_ts
connection established timestamp in seconds with the milliseconds resolution.

### \$request_created_ts
request created timestamp in seconds with the milliseconds resolution.

### \$response_header_sent_ts
response header sent timestamp in seconds with the milliseconds resolution.

### \$request_handling_time
keeps time spent on handling request internally from receiving the request to sending the response header to the client.

### \$response_body_time
keeps time spent on sending the response body to the client.

### \$hostname_uppercase
Uppercase host name.

### \$hostname_lowercase
Lowercase host name.

### \$time_rfc1123
local time in the rfc 1123 standard format. the time format is consistent with the http `Date` header.

### \$tcpinfo_total_retrans

### \$tcpinfo_pmtu

### \$tcpinfo_rcv_ssthresh

### \$tcpinfo_snd_ssthresh

### \$tcpinfo_advmss

### \$tcpinfo_reordering

### \$tcpinfo_rcv_rtt

### \$tcpinfo_last_data_sent

### \$upstream_method
upstream method, usually “GET” or “POST”.

### \$upstream_scheme
upstream scheme, "http" or "https".

### \$upstream_uri
full upstream request uri.

### \$upstream_last_addr
keeps the IP address and port, or the path to the UNIX-domain socket of the latest upstream server.

### \$upstream_last_status
keeps status code of the response obtained from the latest upstream server.

### \$upstream_start_ts
keeps timestamp of upstream starts; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.

### \$upstream_last_start_ts
keeps timestamp of latest upstream starts; the time is kept in seconds with millisecond resolution.

### \$upstream_ssl_start_ts
keeps timestamp of upstream ssl handshake starts; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.

### \$upstream_last_ssl_start_ts
keeps timestamp of latest upstream ssl handshake starts; the time is kept in seconds with millisecond resolution.

### \$upstream_send_start_ts
keeps timestamp of upstream request send starts; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.

### \$upstream_last_send_start_ts
keeps timestamp of latest upstream request send starts; the time is kept in seconds with millisecond resolution.

### \$upstream_send_end_ts
keeps timestamp of upstream request send ends; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.

### \$upstream_last_send_end_ts
keeps timestamp of latest upstream request send ends; the time is kept in seconds with millisecond resolution.

### \$upstream_header_ts
keeps timestamp of upstream response header sent; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.

### \$upstream_last_header_ts
keeps timestamp of latest upstream response header sent; the time is kept in seconds with millisecond resolution.

### \$upstream_end_ts
keeps timestamp of upstream response sent or abnormal interruption; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.

### \$upstream_last_end_ts
keeps timestamp of latest upstream response sent or abnormal interruption; the time is kept in seconds with millisecond resolution.

### \$upstream_last_connect_time
keeps time spent on establishing a connection with the upstream server; the time is kept in seconds with millisecond resolution. In case of SSL, includes time spent on handshake.

### \$upstream_transport_connect_time
keeps time spent on establishing a connection with the upstream server; the time is kept in seconds with millisecond resolution. In case of SSL, does not include time spent on handshake. Times of several connections are separated by commas and colons like addresses in the $upstream_addr variable.

### \$upstream_last_transport_connect_time
keeps time spent on establishing a connection with the upstream server; the time is kept in seconds with millisecond resolution. In case of SSL, does not include time spent on handshake.

### \$upstream_ssl_time
keeps time spent on upstream ssl handshake; the time is kept in seconds with millisecond resolution. Note that this timing starts only after receiving the upstream request header. Times of several ssl connections are separated by commas and colons like addresses in the $upstream_addr variable.

### \$upstream_last_ssl_time
keeps time spent on latest upstream ssl handshake; the time is kept in seconds with millisecond resolution. Note that this timing starts only after receiving the upstream request header.

### \$upstream_send_time
keeps time spent on sending request to the upstream server; the time is kept in seconds with millisecond resolution. Times of several send requests are separated by commas and colons like addresses in the $upstream_addr variable.

### \$upstream_last_send_time
keeps time spent on sending request to the latest upstream server; the time is kept in seconds with millisecond resolution.

### \$upstream_read_time
keeps time spent on reading response from the upstream server; the time is kept in seconds with millisecond resolution. Note that this timing starts only after receiving the upstream request header. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.

### \$upstream_last_read_time
keeps time spent on reading response from the latest upstream server; the time is kept in seconds with millisecond resolution. Note that this timing starts only after receiving the upstream request header.

### \$upstream_last_header_time
keeps time spent on receiving the response header from the latest upstream server; the time is kept in seconds with millisecond resolution.

### \$upstream_last_response_time
keeps time spent on receiving the response from the latest upstream server; the time is kept in seconds with millisecond resolution.

### \$upstream_last_response_length
keeps the length of the response obtained from the upstream server; the length is kept in bytes.

### \$upstream_last_bytes_received
number of bytes received from an upstream server.

### \$upstream_last_bytes_sent
number of bytes sent to an upstream server.

### \$upstream_cache_key

the cache key being used.

### \$upstream_cache_key_crc32

the crc32 checksum of cache key.

### \$upstream_cache_key_hash

the md5sum hash of cache key.

### \$upstream_cache_main_hash

the md5sum hash of main cache key.

### \$upstream_cache_variant_hash

the md5sum hash of variant cache key when a request is cached based on the Vary response header.

### \$upstream_cache_file

the file path of upstream cache.

### \$upstream_cache_age

age of the cache item.

### \$upstream_cache_create_time

cache create time. unix time.

### \$upstream_cache_create_date

cache create time. the time format is consistent with the http `Date` header.

### \$upstream_cache_expire_time

cache expire time. unix time.

### \$upstream_cache_expire_date

cache expire time. the time format is consistent with the http `Date` header.

### \$upstream_cache_ttl

cache ttl.

### \$upstream_cache_max_age

cache max age.

## Off topic

The following is some timestamp calculation logic. The variables calculated using $msec have accurate values only when used in the access logs. Their values may not be accurate if used outside the log phase.

- \$connection_time = \$msec - \$connection_established_ts (connections may be reused)
- \$request_handling_time = \$response_header_sent_ts - \$request_created_ts
- \$response_body_time = \$msec - \$response_header_sent_ts
- \$request_time = \$msec - \$request_created_ts

# Author

Jonathan Kolb
Hanada im@hanada.info

# License

This Nginx module is licensed under [BSD 2-Clause License](LICENSE).