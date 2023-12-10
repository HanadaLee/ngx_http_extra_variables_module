| Variable                          |Value                                              |
| --------------------------------- | ------------------------------------------------- |
| **$dollar**                       | A literal dollar sign.                            |
| **$sec**                          | Current time in seconds.                          |
| **$ext**                          | The extension from \$uri.                         |
| **$has_args**                     | “&” if a request line has arguments, or “?” otherwise|
| **$location**                     | The name of the current location block.    |
| **$redirect_count**               | The number of times the current request has been internally.|
| **$subrequest_count**             | The number of subrequests performed for this request.|
| **$connect_start_ts**             | Connect start timestamp in seconds with the milliseconds resolution.|
| **$request_create_ts**            | Request create timestamp in seconds with the milliseconds resolution.|
| **$response_header_sent_ts**      | Response header sent timestamp in seconds with the milliseconds resolution.|
| **$ignore_cache_control**         | "1" if the value of the proxy_ignore_header directive contains cache-control, or "0" otherwise.|
| **$upstream_url**                 | Full upstream request url.                         |
| **$upstream_connect_start_ts**    | Keeps timestamp of upstream connection starts; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.|
| **$upstream_send_start_ts**       | Keeps timestamp of upstream request send starts; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.|
| **$upstream_send_end_ts**         | Keeps timestamp of upstream request send ends; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.|
| **$upstream_header_ts**           | Keeps timestamp of upstream response header sent; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.|
| **$upstream_response_ts**           | Keeps timestamp of upstream response body sent; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.|
| **$upstream_send_time**           | Keeps time spent on sending request to the upstream server; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.
| **$upstream_read_time**           | Keeps time spent on reading response body from the upstream server; the time is kept in seconds with millisecond resolution. Times of several responses are separated by commas and colons like addresses in the $upstream_addr variable.|
| **$cache_file**                   | The cache file path for a cached.                 |