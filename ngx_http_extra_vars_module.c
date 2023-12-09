
/*
 * Copyright (C) Jonathan Kolb
 * Copyright (C) Hanada
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <nginx.h>

#define NGX_EXTRA_VAR_REDIRECT_COUNT                 0
#define NGX_EXTRA_VAR_SUBREQUEST_COUNT               1

#define NGX_EXTRA_VAR_REQUEST_CREATE_TS              0
#define NGX_EXTRA_VAR_REQUEST_HEADER_PARSE_DONE_TS   1
#define NGX_EXTRA_VAR_RESPONSE_HEADER_SENT_TS        2

#define NGX_EXTRA_VAR_UPSTREAM_CONNECT_START_TS      0
#define NGX_EXTRA_VAR_UPSTREAM_CONNECT_END_TS        1
#define NGX_EXTRA_VAR_UPSTREAM_SEND_START_TS         2
#define NGX_EXTRA_VAR_UPSTREAM_SEND_END_TS           3
#define NGX_EXTRA_VAR_UPSTREAM_HEADER_TS             4
#define NGX_EXTRA_VAR_UPSTREAM_RESPONSE_TS           5

static ngx_int_t ngx_http_extra_vars_add_variables(ngx_conf_t *cf);

static ngx_int_t ngx_extra_var_location_name(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_extra_var_uint(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_extra_var_connect_start_ts(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_extra_var_request_ts(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_extra_var_ext(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_extra_var_ignore_cache_control(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_extra_var_upstream_url(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_extra_var_upstream_ts(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);

#if (NGX_HTTP_CACHE)
static ngx_int_t ngx_extra_var_cache_file(ngx_http_request_t *r,
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
    { ngx_string("ext"), NULL, ngx_extra_var_ext, 0,
        NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("location_name"), NULL, ngx_extra_var_location_name, 0,
        NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("redirect_count"), NULL, ngx_extra_var_uint,
        NGX_EXTRA_VAR_REDIRECT_COUNT, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("subrequest_count"), NULL, ngx_extra_var_uint,
        NGX_EXTRA_VAR_SUBREQUEST_COUNT, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("connect_start_ts"), NULL, ngx_extra_var_connect_start_ts, 0,
        0, 0 },

    { ngx_string("request_create_ts"), NULL, ngx_extra_var_request_ts,
        NGX_EXTRA_VAR_REQUEST_CREATE_TS, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("request_header_parse_done_ts"), NULL, ngx_extra_var_request_ts,
        NGX_EXTRA_VAR_REQUEST_HEADER_PARSE_DONE_TS, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("response_header_sent_ts"), NULL, ngx_extra_var_request_ts,
        NGX_EXTRA_VAR_RESPONSE_HEADER_SENT_TS, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    {ngx_string("ignore_cache_control"), NULL, ngx_extra_var_ignore_cache_control, 0,
        NGX_HTTP_VAR_NOCACHEABLE, 0},

    { ngx_string("upstream_url"), NULL, ngx_extra_var_upstream_url, 0,
        NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_connect_start_ts"), NULL, ngx_extra_var_upstream_ts,
        NGX_EXTRA_VAR_UPSTREAM_CONNECT_START_TS, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_connect_end_ts"), NULL, ngx_extra_var_upstream_ts,
        NGX_EXTRA_VAR_UPSTREAM_CONNECT_START_TS, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_send_start_ts"), NULL, ngx_extra_var_upstream_ts,
        NGX_EXTRA_VAR_UPSTREAM_SEND_START_TS, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_send_end_ts"), NULL, ngx_extra_var_upstream_ts,
        NGX_EXTRA_VAR_UPSTREAM_SEND_END_TS, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_header_ts"), NULL, ngx_extra_var_upstream_ts,
        NGX_EXTRA_VAR_UPSTREAM_HEADER_TS, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_response_ts"), NULL, ngx_extra_var_upstream_ts,
        NGX_EXTRA_VAR_UPSTREAM_RESPONSE_TS, NGX_HTTP_VAR_NOCACHEABLE, 0 },

#if (NGX_HTTP_CACHE)
    { ngx_string("cache_file"), NULL, ngx_extra_var_cache_file, 0,
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
ngx_extra_var_ext(ngx_http_request_t *r,
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
ngx_extra_var_location_name(ngx_http_request_t *r,
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
ngx_extra_var_uint(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    ngx_uint_t  value;
    u_char     *p;

    p = ngx_pnalloc(r->pool, NGX_INT_T_LEN);
    if (p == NULL) {
        return NGX_ERROR;
    }

    switch (data) {
    case NGX_EXTRA_VAR_REDIRECT_COUNT:
        value = NGX_HTTP_MAX_URI_CHANGES + 1 - r->uri_changes;
        break;

    case NGX_EXTRA_VAR_SUBREQUEST_COUNT:
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
ngx_extra_var_connect_start_ts(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    ngx_msec_int_t   ms;
    u_char          *p;

    if (r->connection->start_time == 0) {
        ngx_str_null(v);

        return NGX_OK;
    }

    ms = r->connection->start_time;

    p = ngx_pnalloc(r->pool, NGX_TIME_T_LEN + 4);
    if (p == NULL) {
        return NGX_ERROR;
    }

    v->len = ngx_sprintf(p, "%T.%03M", (time_t) ms / 1000, ms % 1000) - p;
    v->data = p;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;
}


static ngx_int_t
ngx_extra_var_request_ts(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char         *p;

    p = ngx_pnalloc(r->pool, NGX_TIME_T_LEN + 4);
    if (p == NULL) {
        return NGX_ERROR;
    }

    switch (data) {
    case NGX_EXTRA_VAR_REQUEST_CREATE_TS:
        if (!r->start_sec || !r->start_msec) {
            v->not_found = 1;
            return NGX_OK;
        }
        p = ngx_sprintf(p, "%T.%03M", r->start_sec, r->start_msec);
        break;

    case NGX_EXTRA_VAR_REQUEST_HEADER_PARSE_DONE_TS:
        if (!r->parse_done_sec || !r->parse_done_msec) {
            v->not_found = 1;
            return NGX_OK;
        }
        p = ngx_sprintf(p, "%T.%03M", r->parse_done_sec, r->parse_done_msec);
        break;

    case NGX_EXTRA_VAR_RESPONSE_HEADER_SENT_TS:
        if (!r->header_sent_sec || !r->header_sent_msec) {
            v->not_found = 1;
            return NGX_OK;
        }
        p = ngx_sprintf(p, "%T.%03M", r->header_sent_sec, r->header_sent_msec);
        break;

    default:
        v->not_found = 1;
        return NGX_OK;
    }

    v->len = ngx_strlen(p);
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = p;

    return NGX_OK;
}


static ngx_int_t
ngx_extra_var_ignore_cache_control(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    if (r->upstream->conf->ignore_headers & NGX_HTTP_UPSTREAM_IGN_CACHE_CONTROL) {
        v->len = 1;
        v->valid = 1;
        v->no_cacheable = 0;
        v->not_found = 0;
        v->data = (u_char *) "1";
    } else {
        v->len = 1;
        v->valid = 1;
        v->no_cacheable = 0;
        v->not_found = 0;
        v->data = (u_char *) "0";
    }

    return NGX_OK;
}

static ngx_int_t
ngx_extra_var_upstream_ts(ngx_http_request_t *r,
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
        case NGX_EXTRA_VAR_UPSTREAM_CONNECT_START_TS:
            ms = state[i].connect_start_msec;

        case NGX_EXTRA_VAR_UPSTREAM_CONNECT_END_TS:
            ms = state[i].connect_end_msec;

        case NGX_EXTRA_VAR_UPSTREAM_SEND_START_TS:
            ms = state[i].send_start_msec;

        case NGX_EXTRA_VAR_UPSTREAM_SEND_END_TS:
            ms = state[i].send_end_msec;

        case NGX_EXTRA_VAR_UPSTREAM_HEADER_TS:
            ms = state[i].header_msec;

        case NGX_EXTRA_VAR_UPSTREAM_RESPONSE_TS:
            ms = state[i].response_msec;

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


static ngx_int_t 
ngx_extra_var_upstream_url(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    char                    *uri_separator;
    ngx_http_upstream_t     *u;
    ngx_str_t                upstream_url;

    u = r->upstream;

    if (u && u->peer.name) {

        uri_separator = "";

#if (NGX_HAVE_UNIX_DOMAIN)
        if (u->peer.sockaddr && u->peer.sockaddr->sa_family == AF_UNIX) {
            uri_separator = ":";
        }
#endif

        upstream_url.len = u->schema.len + u->peer.name->len + ngx_strlen(uri_separator) + u->uri.len;
        upstream_url.data = ngx_pnalloc(r->pool, upstream_url.len);
        if (upstream_url.data == NULL) {
            return NGX_ERROR;
        }

        ngx_snprintf(upstream_url.data, upstream_url.len, "%V%V%s%V",
                    &u->schema, u->peer.name, uri_separator, &u->uri);

        v->len = upstream_url.len;
        v->data = upstream_url.data;
        v->valid = 1;
        v->no_cacheable = 0;
        v->not_found = 0;
    } else {
        v->not_found = 1;
    }

    return NGX_OK;
}


#if (NGX_HTTP_CACHE)
static ngx_int_t
ngx_extra_var_cache_file(ngx_http_request_t *r,
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