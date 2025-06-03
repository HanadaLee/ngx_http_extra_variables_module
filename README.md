# ngx_http_extra_variables_module

A collection of extra variables for NGINX. Used to meet logging or other needs.

Some of these variables need to come from nginx core patch [nginx-ext.patch](https://git.hanada.info/hanada/openresty/-/tree/main/patches) to work. These variables will be marked with * in the following text.

## Embedded Variables

### \$request_path
Original request path (without arguments).

### \$request_args
Original request arguments.

### \$request_is_args
“?” if original request uri has arguments, or an empty string otherwise.

### \$request_has_args
"&" if original request uri has arguments, or "?" otherwise.

### \$current_uri
Full current uri (with arguments), normalized.

The value of $current_uri may change during request processing, e.g. when doing internal redirects, or when using index files.

### \$current_path
Current path (without arguments), normalized.

The value of $current_path is the same as $uri. it may change during request processing, e.g. when doing internal redirects, or when using index files.

### \$escaped_current_uri
Full current uri in request (with arguments), normalized and escaped again.

The value of $escaped_current_uri may change during request processing, e.g. when doing internal redirects, or when using index files.

### \$escaped_current_path
Current path in request (without arguments), normalized and escaped again.

The value of $normalized_current_uri_path may change during request processing, e.g. when doing internal redirects, or when using index files.

### \$has_args
"&" if a request line has arguments, or "?" otherwise.

### \$dollar
A literal dollar sign.

### \$sec
Current time in seconds.

### \$ext
The extension from \$uri.

### \$is_internal
"1" if the current request is an "internal request", i.e., a request initiated from inside the current Nginx server instead of from the client side, or "0" otherwise.

### \$is_subrequest
"1" if the current request is an Nginx subrequest, or "0" otherwise.

### \$redirect_count
The number of times the current request has been internally.

### \$subrequest_count
The number of subrequests performed for this request.

### \$connection_established_msec
Connection established timestamp in seconds with the milliseconds resolution.

### \$request_created_msec
Request created timestamp in seconds with the milliseconds resolution.

### \$response_header_sent_msec *
Response header sent timestamp in seconds with the milliseconds resolution.

### \$request_handling_time *
Keeps time spent on handling request internally from receiving the request to sending the response header to the client.

### \$response_body_time *
Keeps time spent on sending the response body to the client.

### \$hostname_uppercase
Uppercase host name.

### \$hostname_lowercase
Lowercase host name.

### \$time_rfc1123
Local time in the rfc 1123 standard format. the time format is consistent with the http `Date` header.

### \$upstream_method *
Upstream method, usually “GET” or “POST”.

### \$upstream_scheme
Upstream scheme, "http" or "https".

### \$upstream_uri
Full upstream request uri.

### \$upstream_last_addr
Keeps the IP address and port, or the path to the UNIX-domain socket of the latest upstream server.

### \$upstream_last_status
Keeps status code of the response obtained from the latest upstream server.

### \$upstream_start_msec *
Keeps timestamp of upstream starts; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.

### \$upstream_last_start_msec *
Keeps timestamp of latest upstream starts; the time is kept in seconds with millisecond resolution.

### \$upstream_ssl_start_msec *
Keeps timestamp of upstream ssl handshake starts; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.

### \$upstream_last_ssl_start_msec *
Keeps timestamp of latest upstream ssl handshake starts; the time is kept in seconds with millisecond resolution.

### \$upstream_send_start_msec *
Keeps timestamp of upstream request send starts; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.

### \$upstream_last_send_start_msec *
Keeps timestamp of latest upstream request send starts; the time is kept in seconds with millisecond resolution.

### \$upstream_send_end_msec *
Keeps timestamp of upstream request send ends; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.

### \$upstream_last_send_end_msec *
Keeps timestamp of latest upstream request send ends; the time is kept in seconds with millisecond resolution.

### \$upstream_header_msec
Keeps timestamp of upstream response header sent; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.

### \$upstream_last_header_msec
Keeps timestamp of latest upstream response header sent; the time is kept in seconds with millisecond resolution.

### \$upstream_end_msec
Keeps timestamp of upstream response sent or abnormal interruption; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.

### \$upstream_last_end_msec
Keeps timestamp of latest upstream response sent or abnormal interruption; the time is kept in seconds with millisecond resolution.

### \$upstream_last_connect_time
Keeps time spent on establishing a connection with the upstream server; the time is kept in seconds with millisecond resolution. In case of SSL, includes time spent on handshake.

### \$upstream_transport_connect_time *
Keeps time spent on establishing a connection with the upstream server; the time is kept in seconds with millisecond resolution. In case of SSL, does not include time spent on handshake. Times of several connections are separated by commas and colons like addresses in the $upstream_addr variable.

### \$upstream_last_transport_connect_time *
Keeps time spent on establishing a connection with the upstream server; the time is kept in seconds with millisecond resolution. In case of SSL, does not include time spent on handshake.

### \$upstream_ssl_time *
Keeps time spent on upstream ssl handshake; the time is kept in seconds with millisecond resolution. Note that this timing starts only after receiving the upstream request header. Times of several ssl connections are separated by commas and colons like addresses in the $upstream_addr variable.

### \$upstream_last_ssl_time *
Keeps time spent on latest upstream ssl handshake; the time is kept in seconds with millisecond resolution. Note that this timing starts only after receiving the upstream request header.

### \$upstream_send_time *
Keeps time spent on sending request to the upstream server; the time is kept in seconds with millisecond resolution. Times of several send requests are separated by commas and colons like addresses in the $upstream_addr variable.

### \$upstream_last_send_time *
Keeps time spent on sending request to the latest upstream server; the time is kept in seconds with millisecond resolution.

### \$upstream_read_time *
Keeps time spent on reading response from the upstream server; the time is kept in seconds with millisecond resolution. Note that this timing starts only after receiving the upstream request header. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.

### \$upstream_last_read_time *
Keeps time spent on reading response from the latest upstream server; the time is kept in seconds with millisecond resolution. Note that this timing starts only after receiving the upstream request header.

### \$upstream_last_header_time
Keeps time spent on receiving the response header from the latest upstream server; the time is kept in seconds with millisecond resolution.

### \$upstream_last_response_time
Keeps time spent on receiving the response from the latest upstream server; the time is kept in seconds with millisecond resolution.

### \$upstream_last_response_length
Keeps the length of the response obtained from the upstream server; the length is kept in bytes.

### \$upstream_last_bytes_received
Number of bytes received from an upstream server.

### \$upstream_last_bytes_sent
Number of bytes sent to an upstream server.

### \$upstream_cacheable
"1" if the upstream request is cacheable, or "0" otherwise. it will be empty when the upstream request has not yet been established, do not use it in the early phase of the request (such as the rewrite or access phase).

### \$upstream_cache_key
The cache key being used.

### \$upstream_cache_crc32
The crc32 checksum of cache key.

### \$upstream_cache_hash
MD5 hash of the current cache key. Returns variant cache key hash when present, otherwise matches `$upstream_cache_main_hash`.

### \$upstream_cache_main_hash
MD5 hash of the main cache key (without variant characteristics).

### \$upstream_cache_variant_hash
MD5 hash of variant-specific cache key generated when using `Vary` header-based caching.

### \$upstream_cache_file
The file path of upstream cache.

### \$upstream_cache_age
Age of the cache item.

### \$upstream_cache_create_sec
Cache create time. unix time.

### \$upstream_cache_create_date
Cache create time. the time format is consistent with the http `Date` header.

### \$upstream_cache_expire_sec
Cache expire time. unix time.

### \$upstream_cache_expire_date
Cache expire time. the time format is consistent with the http `Date` header.

### \$upstream_cache_ttl
Cache ttl.

### \$upstream_cache_max_age
Cache max age.

## Variables related to time-spent measurement

The following is some timestamp calculation logic. The variables calculated using $msec have accurate values only when used in the access logs. Their values may not be accurate if used outside the log phase.

- \$connection_time = \$msec - \$connection_established_msec (connections may be reused)
- \$request_handling_time = \$response_header_sent_msec - \$request_created_msec
- \$response_body_time = \$msec - \$response_header_sent_msec
- \$request_time = \$msec - \$request_created_msec

- \$upstream_last_transport_connect_time = \$upstream_last_ssl_start_msec - \$upstream_last_start_msec (with ssl)
- \$upstream_last_transport_connect_time = \$upstream_last_send_start_msec - \$upstream_last_start_msec (without ssl)

- \$upstream_last_ssl_time = \$upstream_last_send_start_msec - \$upstream_last_ssl_start_msec
- \$upstream_last_send_time = \$upstream_last_send_end_msec - \$upstream_last_send_start_msec
- \$upstream_last_read_time = \$upstream_last_end_msec - \$upstream_last_header_msec

- \$upstream_last_header_time = \$upstream_last_header_msec - \$upstream_last_start_msec
- \$upstream_last_response_time = \$upstream_last_end_msec - \$upstream_last_start_msec


# Author

Jonathan Kolb
Hanada im@hanada.info

# License

This Nginx module is licensed under [BSD 2-Clause License](LICENSE).