|Variable                   |Value                                                                           |
|---------------------------|--------------------------------------------------------------------------------|
|**$cache_file**            |The cache file path for a cached response.                                      |
|**$ext**                   |The extension from \$uri.                                                       |
|**$location**              |Set to the name of the current location block.                                  |
|**$redirect_count**        |The number of times the current request has been internally redirected.         |
|**$request_received_ts**   |Timestamp when the request was received, to msec precision.                     |
|**$subrequest_count**      |The number of subrequests performed for this request.                           |
|**$ignore_cache_control**  |Returns "1" if the value of the proxy_ignore_header directive contains cache-control, or "0" otherwise|
|**$upstream_request_line** |Full upstream request line                                                      |