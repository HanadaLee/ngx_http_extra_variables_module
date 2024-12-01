
/*
 * Copyright (C) Jonathan Kolb
 * Copyright (C) Hanada
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <nginx.h>


#define NGX_HTTP_EXTRA_VARIABLE_REDIRECT_COUNT                            0
#define NGX_HTTP_EXTRA_VARIABLE_SUBREQUEST_COUNT                          1

#define NGX_HTTP_EXTRA_VARIABLE_REQUEST_CREATED_TS                        2
#if (NGX_HTTP_EXT)
#define NGX_HTTP_EXTRA_VARIABLE_RESPONSE_HEADER_SENT_TS                   3

#define NGX_HTTP_EXTRA_VARIABLE_REQUEST_HANDlING_TIME                     4
#define NGX_HTTP_EXTRA_VARIABLE_RESPONSE_BODY_TIME                        5

#define NGX_HTTP_EXTRA_VARIABLE_UPSTREAM_START_TS                         10
#if (NGX_HTTP_SSL)
#define NGX_HTTP_EXTRA_VARIABLE_UPSTREAM_SSL_START_TS                     11
#endif
#define NGX_HTTP_EXTRA_VARIABLE_UPSTREAM_SEND_START_TS                    12
#define NGX_HTTP_EXTRA_VARIABLE_UPSTREAM_SEND_END_TS                      13
#define NGX_HTTP_EXTRA_VARIABLE_UPSTREAM_HEADER_TS                        14
#define NGX_HTTP_EXTRA_VARIABLE_UPSTREAM_END_TS                           15
#endif

#define NGX_HTTP_EXTRA_VARIABLE_UPSTREAM_CONNECT_TIME                     20
#if (NGX_HTTP_EXT)
#define NGX_HTTP_EXTRA_VARIABLE_UPSTREAM_TRANSPORT_CONNECT_TIME           21
#if (NGX_HTTP_SSL)
#define NGX_HTTP_EXTRA_VARIABLE_UPSTREAM_SSL_TIME                         22
#endif
#define NGX_HTTP_EXTRA_VARIABLE_UPSTREAM_SEND_TIME                        23
#define NGX_HTTP_EXTRA_VARIABLE_UPSTREAM_READ_TIME                        24
#endif
#define NGX_HTTP_EXTRA_VARIABLE_UPSTREAM_HEADER_TIME                      25
#define NGX_HTTP_EXTRA_VARIABLE_UPSTREAM_RESPONSE_TIME                    26

#define NGX_HTTP_EXTRA_VARIABLE_UPSTREAM_RESPONSE_LENGTH                  30
#define NGX_HTTP_EXTRA_VARIABLE_UPSTREAM_BYTES_RECEIVED                   31
#define NGX_HTTP_EXTRA_VARIABLE_UPSTREAM_BYTES_SENT                       32


static ngx_int_t ngx_http_add_extra_variables(ngx_conf_t *cf);

static ngx_int_t ngx_http_extra_variable_request_path(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_request_args(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_request_is_args(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_request_has_args(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_current_uri(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_current_path(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_escaped_current_uri(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_escaped_current_path(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_has_args(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_dollar(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_sec(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_hextime(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_ext(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_is_internal(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_is_subrequest(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_location_name(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_uint(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_connection_established_ts(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_request_ts(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
#if (NGX_HTTP_EXT)
static ngx_int_t ngx_http_extra_variable_request_time(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
#endif
static ngx_int_t ngx_http_extra_variable_hostname_uppercase(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_hostname_lowercase(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_time_rfc1123(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
#if (NGX_HAVE_TCP_INFO)
static ngx_int_t ngx_http_extra_variable_tcpinfo(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
#endif
#if (NGX_HTTP_EXT)
static ngx_int_t ngx_http_extra_variable_upstream_method(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
#endif
static ngx_int_t ngx_http_extra_variable_upstream_scheme(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_upstream_uri( ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_upstream_addr(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_upstream_status(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
#if (NGX_HTTP_EXT)
static ngx_int_t ngx_http_extra_variable_upstream_multi_ts(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_upstream_single_ts(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_upstream_multi_time(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
#endif
static ngx_int_t ngx_http_extra_variable_upstream_single_time(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
#if 0
static ngx_int_t ngx_http_extra_variable_upstream_multi_length(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);
#endif
static ngx_int_t ngx_http_extra_variable_upstream_single_length(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);


#if (NGX_HTTP_CACHE)
static ngx_int_t ngx_http_extra_variable_upstream_cache_key(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_upstream_cache_key_crc32(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_upstream_cache_key_hash(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_upstream_cache_main_hash(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_upstream_cache_variant_hash(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_upstream_cache_file(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_upstream_cache_age(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_upstream_cache_create_time(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_upstream_cache_create_date(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variables_check_cache_control(
    ngx_http_request_t *r);
static ngx_int_t ngx_http_extra_variables_check_accel_expires(
    ngx_http_request_t *r);
static time_t ngx_http_cache_get_expire_time(ngx_http_request_t *r);
static ngx_int_t ngx_http_extra_variable_upstream_cache_expire_time(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_upstream_cache_expire_date(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_upstream_cache_ttl(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_variable_upstream_cache_max_age(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);
#endif

static ngx_http_module_t  ngx_http_extra_variables_module_ctx = {
    ngx_http_add_extra_variables,               /* preconfiguration */
    NULL,                                       /* postconfiguration */

    NULL,                                       /* create main configuration */
    NULL,                                       /* init main configuration */

    NULL,                                       /* create server configuration */
    NULL,                                       /* merge server configuration */

    NULL,                                       /* create location configuration */
    NULL                                        /* merge location configuration */
};


ngx_module_t  ngx_http_extra_variables_module = {
    NGX_MODULE_V1,
    &ngx_http_extra_variables_module_ctx,       /* module context */
    NULL,                                       /* module directives */
    NGX_HTTP_MODULE,                            /* module type */
    NULL,                                       /* init master */
    NULL,                                       /* init module */
    NULL,                                       /* init process */
    NULL,                                       /* init thread */
    NULL,                                       /* exit thread */
    NULL,                                       /* exit process */
    NULL,                                       /* exit master */
    NGX_MODULE_V1_PADDING
};


