
/*
 * Copyright (C) Jonathan Kolb
 * Copyright (C) Hanada
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <nginx.h>


#define NGX_HTTP_EXTRA_VAR_REDIRECT_COUNT                 0
#define NGX_HTTP_EXTRA_VAR_SUBREQUEST_COUNT               1

#define NGX_HTTP_EXTRA_VAR_REQUEST_CREATED_TS             0
#define NGX_HTTP_EXTRA_VAR_RESPONSE_HEADER_SENT_TS        1

#define NGX_HTTP_EXTRA_VAR_REQUEST_HANDlING_TIME          0
#define NGX_HTTP_EXTRA_VAR_RESPONSE_BODY_TIME             1


static ngx_int_t ngx_http_extra_vars_add_variables(ngx_conf_t *cf);

static ngx_int_t ngx_http_extra_var_request_path(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_var_request_args(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_var_request_is_args(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_var_request_has_args(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_var_current_uri(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_var_current_path(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_var_escaped_current_uri(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_var_escaped_current_path(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_var_has_args(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_var_dollar(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_var_sec(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_var_ext(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_var_is_internal(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_var_is_subrequest(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_var_location_name(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_var_uint(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_var_connection_established_ts(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_var_request_ts(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_var_request_time(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_var_ignore_cache_control(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_var_ignore_x_accel_expires(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_var_hostname_uppercase(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_var_hostname_lowercase(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_extra_var_time_rfc1123(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
#if (NGX_HTTP_CACHE)
static ngx_int_t ngx_http_extra_var_cache_file(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
#endif


static ngx_http_module_t  ngx_http_extra_vars_module_ctx = {
    ngx_http_extra_vars_add_variables,      /* preconfiguration */
    NULL,                                   /* postconfiguration */

    NULL,                                   /* create main configuration */
    NULL,                                   /* init main configuration */

    NULL,                                   /* create server configuration */
    NULL,                                   /* merge server configuration */

    NULL,                                   /* create location configuration */
    NULL                                    /* merge location configuration */
};


ngx_module_t  ngx_http_extra_vars_module = {
    NGX_MODULE_V1,
    &ngx_http_extra_vars_module_ctx,         /* module context */
    NULL,                                    /* module directives */
    NGX_HTTP_MODULE,                         /* module type */
    NULL,                                    /* init master */
    NULL,                                    /* init module */
    NULL,                                    /* init process */
    NULL,                                    /* init thread */
    NULL,                                    /* exit thread */
    NULL,                                    /* exit process */
    NULL,                                    /* exit master */
    NGX_MODULE_V1_PADDING
};


