
/*
 * Copyright (C) Hanada
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <nginx.h>

#define NGX_EXTRA_VAR_TIME_NOW       0
#define NGX_EXTRA_VAR_TIME_ELAPSED   1
#define NGX_EXTRA_VAR_TIME_REQUEST   2

#define NGX_EXTRA_VAR_REDIRECT_COUNT         0
#define NGX_EXTRA_VAR_SUBREQUEST_COUNT       1
#define NGX_EXTRA_VAR_CONNECTION_REQUESTS    2

static ngx_int_t ngx_http_extra_vars_add_variables(ngx_conf_t *cf);

static ngx_int_t ngx_extra_var_location_name(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_extra_var_time_msec(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_extra_var_uint(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_extra_var_ext(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_extra_var_ignore_cache_control(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_extra_var_upstream_method(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_extra_var_upstream_url(ngx_http_request_t *r,
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

    { ngx_string("request_received_ts"), NULL, ngx_extra_var_time_msec,
        NGX_EXTRA_VAR_TIME_REQUEST, 0, 0 },

    { ngx_string("subrequest_count"), NULL, ngx_extra_var_uint,
        NGX_EXTRA_VAR_SUBREQUEST_COUNT, NGX_HTTP_VAR_NOCACHEABLE, 0 },

    {ngx_string("ignore_cache_control"), NULL, ngx_extra_var_ignore_cache_control, 0,
        NGX_HTTP_VAR_NOCACHEABLE, 0},

    { ngx_string("upstream_method"), NULL, ngx_extra_var_upstream_method, 0,
        NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_url"), NULL, ngx_extra_var_upstream_url, 0,
        NGX_HTTP_VAR_NOCACHEABLE, 0 },

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
ngx_extra_var_time_msec(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char         *p;
    ngx_time_t     *tp, t;
    ngx_msec_int_t  ms;

    p = ngx_pnalloc(r->pool, NGX_TIME_T_LEN + 4);
    if (p == NULL) {
        return NGX_ERROR;
    }

    switch (data) {
    case NGX_EXTRA_VAR_TIME_NOW :
        tp = ngx_timeofday();
        break;

    case NGX_EXTRA_VAR_TIME_ELAPSED :
        tp = ngx_timeofday();

        ms = (ngx_msec_int_t) ((tp->sec - r->start_sec) * 1000
                 + (tp->msec - r->start_msec));
        ms = ngx_max(ms, 0);

        tp = &t;
        tp->sec = ms / 1000;
        tp->msec = ms % 1000;
        break;

    case NGX_EXTRA_VAR_TIME_REQUEST :
        tp = &t;
        tp->sec = r->start_sec;
        tp->msec = r->start_msec;
        break;

    default :
        v->not_found = 1;
        return NGX_OK;
    }

    v->len = ngx_sprintf(p, "%T.%03M", tp->sec, tp->msec) - p;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = p;

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

    case NGX_EXTRA_VAR_CONNECTION_REQUESTS:
        value = r->connection->requests;
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
ngx_extra_var_upstream_url(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    char                      *uri_separator;
    ngx_http_upstream_t       *u;
    ngx_str_t                 upstream_url;

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


static ngx_int_t 
ngx_extra_var_upstream_method(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    if (r->upstream) {
        v->len = r->upstream->method.len;
        v->data = r->upstream->method.data;
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