static ngx_http_variable_t  ngx_http_extra_variables[] = {
    { ngx_string("request_path"), NULL,
      ngx_http_extra_variable_request_path,
      0, 0, 0 },

    { ngx_string("request_args"), NULL,
      ngx_http_extra_variable_request_args,
      0, 0, 0 },

    { ngx_string("request_is_args"), NULL,
      ngx_http_extra_variable_request_is_args,
      0, 0, 0 },

    { ngx_string("request_has_args"), NULL,
      ngx_http_extra_variable_request_has_args,
      0, 0, 0 },

    { ngx_string("current_uri"), NULL,
      ngx_http_extra_variable_current_uri, 0,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("current_path"), NULL,
      ngx_http_extra_variable_current_path, 0,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("escaped_current_uri"), NULL,
      ngx_http_extra_variable_escaped_current_uri, 0,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("escaped_current_path"), NULL,
      ngx_http_extra_variable_escaped_current_path, 0,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("has_args"), NULL,
      ngx_http_extra_variable_has_args,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("dollar"), NULL,
      ngx_http_extra_variable_dollar,
      0, 0, 0 },

    { ngx_string("sec"), NULL,
      ngx_http_extra_variable_sec, 
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("hextime"), NULL,
      ngx_http_extra_variable_hextime, 
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("ext"), NULL,
      ngx_http_extra_variable_ext, 
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("is_internal"), NULL,
      ngx_http_extra_variable_is_internal,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("is_subrequest"), NULL,
      ngx_http_extra_variable_is_subrequest,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("location_name"), NULL,
      ngx_http_extra_variable_location_name,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("redirect_count"), NULL,
      ngx_http_extra_variable_uint,
      NGX_HTTP_EXTRA_VARIABLE_REDIRECT_COUNT,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("subrequest_count"), NULL,
      ngx_http_extra_variable_uint,
      NGX_HTTP_EXTRA_VARIABLE_SUBREQUEST_COUNT,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("connection_established_ts"), NULL,
      ngx_http_extra_variable_connection_established_ts,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("request_created_ts"), NULL,
      ngx_http_extra_variable_request_ts,
      NGX_HTTP_EXTRA_VARIABLE_REQUEST_CREATED_TS,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

#if (NGX_HTTP_EXT)
    { ngx_string("response_header_sent_ts"), NULL,
      ngx_http_extra_variable_request_ts,
      NGX_HTTP_EXTRA_VARIABLE_RESPONSE_HEADER_SENT_TS,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("request_handling_time"), NULL,
      ngx_http_extra_variable_request_time,
      NGX_HTTP_EXTRA_VARIABLE_REQUEST_HANDlING_TIME,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("response_body_time"), NULL,
      ngx_http_extra_variable_request_time,
      NGX_HTTP_EXTRA_VARIABLE_RESPONSE_BODY_TIME,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },
#endif

    { ngx_string("hostname_uppercase"), NULL,
      ngx_http_extra_variable_hostname_uppercase,
      0, 0, 0 },

    { ngx_string("hostname_lowercase"), NULL,
      ngx_http_extra_variable_hostname_lowercase,
      0, 0, 0 },

    { ngx_string("time_rfc1123"), NULL,
      ngx_http_extra_variable_time_rfc1123,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

#if (NGX_HAVE_TCP_INFO)
    { ngx_string("tcpinfo_total_retrans"), NULL,
      ngx_http_extra_variable_tcpinfo,
      4, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("tcpinfo_pmtu"), NULL,
      ngx_http_extra_variable_tcpinfo,
      5, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("tcpinfo_rcv_ssthresh"), NULL,
      ngx_http_extra_variable_tcpinfo,
      6, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("tcpinfo_snd_ssthresh"), NULL,
      ngx_http_extra_variable_tcpinfo,
      7, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("tcpinfo_advmss"), NULL,
      ngx_http_extra_variable_tcpinfo,
      8, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("tcpinfo_reordering"), NULL,
      ngx_http_extra_variable_tcpinfo,
      9, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("tcpinfo_rcv_rtt"), NULL,
      ngx_http_extra_variable_tcpinfo,
      10, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("tcpinfo_last_data_sent"), NULL,
      ngx_http_extra_variable_tcpinfo,
      11, NGX_HTTP_VAR_NOCACHEABLE, 0 },
#endif

#if (NGX_HTTP_EXT)
    { ngx_string("upstream_method"), NULL,
      ngx_http_extra_variable_upstream_method, 0,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },
#endif

    { ngx_string("upstream_scheme"), NULL,
      ngx_http_extra_variable_upstream_scheme, 0,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_uri"), NULL,
      ngx_http_extra_variable_upstream_uri, 0,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_last_addr"), NULL,
      ngx_http_extra_variable_upstream_addr, 0,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_last_status"), NULL,
      ngx_http_extra_variable_upstream_status, 0,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

#if (NGX_HTTP_EXT)
    { ngx_string("upstream_start_ts"), NULL,
      ngx_http_extra_variable_upstream_multi_ts,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_START_TS,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_last_start_ts"), NULL,
      ngx_http_extra_variable_upstream_single_ts,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_START_TS,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

#if (NGX_HTTP_SSL)
    { ngx_string("upstream_ssl_start_ts"), NULL,
      ngx_http_extra_variable_upstream_multi_ts,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_SSL_START_TS,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_last_ssl_start_ts"), NULL,
      ngx_http_extra_variable_upstream_single_ts,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_SSL_START_TS,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },
#endif

    { ngx_string("upstream_send_start_ts"), NULL,
      ngx_http_extra_variable_upstream_multi_ts,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_SEND_START_TS,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_last_send_start_ts"), NULL,
      ngx_http_extra_variable_upstream_single_ts,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_SEND_START_TS,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_send_end_ts"), NULL,
      ngx_http_extra_variable_upstream_multi_ts,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_SEND_END_TS,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_last_send_end_ts"), NULL,
      ngx_http_extra_variable_upstream_single_ts,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_SEND_END_TS,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },


    { ngx_string("upstream_header_ts"), NULL,
      ngx_http_extra_variable_upstream_multi_ts,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_HEADER_TS,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_last_header_ts"), NULL,
      ngx_http_extra_variable_upstream_single_ts,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_HEADER_TS,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_end_ts"), NULL,
      ngx_http_extra_variable_upstream_multi_ts,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_END_TS,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_last_end_ts"), NULL,
      ngx_http_extra_variable_upstream_single_ts,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_END_TS,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },
#endif

#if 0
    { ngx_string("upstream_connect_time"), NULL,
      ngx_http_extra_variable_upstream_multi_time,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_CONNECT_TIME,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },
#endif

    { ngx_string("upstream_last_connect_time"), NULL,
      ngx_http_extra_variable_upstream_single_time,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_CONNECT_TIME,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

#if (NGX_HTTP_EXT)
    { ngx_string("upstream_transport_connect_time"), NULL,
      ngx_http_extra_variable_upstream_multi_time,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_TRANSPORT_CONNECT_TIME,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_last_transport_connect_time"), NULL,
      ngx_http_extra_variable_upstream_single_time,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_TRANSPORT_CONNECT_TIME,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

#if (NGX_HTTP_SSL)
    { ngx_string("upstream_ssl_time"), NULL,
      ngx_http_extra_variable_upstream_multi_time,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_SSL_TIME,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_last_ssl_time"), NULL,
      ngx_http_extra_variable_upstream_single_time,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_SSL_TIME,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },
#endif

    { ngx_string("upstream_send_time"), NULL,
      ngx_http_extra_variable_upstream_multi_time,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_SEND_TIME,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_last_send_time"), NULL,
      ngx_http_extra_variable_upstream_single_time,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_SEND_TIME,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_read_time"), NULL,
      ngx_http_extra_variable_upstream_multi_time,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_READ_TIME,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_last_read_time"), NULL,
      ngx_http_extra_variable_upstream_single_time,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_READ_TIME,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },
#endif

#if 0
    { ngx_string("upstream_header_time"), NULL,
      ngx_http_extra_variable_upstream_multi_time,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_HEADER_TIME,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },
#endif

    { ngx_string("upstream_last_header_time"), NULL,
      ngx_http_extra_variable_upstream_single_time,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_HEADER_TIME,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

#if 0
    { ngx_string("upstream_response_time"), NULL,
      ngx_http_extra_variable_upstream_multi_time,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_RESPONSE_TIME,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },
#endif

    { ngx_string("upstream_last_response_time"), NULL,
      ngx_http_extra_variable_upstream_single_time,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_RESPONSE_TIME,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

#if 0
    { ngx_string("upstream_response_length"), NULL,
      ngx_http_extra_variable_upstream_multi_length,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_RESPONSE_LENGTH,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },
#endif

    { ngx_string("upstream_last_response_length"), NULL,
      ngx_http_extra_variable_upstream_single_length,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_RESPONSE_LENGTH,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

#if 0
    { ngx_string("upstream_bytes_received"), NULL,
      ngx_http_extra_variable_upstream_multi_length,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_BYTES_RECEIVED,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },
#endif

    { ngx_string("upstream_last_bytes_received"), NULL,
      ngx_http_extra_variable_upstream_single_length,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_BYTES_RECEIVED,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

#if 0
    { ngx_string("upstream_bytes_sent"), NULL,
      ngx_http_extra_variable_upstream_multi_length,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_BYTES_SENT,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },
#endif

    { ngx_string("upstream_last_bytes_sent"), NULL,
      ngx_http_extra_variable_upstream_single_length,
      NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_BYTES_SENT,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

#if (NGX_HTTP_CACHE)
    { ngx_string("upstream_cache_key"), NULL,
      ngx_http_extra_variable_upstream_cache_key,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_cache_key_crc32"), NULL,
      ngx_http_extra_variable_upstream_cache_key_crc32,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_cache_key_hash"), NULL,
      ngx_http_extra_variable_upstream_cache_key_hash,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_cache_main_hash"), NULL,
      ngx_http_extra_variable_upstream_cache_main_hash,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_cache_variant_hash"), NULL,
      ngx_http_extra_variable_upstream_cache_variant_hash,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_cache_file"), NULL,
      ngx_http_extra_variable_upstream_cache_file,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_cache_age"), NULL,
      ngx_http_extra_variable_upstream_cache_age,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_cache_create_time"), NULL,
      ngx_http_extra_variable_upstream_cache_create_time,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_cache_create_date"), NULL,
      ngx_http_extra_variable_upstream_cache_create_date,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_cache_expire_time"), NULL,
      ngx_http_extra_variable_upstream_cache_expire_time,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_cache_expire_date"), NULL,
      ngx_http_extra_variable_upstream_cache_expire_date,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_cache_ttl"), NULL,
      ngx_http_extra_variable_upstream_cache_ttl,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_cache_max_age"), NULL,
      ngx_http_extra_variable_upstream_cache_max_age,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },
#endif

    { ngx_null_string, NULL, NULL, 0, 0, 0 }
};


static ngx_int_t
ngx_http_add_extra_variables(ngx_conf_t *cf)
{
    ngx_http_variable_t  *var, *v;

    for (v = ngx_http_extra_variables; v->name.len; v++) {
        var = ngx_http_add_variable(cf, &v->name, v->flags);
        if (var == NULL) {
            return NGX_ERROR;
        }

        var->get_handler = v->get_handler;
        var->data = v->data;
    }

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_request_path(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char *p;

    p = ngx_strlchr(r->unparsed_uri.data,
        r->unparsed_uri.data + r->unparsed_uri.len, '?');

    if (p) {
        v->data = r->unparsed_uri.data;
        v->len = p - r->unparsed_uri.data;

    } else {
        v->data = r->unparsed_uri.data;
        v->len = r->unparsed_uri.len;
    }


    v->data = ngx_pnalloc(r->pool, v->len + 1);
    if (v->data == NULL) {
        return NGX_ERROR;
    }

    ngx_memcpy(v->data, r->unparsed_uri.data, v->len);
    v->data[v->len] = '\0';

    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_request_args(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char *p;

    p = ngx_strlchr(r->unparsed_uri.data,
                    r->unparsed_uri.data + r->unparsed_uri.len, '?');

    if (p) {
        v->data = p + 1;
        v->len = r->unparsed_uri.data + r->unparsed_uri.len - v->data;

        u_char *args = ngx_pnalloc(r->pool, v->len + 1);
        if (args == NULL) {
            return NGX_ERROR;
        }

        ngx_memcpy(args, v->data, v->len);
        args[v->len] = '\0';

        v->data = args;
        v->valid = 1;
        v->no_cacheable = 0;
        v->not_found = 0;

    } else {
        *v = ngx_http_variable_null_value;
    }

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_request_is_args(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char *p;

    p = ngx_strlchr(r->unparsed_uri.data,
        r->unparsed_uri.data + r->unparsed_uri.len, '?');

    if (p) {
        v->len = 1;
        v->valid = 1;
        v->no_cacheable = 0;
        v->not_found = 0;
        v->data = (u_char *) "?";
    } else {
        *v = ngx_http_variable_null_value;
    }

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_request_has_args(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char *p;

    p = ngx_strlchr(r->unparsed_uri.data,
        r->unparsed_uri.data + r->unparsed_uri.len, '?');

    if (p) {
        v->data = (u_char *) "&";
    } else {
        v->data = (u_char *) "?";
    }

    v->len = 1;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_current_uri(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char *p;

    v->len = r->uri.len + (r->args.len ? 1 + r->args.len : 0);
    v->data = ngx_pnalloc(r->pool, v->len);
    if (v->data == NULL) {
        return NGX_ERROR;
    }

    p = ngx_copy(v->data, r->uri.data, r->uri.len);
    if (r->args.len) {
        *p++ = '?';
        ngx_memcpy(p, r->args.data, r->args.len);
    }

    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_current_path(ngx_http_request_t *r, 
    ngx_http_variable_value_t *v, uintptr_t data)
{
    ngx_str_t *s = &r->uri;

    if (s->data) {
        v->len = s->len;
        v->valid = 1;
        v->no_cacheable = 0;
        v->not_found = 0;
        v->data = s->data;

    } else {
        v->not_found = 1;
    }

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_escaped_current_uri(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    size_t      len;
    u_char     *p;
    uintptr_t   escape;

    if (r == NULL || r->uri.data == NULL) {
        v->not_found = 1;
        return NGX_OK;
    }

    escape = ngx_escape_uri(NULL, r->uri.data, r->uri.len, NGX_ESCAPE_URI);

    len = r->uri.len + escape * 2;

    if (r->args.len > 0) {
        len += 1 + r->args.len;
    }

    v->data = ngx_pnalloc(r->pool, len + 1);
    if (v->data == NULL) {
        v->not_found = 1;
        return NGX_OK;
    }

    p = v->data;

    if (escape) {
        ngx_escape_uri(p, r->uri.data, r->uri.len, NGX_ESCAPE_URI);
        p += r->uri.len + escape * 2;
    } else {
        p = ngx_copy(p, r->uri.data, r->uri.len);
    }

    if (r->args.len > 0) {
        *p++ = '?';
        p = ngx_copy(p, r->args.data, r->args.len);
    }

    *p = '\0';

    v->len = p - v->data;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_escaped_current_path(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    size_t      len;
    u_char     *p;
    uintptr_t   escape;

    if (r == NULL || r->uri.data == NULL) {
        v->not_found = 1;
        return NGX_OK;
    }

    escape = ngx_escape_uri(NULL, r->uri.data, r->uri.len, NGX_ESCAPE_URI);

    len = r->uri.len + escape * 2;

    v->data = ngx_pnalloc(r->pool, len + 1);
    if (v->data == NULL) {
        v->not_found = 1;
        return NGX_OK;
    }

    p = v->data;

    if (escape) {
        ngx_escape_uri(p, r->uri.data, r->uri.len, NGX_ESCAPE_URI);
        p += r->uri.len + escape * 2;
    } else {
        p = ngx_copy(p, r->uri.data, r->uri.len);
    }

    *p = '\0';

    v->len = p - v->data;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}


static ngx_int_t 
ngx_http_extra_variable_dollar(ngx_http_request_t *r, 
    ngx_http_variable_value_t *v, uintptr_t data)
{
    v->len = 1;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = (u_char *) "$";

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_sec(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char      *p;
    ngx_time_t  *tp;

    p = ngx_pnalloc(r->pool, NGX_TIME_T_LEN);
    if (p == NULL) {
        return NGX_ERROR;
    }

    tp = ngx_timeofday();

    v->len = ngx_sprintf(p, "%T", tp->sec) - p;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = p;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_hextime(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char      *p;
    ngx_time_t  *tp;
    uint32_t     timestamp;

    p = ngx_pnalloc(r->pool, NGX_TIME_T_LEN);
    if (p == NULL) {
        return NGX_ERROR;
    }

    tp = ngx_timeofday();
    timestamp = (uint32_t) tp->sec;

    v->len = ngx_sprintf(p, "%xi", timestamp) - p;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = p;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_ext(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    v->len = r->exten.len;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = r->exten.data;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_has_args(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    if (r->args.len == 0) {
        v->data = (u_char *) "?";
    } else {
        v->data = (u_char *) "&";
    }

    v->len = 1;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_is_internal(ngx_http_request_t *r,
                                ngx_http_variable_value_t *v, uintptr_t data)
{
    if (r->internal) {
        v->data = (u_char *) "1";
    } else {
        v->data = (u_char *) "0";
    }

    v->len = 1;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_is_subrequest(ngx_http_request_t *r,
                                ngx_http_variable_value_t *v, uintptr_t data)
{
    if (r->parent) {
        v->data = (u_char *) "1";
    } else {
        v->data = (u_char *) "0";
    }

    v->len = 1;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_location_name(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    ngx_http_core_loc_conf_t  *clcf;

    clcf = ngx_http_get_module_loc_conf(r, ngx_http_core_module);

    v->len = clcf->name.len;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = clcf->name.data;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_uint(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    ngx_uint_t  value;
    u_char     *p;

    p = ngx_pnalloc(r->pool, NGX_INT_T_LEN);
    if (p == NULL) {
        return NGX_ERROR;
    }

    switch (data) {
    case NGX_HTTP_EXTRA_VARIABLE_REDIRECT_COUNT:
        value = NGX_HTTP_MAX_URI_CHANGES + 1 - r->uri_changes;
        break;

    case NGX_HTTP_EXTRA_VARIABLE_SUBREQUEST_COUNT:
        value = NGX_HTTP_MAX_SUBREQUESTS + 1 - r->subrequests;
        break;

    default:
        v->not_found = 1;
        return NGX_OK;
    }

    v->len = ngx_sprintf(p, "%ui", value) - p;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = p;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_connection_established_ts(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char          *p;
    ngx_time_t      *tp;
    ngx_msec_t       ms;

    p = ngx_pnalloc(r->pool, NGX_TIME_T_LEN + 4);
    if (p == NULL) {
        return NGX_ERROR;
    }

    tp = ngx_timeofday();

    ms = (ngx_msec_t)
            (tp->sec * 1000 + tp->msec
                + r->connection->start_time - ngx_current_msec);

    v->len = ngx_sprintf(p, "%T.%03M", (time_t) ms / 1000, ms % 1000) - p;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = p;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_request_ts(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char         *p;

    p = ngx_pnalloc(r->pool, NGX_TIME_T_LEN + 4);
    if (p == NULL) {
        return NGX_ERROR;
    }

    switch (data) {
    case NGX_HTTP_EXTRA_VARIABLE_REQUEST_CREATED_TS:
        if (!r->start_sec || !r->start_msec) {
            v->not_found = 1;
            return NGX_OK;
        }
        v->len = ngx_sprintf(p, "%T.%03M", r->start_sec, r->start_msec) - p;
        break;

#if (NGX_HTTP_EXT)
    case NGX_HTTP_EXTRA_VARIABLE_RESPONSE_HEADER_SENT_TS:
        if (!r->header_sent_sec || !r->header_sent_msec) {
            v->not_found = 1;
            return NGX_OK;
        }
        v->len = ngx_sprintf(p, "%T.%03M",
                                r->header_sent_sec, r->header_sent_msec) - p;
        break;
#endif

    default:
        v->not_found = 1;
        return NGX_OK;
    }

    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = p;

    return NGX_OK;
}


#if (NGX_HTTP_EXT)
static ngx_int_t
ngx_http_extra_variable_request_time(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char          *p;
    ngx_time_t      *tp;
    ngx_msec_int_t   ms;

    p = ngx_pnalloc(r->pool, NGX_TIME_T_LEN + 4);
    if (p == NULL) {
        return NGX_ERROR;
    }

    tp = ngx_timeofday();

    if (!r->header_sent_sec || !r->header_sent_msec) {
        v->not_found = 1;
        return NGX_OK;
    }

    switch (data) {
    case NGX_HTTP_EXTRA_VARIABLE_REQUEST_HANDlING_TIME:
        ms = (ngx_msec_int_t) 
                 ((r->header_sent_sec - r->start_sec) * 1000
                    + (r->header_sent_msec - r->start_msec));
        break;

    case NGX_HTTP_EXTRA_VARIABLE_RESPONSE_BODY_TIME:
        ms = (ngx_msec_int_t) 
                 ((tp->sec - r->header_sent_sec ) * 1000
                    + (tp->msec - r->header_sent_msec));
        break;

    default:
        v->not_found = 1;
        return NGX_OK;
    }

    ms = ngx_max(ms, 0);

    v->len = ngx_sprintf(p, "%T.%03M", (time_t) ms / 1000, ms % 1000) - p;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = p;

    return NGX_OK;
}
#endif


static ngx_int_t
ngx_http_extra_variable_hostname_uppercase(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char *p, *dst;
    size_t len = ngx_cycle->hostname.len;

    v->len = len;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    v->data = ngx_pnalloc(r->pool, len);
    if (v->data == NULL) {
        return NGX_ERROR;
    }

    dst = v->data;
    p = ngx_cycle->hostname.data;

    for (size_t i = 0; i < len; i++) {
        dst[i] = ngx_toupper(p[i]);
    }

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_hostname_lowercase(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char *p, *dst;
    size_t len = ngx_cycle->hostname.len;

    v->len = len;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    v->data = ngx_pnalloc(r->pool, len);
    if (v->data == NULL) {
        return NGX_ERROR;
    }

    dst = v->data;
    p = ngx_cycle->hostname.data;

    for (size_t i = 0; i < len; i++) {
        dst[i] = ngx_tolower(p[i]);
    }

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_time_rfc1123(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char  *p;

    p = ngx_pnalloc(r->pool, ngx_cached_http_time.len);
    if (p == NULL) {
        return NGX_ERROR;
    }

    ngx_memcpy(p, ngx_cached_http_time.data,
               ngx_cached_http_time.len);

    v->len = ngx_cached_http_time.len;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = p;

    return NGX_OK;
}


#if (NGX_HAVE_TCP_INFO)
static ngx_int_t
ngx_http_extra_variable_tcpinfo(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    struct tcp_info  ti;
    socklen_t        len;
    uint32_t         value;

    len = sizeof(struct tcp_info);
    if (getsockopt(r->connection->fd, IPPROTO_TCP, TCP_INFO, &ti, &len) == -1) {
        v->not_found = 1;
        return NGX_OK;
    }

    v->data = ngx_pnalloc(r->pool, NGX_INT32_LEN);
    if (v->data == NULL) {
        return NGX_ERROR;
    }

    switch (data) {
    case 4:
        value = ti.tcpi_total_retrans;
        break;

    case 5:
        value = ti.tcpi_pmtu;
        break;

    case 6:
        value = ti.tcpi_rcv_ssthresh;
        break;

    case 7:
        value = ti.tcpi_snd_ssthresh;
        break;

    case 8:
        value = ti.tcpi_advmss;
        break;

    case 9:
        value = ti.tcpi_reordering;
        break;

    case 10:
        value = ti.tcpi_rcv_rtt;
        break;

    case 11:
        value = ti.tcpi_last_data_sent;
        break;

    /* suppress warning */
    default:
        value = 0;
        break;
    }

    v->len = ngx_sprintf(v->data, "%uD", value) - v->data;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}
#endif


static ngx_int_t
ngx_http_extra_variable_upstream_method(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    ngx_http_upstream_t *u;

    u = r->upstream;

    if (u && u->method_name.len > 0) {
        v->len = u->method_name.len;
        v->data = u->method_name.data;
        v->valid = 1;
        v->no_cacheable = 0;
        v->not_found = 0;
    } else {
        v->not_found = 1;
    }

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_upstream_scheme(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    ngx_http_upstream_t *u;
    ngx_str_t scheme;

    u = r->upstream;

    if (u && u->schema.len > 0) {
        scheme.data = u->schema.data;
        scheme.len = u->schema.len;

        if (scheme.len > 3 && ngx_strncmp(&scheme.data[scheme.len - 3], "://", 3) == 0) {
            scheme.len -= 3;
        }

        v->len = scheme.len;
        v->data = scheme.data;
        v->valid = 1;
        v->no_cacheable = 0;
        v->not_found = 0;
    } else {
        v->not_found = 1;
    }

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_upstream_uri(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    ngx_http_upstream_t     *u;
    ngx_str_t                uri;

    u = r->upstream;

    if (u && u->uri.len > 0) {
        uri.len = u->uri.len;
        uri.data = ngx_pnalloc(r->pool, uri.len);
        if (uri.data == NULL) {
            return NGX_ERROR;
        }

        ngx_memcpy(uri.data, u->uri.data, uri.len);

        v->len = uri.len;
        v->data = uri.data;
        v->valid = 1;
        v->no_cacheable = 0;
        v->not_found = 0;
    } else {
        v->not_found = 1;
    }

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_upstream_addr(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    ngx_http_upstream_state_t  *state;

    if (r->upstream && r->upstream->state && r->upstream->state->peer) {
        v->len = r->upstream->state->peer->len;
        v->data = ngx_pnalloc(r->pool, v->len);
        if (v->data == NULL) {
            return NGX_ERROR;
        }
        ngx_memcpy(v->data, r->upstream->state->peer->data, v->len);
        v->valid = 1;
        v->no_cacheable = 0;
        v->not_found = 0;
        return NGX_OK;
    }

    if (r->upstream_states == NULL || r->upstream_states->nelts == 0) {
        v->not_found = 1;
        return NGX_OK;
    }

    state = r->upstream_states->elts;
    state = &state[r->upstream_states->nelts - 1];
    
    if (state->peer) {
        v->len = state->peer->len;
        v->data = ngx_pnalloc(r->pool, v->len);
        if (v->data == NULL) {
            return NGX_ERROR;
        }
        ngx_memcpy(v->data, state->peer->data, v->len);

    } else {
        v->not_found = 1;
        return NGX_OK;
    }

    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_upstream_status(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    ngx_http_upstream_state_t  *state;

    if (r->upstream && r->upstream->state && r->upstream->state->status) {
        state = r->upstream->state;

    } else {
        if (r->upstream_states == NULL || r->upstream_states->nelts == 0) {
            v->not_found = 1;
            return NGX_OK;
        }
        state = r->upstream_states->elts;
        state = &state[r->upstream_states->nelts - 1];
    }

    if (!state->status) {
        v->not_found = 1;
        return NGX_OK;
    }

    v->data = ngx_pnalloc(r->pool, NGX_INT_T_LEN);
    if (v->data == NULL) {
        return NGX_ERROR;
    }

    v->len = ngx_sprintf(v->data, "%ui", state->status) - v->data;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}


#if (NGX_HTTP_EXT)
static ngx_int_t
ngx_http_extra_variable_upstream_multi_ts(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char                     *p;
    ngx_time_t                 *tp;
    size_t                      len;
    ngx_uint_t                  i;
    ngx_msec_t                  ms;
    ngx_http_upstream_state_t  *state;

    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    if (r->upstream_states == NULL || r->upstream_states->nelts == 0) {
        v->not_found = 1;
        return NGX_OK;
    }

    len = r->upstream_states->nelts * (NGX_TIME_T_LEN + 4 + 2);

    p = ngx_pnalloc(r->pool, len);
    if (p == NULL) {
        return NGX_ERROR;
    }

    v->data = p;

    i = 0;
    state = r->upstream_states->elts;

    tp = ngx_timeofday();

    for ( ;; ) {

        switch (data) {

        case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_START_TS:
            ms = state[i].start_msec;
            break;

#if (NGX_HTTP_SSL)
        case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_SSL_START_TS:
            ms = state[i].ssl_start_msec;
            break;
#endif

        case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_SEND_START_TS:
            ms = state[i].send_start_msec;
            break;

        case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_SEND_END_TS:
            ms = state[i].send_end_msec;
            break;

        case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_HEADER_TS:
            if (state[i].header_time == (ngx_msec_t) -1) {
                ms = (ngx_msec_t) -1;
            } else {
                ms = (ngx_msec_t) (state[i].start_msec + state[i].header_time);
            }
            break;

        case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_END_TS:
            ms = (ngx_msec_t) (state[i].start_msec + state[i].response_time);
            break;

        default:
            v->not_found = 1;
            return NGX_OK;
        }

        if (ms != (ngx_msec_t) -1) {
            ms = (ngx_msec_t)
                (tp->sec * 1000 + tp->msec + ms - ngx_current_msec);
            p = ngx_sprintf(p, "%T.%03M", (time_t) ms / 1000, ms % 1000);

        } else {
            *p++ = '-';
        }

        if (++i == r->upstream_states->nelts) {
            break;
        }

        if (state[i].peer) {
            *p++ = ',';
            *p++ = ' ';

        } else {
            *p++ = ' ';
            *p++ = ':';
            *p++ = ' ';

            if (++i == r->upstream_states->nelts) {
                break;
            }

            continue;
        }
    }

    v->len = p - v->data;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_upstream_single_ts(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char                     *p;
    ngx_time_t                 *tp;
    ngx_msec_t                  ms;
    ngx_http_upstream_state_t  *state;

    if (r->upstream && r->upstream->state) {
        state = r->upstream->state;

    } else {
        if (r->upstream_states == NULL || r->upstream_states->nelts == 0) {
            v->not_found = 1;
            return NGX_OK;
        }
        state = r->upstream_states->elts;
        state = &state[r->upstream_states->nelts - 1];
    }

    tp = ngx_timeofday();

    switch (data) {

    case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_START_TS:
        ms = state->start_msec;
        break;

#if (NGX_HTTP_SSL)
    case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_SSL_START_TS:
        ms = state->ssl_start_msec;
        break;
#endif

    case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_SEND_START_TS:
        ms = state->send_start_msec;
        break;

    case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_SEND_END_TS:
        ms = state->send_end_msec;
        break;

    case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_HEADER_TS:
        if (state->header_time == (ngx_msec_t) -1) {
            ms = (ngx_msec_t) -1;
        } else {
            ms = (ngx_msec_t) (state->start_msec + state->header_time);
        }
        break;

    case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_END_TS:
        ms = (ngx_msec_t) (state->start_msec + state->response_time);
        break;

    default:
        v->not_found = 1;
        return NGX_OK;
    }

    if (ms != (ngx_msec_t) -1) {
        ms = (ngx_msec_t)(tp->sec * 1000 + tp->msec + ms - ngx_current_msec);

        p = ngx_pnalloc(r->pool, NGX_TIME_T_LEN + 4);
        if (p == NULL) {
            return NGX_ERROR;
        }

        v->data = p;
        p = ngx_sprintf(p, "%T.%03M", (time_t) ms / 1000, ms % 1000);
        v->len = p - v->data;

    } else {
        v->not_found = 1;
        return NGX_OK;
    }

    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_upstream_multi_time(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char                     *p;
    size_t                      len;
    ngx_uint_t                  i;
    ngx_msec_int_t              ms;
    ngx_http_upstream_state_t  *state;

    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    if (r->upstream_states == NULL || r->upstream_states->nelts == 0) {
        v->not_found = 1;
        return NGX_OK;
    }

    len = r->upstream_states->nelts * (NGX_TIME_T_LEN + 4 + 2);

    p = ngx_pnalloc(r->pool, len);
    if (p == NULL) {
        return NGX_ERROR;
    }

    v->data = p;

    i = 0;
    state = r->upstream_states->elts;

    for ( ;; ) {

        switch (data) {

        /* NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_CONNECT_TIME */

#if (NGX_HTTP_SSL)
        case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_TRANSPORT_CONNECT_TIME:
            if (state[i].ssl_start_msec == (ngx_msec_t) -1) {
                ms = state[i].connect_time;
            } else {
                ms = (ngx_msec_t) (state[i].ssl_start_msec - state[i].start_msec);
            }
            break;

        case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_SSL_TIME:
            if (state[i].ssl_start_msec == (ngx_msec_t) -1) {
                ms = (ngx_msec_t) -1;
            } else if (state[i].send_start_msec == (ngx_msec_t) -1) {
                ms = (ngx_msec_t) (state[i].start_msec + state[i].response_time - state[i].ssl_start_msec);
            } else {
                ms = (ngx_msec_t) (state[i].send_start_msec - state[i].ssl_start_msec);
            }
            break;
#else
        case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_TRANSPORT_CONNECT_TIME:
            ms = state[i].connect_time;
            break;
#endif

        case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_SEND_TIME:
            if (state[i].send_start_msec == (ngx_msec_t) -1) {
                ms = (ngx_msec_t) -1;
            } else if (state[i].send_end_msec == (ngx_msec_t) -1) {
                ms = (ngx_msec_t) (state[i].start_msec + state[i].response_time - state[i].send_start_msec);
            } else {
                ms = (ngx_msec_t) (state[i].send_end_msec - state[i].send_start_msec);
            }
            break;

        case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_READ_TIME:
            if (state[i].send_end_msec == (ngx_msec_t) -1) {
                ms = (ngx_msec_t) -1;
            } else {
                ms = (ngx_msec_t) (state[i].start_msec + state[i].response_time - state[i].send_end_msec);
            }
            break;

        /* NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_HEADER_TIME */

        /* NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_RESPONSE_TIME */

        default:
            v->not_found = 1;
            return NGX_OK;
        }

        if (ms != -1) {
            ms = ngx_max(ms, 0);
            p = ngx_sprintf(p, "%T.%03M", (time_t) ms / 1000, ms % 1000);

        } else {
            *p++ = '-';
        }

        if (++i == r->upstream_states->nelts) {
            break;
        }

        if (state[i].peer) {
            *p++ = ',';
            *p++ = ' ';

        } else {
            *p++ = ' ';
            *p++ = ':';
            *p++ = ' ';

            if (++i == r->upstream_states->nelts) {
                break;
            }

            continue;
        }
    }

    v->len = p - v->data;

    return NGX_OK;
}
#endif


static ngx_int_t
ngx_http_extra_variable_upstream_single_time(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char                     *p;
    ngx_msec_int_t              ms;
    ngx_http_upstream_state_t  *state;

    if (r->upstream && r->upstream->state) {
        state = r->upstream->state;

    } else {
        if (r->upstream_states == NULL || r->upstream_states->nelts == 0) {
            v->not_found = 1;
            return NGX_OK;
        }
        state = r->upstream_states->elts;
        state = &state[r->upstream_states->nelts - 1];
    }

    switch (data) {

    case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_CONNECT_TIME:
        ms = state->connect_time;
        break;

#if (NGX_HTTP_EXT)
#if (NGX_HTTP_SSL)
    case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_TRANSPORT_CONNECT_TIME:
        if (state->ssl_start_msec == (ngx_msec_t) -1) {
            ms = state->connect_time;
        } else {
            ms = (ngx_msec_t) (state->ssl_start_msec - state->start_msec);
        }
        break;

    case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_SSL_TIME:
        if (state->ssl_start_msec == (ngx_msec_t) -1) {
            ms = (ngx_msec_t) -1;
        } else if (state->send_start_msec == (ngx_msec_t) -1) {
            ms = (ngx_msec_t) (state->start_msec + state->response_time - state->ssl_start_msec);
        } else {
            ms = (ngx_msec_t) (state->send_start_msec - state->ssl_start_msec);
        }
        break;
#else
    case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_TRANSPORT_CONNECT_TIME:
        ms = state->connect_time;
        break;
#endif

    case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_SEND_TIME:
        if (state->send_start_msec == (ngx_msec_t) -1) {
            ms = (ngx_msec_t) -1;
        } else if (state->send_end_msec == (ngx_msec_t) -1) {
            ms = (ngx_msec_t) (state->start_msec + state->response_time - state->send_start_msec);
        } else {
            ms = (ngx_msec_t) (state->send_end_msec - state->send_start_msec);
        }
        break;

    case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_READ_TIME:
        if (state->send_end_msec == (ngx_msec_t) -1) {
            ms = (ngx_msec_t) -1;
        } else {
            ms = (ngx_msec_t) (state->start_msec + state->response_time - state->send_end_msec);
        }
        break;
#endif

    case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_HEADER_TIME:
        ms = state->header_time;
        break;
        
    case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_RESPONSE_TIME:
        ms = state->response_time;
        break;

    default:
        v->not_found = 1;
        return NGX_OK;
    }

    if (ms != -1) {
        ms = ngx_max(ms, 0);

        p = ngx_pnalloc(r->pool, NGX_TIME_T_LEN + 4);
        if (p == NULL) {
            return NGX_ERROR;
        }

        v->data = p;
        p = ngx_sprintf(p, "%T.%03M", (time_t) ms / 1000, ms % 1000);
        v->len = p - v->data;

    } else {
        v->not_found = 1;
        return NGX_OK;
    }

    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}


#if 0
static ngx_int_t
ngx_http_extra_variable_upstream_multi_length(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char                     *p;
    size_t                      len;
    ngx_uint_t                  i;
    ngx_http_upstream_state_t  *state;

    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    if (r->upstream_states == NULL || r->upstream_states->nelts == 0) {
        v->not_found = 1;
        return NGX_OK;
    }

    len = r->upstream_states->nelts * (NGX_TIME_T_LEN + 4 + 2);

    p = ngx_pnalloc(r->pool, len);
    if (p == NULL) {
        return NGX_ERROR;
    }

    v->data = p;

    i = 0;
    state = r->upstream_states->elts;

    for ( ;; ) {

        switch(data) {

            /* NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_RESPONSE_LENGTH */

            /* NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_BYTES_RECEIVED */
            
            /* NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_BYTES_SENT */

            default:
                v->not_found = 1;
                return NGX_OK;
        }

        if (++i == r->upstream_states->nelts) {
            break;
        }

        if (state[i].peer) {
            *p++ = ',';
            *p++ = ' ';

        } else {
            *p++ = ' ';
            *p++ = ':';
            *p++ = ' ';

            if (++i == r->upstream_states->nelts) {
                break;
            }

            continue;
        }
    }

    v->len = p - v->data;

    return NGX_OK;
}
#endif


static ngx_int_t
ngx_http_extra_variable_upstream_single_length(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    ngx_http_upstream_state_t *state;
    u_char *p;

    if (r->upstream && r->upstream->state) {
        state = r->upstream->state;

    } else {
        if (r->upstream_states == NULL || r->upstream_states->nelts == 0) {
            v->not_found = 1;
            return NGX_OK;
        }
        state = r->upstream_states->elts;
        state = &state[r->upstream_states->nelts - 1];
    }

    p = ngx_pnalloc(r->pool, NGX_OFF_T_LEN);
    if (p == NULL) {
        return NGX_ERROR;
    }

    v->data = p;

    switch(data) {

    case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_RESPONSE_LENGTH:
        p = ngx_sprintf(p, "%O", state->response_length);
        break;

    case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_BYTES_RECEIVED:
        p = ngx_sprintf(p, "%O", state->bytes_received);
        break;
    
    case NGX_HTTP_EXTRA_VARIABLES_UPSTREAM_BYTES_SENT:
        p = ngx_sprintf(p, "%O", state->bytes_sent);
        break;

    default:
        v->not_found = 1;
        return NGX_OK;
    }

    v->len = p - v->data;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}


#if (NGX_HTTP_CACHE)
static ngx_int_t
ngx_http_extra_variable_upstream_cache_key(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char            *p;
    size_t             len;
    ngx_str_t         *key;
    ngx_uint_t         i;
    ngx_http_cache_t  *c;

    if (r->cache == NULL || r->cache->keys.nelts == 0) {
        v->not_found = 1;
        return NGX_OK;
    }

    c = r->cache;

    len = 0;
    key = c->keys.elts;

    for (i = 0; i < c->keys.nelts; i++) {
        len += key[i].len;
    }

    p = ngx_pnalloc(r->pool, len);
    if (p == NULL) {
        return NGX_ERROR;
    }

    v->len = len;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = p;

    for (i = 0; i < c->keys.nelts; i++) {
        p = ngx_cpymem(p, key[i].data, key[i].len);
    }

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_upstream_cache_key_crc32(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char *crc32_str;

    if (r->cache == NULL) {
        v->not_found = 1;
        return NGX_OK;
    }

    crc32_str = ngx_pnalloc(r->pool, 8 + 1);
    if (crc32_str == NULL) {
        return NGX_ERROR;
    }

    ngx_sprintf(crc32_str, "%08xD", r->cache->crc32);

    v->data = crc32_str;
    v->len = 8;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_upstream_cache_key_hash(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char *key_hash;
    size_t  key_len;

    if (r->cache == NULL) {
        v->not_found = 1;
        return NGX_OK;
    }

    key_len = NGX_HTTP_CACHE_KEY_LEN;

    key_hash = ngx_pnalloc(r->pool, key_len * 2 + 1);
    if (key_hash == NULL) {
        return NGX_ERROR;
    }

    ngx_hex_dump(key_hash, r->cache->key, key_len);

    key_hash[key_len * 2] = '\0';

    v->data = key_hash;
    v->len = key_len * 2;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_upstream_cache_main_hash(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char *main_hash;
    size_t  main_len;

    if (r->cache == NULL) {
        v->not_found = 1;
        return NGX_OK;
    }

    main_len = NGX_HTTP_CACHE_KEY_LEN;

    main_hash = ngx_pnalloc(r->pool, main_len * 2 + 1);
    if (main_hash == NULL) {
        return NGX_ERROR;
    }

    ngx_hex_dump(main_hash, r->cache->main, main_len);

    main_hash[main_len * 2] = '\0';

    v->data = main_hash;
    v->len = main_len * 2;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_upstream_cache_variant_hash(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char *variant_hash;
    size_t  variant_len;

    if (r->cache == NULL) {
        v->not_found = 1;
        return NGX_OK;
    }

    variant_len = NGX_HTTP_CACHE_KEY_LEN;

    variant_hash = ngx_pnalloc(r->pool, variant_len * 2 + 1);
    if (variant_hash == NULL) {
        return NGX_ERROR;
    }

    ngx_hex_dump(variant_hash, r->cache->variant, variant_len);

    variant_hash[variant_len * 2] = '\0';

    v->data = variant_hash;
    v->len = variant_len * 2;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_upstream_cache_file(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    ngx_http_cache_t  *c;

    c = r->cache;
    if (c == NULL || c->file.name.len == 0) {
        v->not_found = 1;
        return NGX_OK;
    }

    v->len = c->file.name.len;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = c->file.name.data;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_upstream_cache_age(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char  *p;
    time_t   now, age;

    if (r->upstream == NULL) {
        v->not_found = 1;
        return NGX_OK;
    }

    if (!r->cached
        || r->cache == NULL
        || r->upstream->cache_status == NGX_HTTP_CACHE_REVALIDATED)
    {
        v->not_found = 1;
        return NGX_OK;
    }

    p = ngx_pnalloc(r->pool, NGX_TIME_T_LEN);
    if (p == NULL) {
        return NGX_ERROR;
    }

    now = ngx_time();
    age = now - r->cache->date;

    if (r->cache->date > now) {
        age = 0;
    }

    v->len = ngx_sprintf(p, "%T", age) - p;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = p;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_upstream_cache_create_time(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char  *p;

    if (r->upstream == NULL) {
        v->not_found = 1;
        return NGX_OK;
    }

    if (!r->cached
        || r->cache == NULL
        || r->upstream->cache_status == NGX_HTTP_CACHE_REVALIDATED)
    {
        v->not_found = 1;
        return NGX_OK;
    }

    p = ngx_pnalloc(r->pool, NGX_TIME_T_LEN);
    if (p == NULL) {
        return NGX_ERROR;
    }

    v->len = ngx_sprintf(p, "%T", r->cache->date) - p;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = p;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_upstream_cache_create_date(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char     *p;

    if (r->upstream == NULL || r->cache == NULL) {
        v->not_found = 1;
        return NGX_OK;
    }

    if (!r->cached || r->upstream->cache_status == NGX_HTTP_CACHE_REVALIDATED) {
        v->not_found = 1;
        return NGX_OK;
    }

    p = ngx_pnalloc(r->pool, sizeof("Mon, 28 Sep 1970 06:00:00 GMT") - 1);
    if (p == NULL) {
        return NGX_ERROR;
    }

    v->len = ngx_http_time(p, r->cache->date) - p;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = p;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variables_check_cache_control(ngx_http_request_t *r)
{
    ngx_http_upstream_t  *u;
    ngx_table_elt_t      *cache_control;
    u_char               *p, *start, *last;
    ngx_int_t             n;
    ngx_uint_t            offset;

    if (r->cache == NULL) {
        return NGX_OK;
    }

    u = r->upstream;
    if (u == NULL) {
        return NGX_OK;
    }

    if (u->conf->ignore_headers & NGX_HTTP_UPSTREAM_IGN_CACHE_CONTROL) {
        return NGX_OK;
    }

    if (r->cache->valid_sec != 0 && u->headers_in.x_accel_expires != NULL) {
        return NGX_OK;
    }

    cache_control = u->headers_in.cache_control;
    if (cache_control == NULL) {
        return NGX_OK;
    }

    start = cache_control->value.data;
    last = start + cache_control->value.len;

#if (NGX_HTTP_EXT)
    if ((ngx_strlcasestrn(start, last, (u_char *) "no-cache", 8 - 1) != NULL 
            && !(u->conf->ignore_cache_control & NGX_HTTP_UPSTREAM_IGN_CC_NOCACHE))
        || (ngx_strlcasestrn(start, last, (u_char *) "no-store", 8 - 1) != NULL
            && !(u->conf->ignore_cache_control & NGX_HTTP_UPSTREAM_IGN_CC_NOSTORE))
        || (ngx_strlcasestrn(start, last, (u_char *) "private", 7 - 1) != NULL
            && !(u->conf->ignore_cache_control & NGX_HTTP_UPSTREAM_IGN_CC_PRIVATE)))
    {
        return NGX_OK;
    }
#else
    if (ngx_strlcasestrn(start, last, (u_char *) "no-cache", 8 - 1) != NULL
        || ngx_strlcasestrn(start, last, (u_char *) "no-store", 8 - 1) != NULL
        || ngx_strlcasestrn(start, last, (u_char *) "private", 7 - 1) != NULL)
    {
        return NGX_OK;
    }
#endif

    p = ngx_strlcasestrn(start, last, (u_char *) "s-maxage=", 9 - 1);
#if (NGX_HTTP_EXT)
    if (p && !(u->conf->ignore_cache_control & NGX_HTTP_UPSTREAM_IGN_CC_SMAXAGE))
    {
        offset = 9;
    }
    else if ((p = ngx_strlcasestrn(start, last, (u_char *) "max-age=", 7))
        && !(u->conf->ignore_cache_control & NGX_HTTP_UPSTREAM_IGN_CC_MAXAGE))
    {
        offset = 8;
    }
    else {
        p = NULL;
    }
#else
    offset = 9;

    if (p == NULL) {
        p = ngx_strlcasestrn(start, last, (u_char *) "max-age=", 8 - 1);
        offset = 8;
    }
#endif

    if (p == NULL) {
        return NGX_OK;
    }

    n = 0;

    for (p += offset; p < last; p++) {
        if (*p == ',' || *p == ';' || *p == ' ') {
            break;
        }

        if (*p >= '0' && *p <= '9') {
            n = n * 10 + (*p - '0');
            continue;
        }

        return NGX_OK;
    }

    if (n == 0) {
        return NGX_OK;
    }

    return NGX_DECLINED;
}


static ngx_int_t
ngx_http_extra_variables_check_accel_expires(ngx_http_request_t *r)
{
    ngx_http_upstream_t  *u;
    ngx_table_elt_t      *h;
    u_char               *p;
    size_t                len;
    ngx_int_t             n;

    if (r->cache == NULL) {
        return NGX_OK;
    }

    u = r->upstream;
    if (u == NULL) {
        return NGX_OK;
    }

    if (u->conf->ignore_headers & NGX_HTTP_UPSTREAM_IGN_XA_EXPIRES) {
        return NGX_OK;
    }

    h = u->headers_in.x_accel_expires;
    if (h == NULL) {
        return NGX_OK;
    }

    len = h->value.len;
    p = h->value.data;

    if (len == 0) {
        return NGX_OK;
    }

    if (p[0] != '@') {
        n = ngx_atoi(p, len);

        if (n > 0) {
            return NGX_DECLINED;
        }

        return NGX_OK;
    }

    return NGX_OK;
}


static time_t
ngx_http_cache_get_expire_time(ngx_http_request_t *r)
{
    time_t      expire_time;
    time_t      now;
    ngx_int_t   rc;

    if (r->cache == NULL) {
        return NGX_ERROR;
    }

    rc = ngx_http_extra_variables_check_accel_expires(r);
    if (rc == NGX_DECLINED) {
        goto new_expire;
    }

    rc = ngx_http_extra_variables_check_cache_control(r);
    if (rc == NGX_DECLINED) {
        goto new_expire;
    }

    return r->cache->valid_sec;

new_expire:

    now = ngx_time();
    expire_time = r->cache->valid_sec - (now - r->cache->date);

    if (expire_time < 0) {
        expire_time = 0;
    }

    return expire_time;
}


static ngx_int_t
ngx_http_extra_variable_upstream_cache_expire_time(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char     *p;
    time_t      expire_time;

    if (r->upstream == NULL) {
        v->not_found = 1;
        return NGX_OK;
    }

    if (!r->cached
        || r->cache == NULL
        || r->upstream->cache_status == NGX_HTTP_CACHE_REVALIDATED)
    {
        v->not_found = 1;
        return NGX_OK;
    }

    expire_time = ngx_http_cache_get_expire_time(r);

    p = ngx_pnalloc(r->pool, NGX_TIME_T_LEN);
    if (p == NULL) {
        return NGX_ERROR;
    }

    v->len = ngx_sprintf(p, "%T", expire_time) - p;
    v->data = p;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_upstream_cache_expire_date(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char     *p;
    time_t      expire_time;

    if (r->upstream == NULL || r->cache == NULL) {
        v->not_found = 1;
        return NGX_OK;
    }

    if (!r->cached || r->upstream->cache_status == NGX_HTTP_CACHE_REVALIDATED) {
        v->not_found = 1;
        return NGX_OK;
    }


    expire_time = ngx_http_cache_get_expire_time(r);

    p = ngx_pnalloc(r->pool, sizeof("Mon, 28 Sep 1970 06:00:00 GMT") - 1);
    if (p == NULL) {
        return NGX_ERROR;
    }

    v->len = ngx_http_time(p, expire_time) - p;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = p;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_upstream_cache_ttl(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char     *p;
    time_t      now, ttl, expire_time;

    if (r->upstream == NULL) {
        v->not_found = 1;
        return NGX_OK;
    }

    if (!r->cached
        || r->cache == NULL
        || r->upstream->cache_status == NGX_HTTP_CACHE_REVALIDATED)
    {
        v->not_found = 1;
        return NGX_OK;
    }

    p = ngx_pnalloc(r->pool, NGX_TIME_T_LEN);
    if (p == NULL) {
        return NGX_ERROR;
    }

    expire_time = ngx_http_cache_get_expire_time(r);
    now = ngx_time();
    ttl = expire_time - now;

    if (expire_time < now) {
        ttl = 0;
    }

    v->len = ngx_sprintf(p, "%T", ttl) - p;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = p;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_variable_upstream_cache_max_age(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char  *p;
    time_t   max_age, expire_time;

    if (r->upstream == NULL) {
        v->not_found = 1;
        return NGX_OK;
    }

    if (!r->cached
        || r->cache == NULL
        || r->upstream->cache_status == NGX_HTTP_CACHE_REVALIDATED)
    {
        v->not_found = 1;
        return NGX_OK;
    }

    p = ngx_pnalloc(r->pool, NGX_TIME_T_LEN);
    if (p == NULL) {
        return NGX_ERROR;
    }

    expire_time = ngx_http_cache_get_expire_time(r);
    max_age = expire_time - r->cache->date;

    v->len = ngx_sprintf(p, "%T", max_age) - p;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = p;

    return NGX_OK;
}
#endif