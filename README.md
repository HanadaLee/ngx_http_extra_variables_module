| Variable                          |Value                                              |
| --------------------------------- | ------------------------------------------------- |
| **$cache_file**                   | The cache file path for a cached.                 |
| **$ext**                          | The extension from \$uri.                         |
| **$location**                     | Set to the name of the current location block.    |
| **$redirect_count**               | The number of times the current request has been internally.|
| **$subrequest_count**             | The number of subrequests performed for this request.|
| **$connect_start_ts**             | Connect start timestamp in seconds with the milliseconds resolution.|
| **$request_create_ts**            | Request create timestamp in seconds with the milliseconds resolution.|
| **$ignore_cache_control**         | Returns "1" if the value of the proxy_ignore_header directive contains cache-control, or "0" otherwise.|
| **$upstream_url**                 | Full upstream request url.                         |
| **$upstream_connect_start_ts**    | keeps timestamp of upstream connection starts; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.|
| **$upstream_connect_end_ts**      | keeps timestamp of upstream connection ends; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.|
| **$upstream_send_start_ts**       | Keep timestamp when the request was started to be sent to the upstream; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.|
| **$upstream_send_end_ts**         | Keep timestamp of completion of sending the request to the upstream; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.|