static ngx_http_variable_t  ngx_http_extra_vars[] = {
    { ngx_string("request_path"), NULL,
      ngx_http_extra_var_request_path, 0, 0, 0 },

    { ngx_string("request_args"), NULL,
      ngx_http_extra_var_request_args, 0, 0, 0 },

    { ngx_string("request_is_args"), NULL,
      ngx_http_extra_var_request_is_args, 0, 0, 0 },

    { ngx_string("request_has_args"), NULL, 
      ngx_http_extra_var_request_has_args, 0, 0, 0 },

    { ngx_string("current_uri"), NULL, 
      ngx_http_extra_var_current_uri, 0,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("current_path"), NULL,
      ngx_http_extra_var_current_path, 0,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("escaped_current_uri"), NULL, 
      ngx_http_extra_var_escaped_current_uri, 0,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("escaped_current_path"), NULL,
      ngx_http_extra_var_escaped_current_path, 0,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("has_args"), NULL, ngx_http_extra_var_has_args,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("dollar"), NULL, ngx_http_extra_var_dollar,
      0, 0, 0 },

    { ngx_string("sec"), NULL, ngx_http_extra_var_sec, 
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("ext"), NULL, ngx_http_extra_var_ext, 
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("is_internal"), NULL, ngx_http_extra_var_is_internal,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("is_subrequest"), NULL, ngx_http_extra_var_is_subrequest,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("location_name"), NULL, ngx_http_extra_var_location_name,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("redirect_count"), NULL, ngx_http_extra_var_uint,
      NGX_HTTP_EXTRA_VAR_REDIRECT_COUNT, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("subrequest_count"), NULL, ngx_http_extra_var_uint,
      NGX_HTTP_EXTRA_VAR_SUBREQUEST_COUNT, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("connection_established_ts"), NULL, 
      ngx_http_extra_var_connection_established_ts, 0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("request_created_ts"), NULL, ngx_http_extra_var_request_ts,
      NGX_HTTP_EXTRA_VAR_REQUEST_CREATED_TS, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("response_header_sent_ts"), NULL, ngx_http_extra_var_request_ts,
      NGX_HTTP_EXTRA_VAR_RESPONSE_HEADER_SENT_TS, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("request_handling_time"), NULL, ngx_http_extra_var_request_time,
      NGX_HTTP_EXTRA_VAR_REQUEST_HANDlING_TIME, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("response_body_time"), NULL, ngx_http_extra_var_request_time,
      NGX_HTTP_EXTRA_VAR_RESPONSE_BODY_TIME, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    {ngx_string("ignore_cache_control"), NULL,
      ngx_http_extra_var_ignore_cache_control, 0, NGX_HTTP_VAR_NOCACHEABLE, 0},

    {ngx_string("ignore_x_accel_expires"), NULL,
      ngx_http_extra_var_ignore_x_accel_expires, 0, NGX_HTTP_VAR_NOCACHEABLE, 0},

    {ngx_string("hostname_uppercase"), NULL, ngx_http_extra_var_hostname_uppercase,
      0, 0, 0},

    {ngx_string("hostname_lowercase"), NULL, ngx_http_extra_var_hostname_lowercase,
      0, 0, 0},

    { ngx_string("time_rfc1123"), NULL, ngx_http_extra_var_time_rfc1123,
      0, NGX_HTTP_VAR_NOCACHEABLE, 0 },

#if (NGX_HTTP_CACHE)
    { ngx_string("cache_file"), NULL, ngx_http_extra_var_cache_file, 0,
        NGX_HTTP_VAR_NOCACHEABLE, 0 },
#endif

    { ngx_null_string, NULL, NULL, 0, 0, 0 }
};


static ngx_int_t
ngx_http_extra_vars_add_variables(ngx_conf_t *cf)
{
    ngx_http_variable_t  *var, *v;

    for (v = ngx_http_extra_vars; v->name.len; v++) {
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
ngx_http_extra_var_request_path(ngx_http_request_t *r,
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
ngx_http_extra_var_request_args(ngx_http_request_t *r,
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
ngx_http_extra_var_request_is_args(ngx_http_request_t *r,
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
ngx_http_extra_var_request_has_args(ngx_http_request_t *r,
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
ngx_http_extra_var_current_uri(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data) {
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
ngx_http_extra_var_current_path(ngx_http_request_t *r, 
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
ngx_http_extra_var_escaped_current_uri(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    ngx_uint_t escape;
    size_t uri_len;
    u_char *p;

    escape = 2 * ngx_escape_uri(NULL, r->uri.data, r->uri.len, NGX_ESCAPE_URI);

    uri_len = r->uri.len + escape;
    
    if (r->args.len > 0) {
        uri_len += 1 + r->args.len;
    }

    v->data = ngx_pnalloc(r->pool, uri_len + 1);
    if (v->data == NULL) {
        return NGX_ERROR;
    }

    p = ngx_escape_uri(v->data, r->uri.data, r->uri.len, NGX_ESCAPE_URI);

    if (r->args.len > 0) {
        *p++ = '?';
        ngx_memcpy(p, r->args.data, r->args.len);
        p += r->args.len;
    }

    *p = '\0';

    v->len = p - v->data;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_var_escaped_current_path(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    ngx_uint_t escape;
    size_t uri_len;
    u_char *p;

    escape = 2 * ngx_escape_uri(NULL, r->uri.data, r->uri.len, NGX_ESCAPE_URI);

    uri_len = r->uri.len + escape;

    v->data = ngx_pnalloc(r->pool, uri_len + 1);
    if (v->data == NULL) {
        return NGX_ERROR;
    }

    p = ngx_escape_uri(v->data, r->uri.data, r->uri.len, NGX_ESCAPE_URI);

    *p = '\0';

    v->len = p - v->data;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}


static ngx_int_t 
ngx_http_extra_var_dollar(ngx_http_request_t *r, 
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
ngx_http_extra_var_sec(ngx_http_request_t *r,
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
ngx_http_extra_var_ext(ngx_http_request_t *r,
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
ngx_http_extra_var_has_args(ngx_http_request_t *r,
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
ngx_http_extra_var_is_internal(ngx_http_request_t *r,
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
ngx_http_extra_var_is_subrequest(ngx_http_request_t *r,
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
ngx_http_extra_var_location_name(ngx_http_request_t *r,
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
ngx_http_extra_var_uint(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    ngx_uint_t  value;
    u_char     *p;

    p = ngx_pnalloc(r->pool, NGX_INT_T_LEN);
    if (p == NULL) {
        return NGX_ERROR;
    }

    switch (data) {
    case NGX_HTTP_EXTRA_VAR_REDIRECT_COUNT:
        value = NGX_HTTP_MAX_URI_CHANGES + 1 - r->uri_changes;
        break;

    case NGX_HTTP_EXTRA_VAR_SUBREQUEST_COUNT:
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
ngx_http_extra_var_connection_established_ts(ngx_http_request_t *r,
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
            (tp->sec * 1000 + tp->msec + r->connection->start_time - ngx_current_msec);

    v->len = ngx_sprintf(p, "%T.%03M", (time_t) ms / 1000, ms % 1000) - p;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = p;

    return NGX_OK;
}


static ngx_int_t
ngx_http_extra_var_request_ts(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char         *p;

    p = ngx_pnalloc(r->pool, NGX_TIME_T_LEN + 4);
    if (p == NULL) {
        return NGX_ERROR;
    }

    switch (data) {
    case NGX_HTTP_EXTRA_VAR_REQUEST_CREATED_TS:
        if (!r->start_sec || !r->start_msec) {
            v->not_found = 1;
            return NGX_OK;
        }
        v->len = ngx_sprintf(p, "%T.%03M", r->start_sec, r->start_msec) - p;
        break;

    case NGX_HTTP_EXTRA_VAR_RESPONSE_HEADER_SENT_TS:
        if (!r->header_sent_sec || !r->header_sent_msec) {
            v->not_found = 1;
            return NGX_OK;
        }
        v->len = ngx_sprintf(p, "%T.%03M", r->header_sent_sec, r->header_sent_msec) - p;
        break;

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


static ngx_int_t
ngx_http_extra_var_request_time(ngx_http_request_t *r,
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
    case NGX_HTTP_EXTRA_VAR_REQUEST_HANDlING_TIME:
        ms = (ngx_msec_int_t) 
                 ((r->header_sent_sec - r->start_sec) * 1000 + (r->header_sent_msec - r->start_msec));
        break;

    case NGX_HTTP_EXTRA_VAR_RESPONSE_BODY_TIME:
        ms = (ngx_msec_int_t) 
                 ((tp->sec - r->header_sent_sec ) * 1000 + (tp->msec - r->header_sent_msec));
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


static ngx_int_t
ngx_http_extra_var_ignore_cache_control(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    if (r->upstream->conf->ignore_headers & NGX_HTTP_UPSTREAM_IGN_CACHE_CONTROL) {
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
ngx_http_extra_var_ignore_x_accel_expires(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    if (r->upstream->conf->ignore_headers & NGX_HTTP_UPSTREAM_IGN_XA_EXPIRES) {
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
ngx_http_extra_var_hostname_uppercase(ngx_http_request_t *r,
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
ngx_http_extra_var_hostname_lowercase(ngx_http_request_t *r,
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
ngx_http_extra_var_time_rfc1123(ngx_http_request_t *r,
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


#if (NGX_HTTP_CACHE)
static ngx_int_t
ngx_http_extra_var_cache_file(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    ngx_http_cache_t  *c;

    c = r->cache;
    if (c == NULL || 0 == c->file.name.len) {
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
#endif