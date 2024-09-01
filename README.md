# ngx_http_extra_vars_module

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

### \$ssl_handshake_start_ts
ssl handshake start timestamp in seconds with the milliseconds resolution.

### \$ssl_handshake_end_ts
ssl handshake finish timestamp in seconds with the milliseconds resolution.

### \$ssl_handshake_time
keeps time spent on ssl handshaking in seconds with the milliseconds resolution.

### \$request_created_ts
request created timestamp in seconds with the milliseconds resolution.

### \$response_header_sent_ts
response header sent timestamp in seconds with the milliseconds resolution.

### \$request_handling_time
keeps time spent on handling request internally from receiving the request to sending the response header to the client.

### \$response_body_time
keeps time spent on sending the response body to the client.

### \$ignore_cache_control
"1" if the value of the proxy_ignore_header directive contains Cache-Control, or "0" otherwise.

### \$ignore_x_accel_expires
"1" if the value of the proxy_ignore_header directive contains X-Accel-Expires, or "0" otherwise.

### \$hostname_uppercase
Uppercase host name.

### \$hostname_lowercase
Lowercase host name.

### \$time_rfc1123
local time in the rfc 1123 standard format. the time format is consistent with the http `Date` header.

### \$cache_file
the cache file path for a cached.

## Off topic

The following is some timestamp calculation logic.  The variables calculated using $msec have accurate values only when used in the access logs. Their values may not be accurate if used outside the log phase.

- \$connection_time = \$msec - \$connection_established_ts (connections may be reused)
- \$ssl_handshake_time = \$ssl_handshake_end_ts - \$ssl_handshake_start_ts
- \$request_handling_time = \$response_header_sent_ts - \$request_created_ts
- \$response_body_time = \$msec - \$response_header_sent_ts
- \$request_time = \$msec - \$request_created_ts