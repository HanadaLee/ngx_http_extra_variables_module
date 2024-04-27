# ngx_http_extra_vars_module

A collection of extra variables for NGINX. Used to meet logging or other needs.

## Embedded Variables

### \$request_uri_path

Full original request URI path (without arguments).

### \$request_uri_args

Full original request URI arguments.

### \$request_uri_is_args

“?” if original request URI has arguments, or an empty string otherwise.

### \$request_uri_has_args

"&" if original request URI has arguments, or "?" otherwise.

### \$rewritten_uri

Full rewritten URI (with arguments).
The value of $rewritten_uri may change during request processing, e.g. when doing internal redirects, or when using index files.

### \$rewritten_uri_path

Full rewritten URI path (without arguments).
The value of $rewritten_uri_path may change during request processing, e.g. when doing internal redirects, or when using index files.

### \$normalized_rewritten_uri

current URI in request (with arguments), normalized.
The value of $normalized_rewritten_uri may change during request processing, e.g. when doing internal redirects, or when using index files.

### \$normalized_rewritten_uri_path

current URI in request (without arguments), normalized.
The value is consistent with $uri.
The value of $normalized_rewritten_uri_path may change during request processing, e.g. when doing internal redirects, or when using index files.

#### \$dollar
A literal dollar sign.

#### \$sec
Current time in seconds.

#### \$ext
The extension from \$uri.

#### \$resty_request_id
Unique request identifier composed of request timestamp, host name and random string. If the request header X-Resty-Request-Id is included, the value inherited from this request header.

#### \$has_args
"&" if a request line has arguments, or "?" otherwise.

#### \$is_internal
"1" if the current request is an "internal request", i.e., a request initiated from inside the current Nginx server instead of from the client side, or "0" otherwise.

#### \$is_subrequest
"1" if the current request is an Nginx subrequest, or "0" otherwise.

#### \$location
The name of the current location block.

#### \$redirect_count
The number of times the current request has been internally.

#### \$subrequest_count
The number of subrequests performed for this request.

#### \$connection_established_ts
Connection established timestamp in seconds with the milliseconds resolution.

#### \$ssl_handshake_start_ts
SSL handshake start timestamp in seconds with the milliseconds resolution.

#### \$ssl_handshake_end_ts
SSL handshake finish timestamp in seconds with the milliseconds resolution.

#### \$ssl_handshake_time
Keeps time spent on ssl handshaking in seconds with the milliseconds resolution.

#### \$request_created_ts
Request created timestamp in seconds with the milliseconds resolution.

#### \$response_header_sent_ts
Response header sent timestamp in seconds with the milliseconds resolution.

#### \$request_handling_time
Keeps time spent on handling request internally from receiving the request to sending the response header to the client.

#### \$response_body_time
Keeps time spent on sending the response body to the client.

#### \$ignore_cache_control
"1" if the value of the proxy_ignore_header directive contains Cache-Control, or "0" otherwise.

#### \$ignore_x_accel_expires
"1" if the value of the proxy_ignore_header directive contains X-Accel-Expires, or "0" otherwise.

#### \$upstream_url
Full upstream request url.

#### \$upstream_start_ts
Keeps timestamp of upstream starts; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.

#### \$upstream_ssl_start_ts
Keeps timestamp of upstream ssl handshake starts; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.

#### \$upstream_ssl_time
Keeps time spent on upstream ssl handshake; the time is kept in seconds with millisecond resolution. Note that this timing starts only after receiving the upstream request header. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.

#### \$upstream_send_start_ts
Keeps timestamp of upstream request send starts; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.

#### \$upstream_send_end_ts
Keeps timestamp of upstream request send ends; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.

#### \$upstream_send_time
Keeps time spent on sending request to the upstream server; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.

#### \$upstream_header_ts
Keeps timestamp of upstream response header sent; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.

#### \$upstream_end_ts
Keeps timestamp of upstream response sent or abnormal interruption; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.

#### \$upstream_read_time
Keeps time spent on reading response from the upstream server; the time is kept in seconds with millisecond resolution. Note that this timing starts only after receiving the upstream request header. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.

#### \$cache_file
The cache file path for a cached.


## Off topic

The following is some timestamp calculation logic.  The variables calculated using $msec have accurate values only when used in the access logs. Their values may not be accurate if used outside the log phase.

- \$connection_time = \$msec - \$connect_established_ts (connections may be reused)
- \$ssl_handshake_time = \$ssl_handshake_end_ts - \$ssl_handshake_start_ts
- \$upstream_dns_time = \$upstream_dns_end_ts - \$upstream_dns_start_ts
- \$upstream_connect_time = \$upstream_send_start_ts - \$upstream_start_ts
- \$upstream_ssl_time = \$upstream_send_start_ts - \$upstream_ssl_start_ts
- \$upstream_send_time = \$upstream_send_end_ts - \$upstream_send_start_ts
- \$upstream_read_time = \$upstream_end_ts - \$upstream_send_end_ts
- \$upstream_header_time = \$upstream_header_ts - \$upstream_start_ts
- \$upstream_response_time = \$upstream_end_ts - \$upstream_start_ts
- \$request_handling_time = \$response_header_sent_ts - \$request_created_ts
- \$response_body_time = \$msec - \$response_header_sent_ts
- \$request_time = \$msec - \$request_created_ts